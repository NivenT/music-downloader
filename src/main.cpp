#include <iostream>
#include <fstream>
#include <cstring>

#include <docopt.h>

#include "util.h"
#include "download_song.h"
#include "lyrics.h"
#include "play.h"
#include "search_and_play.h"
#include "ytconverter.h"
#include "youtube.h"

using namespace std;

extern std::vector<YTConverter*> converters;

// TODO: Give --play-song a better name
static const char* USAGE =
R"({progName}

Usage:
    {progName} (-h | --help)
    {progName} [--api-key KEY | --api-file FILE] [--songs FILE] [--dest FOLDER] [-v | --verbose]
    {progName} [--api-key KEY | --api-file FILE] --download SONGS... [--dest FOLDER] [-v | --verbose]
    {progName} [--api-key KEY | --api-file FILE] --play-song SONGS... [--keep] [--show-lyrics] [-v | --verbose]
    {progName} --yt-ids SONGS... [--dest FOLDER] [-v | --verbose]
    {progName} --lyrics SONG [--save FILE] [--hide] [-v | --verbose]
    {progName} --play FILES... [--dir FOLDER] [--show-lyrics] [--show-play-output] [-v | --verbose]
    {progName} --play-from-ids SONGS... [--keep] [--show-lyrics] [-v | --verbose]

Options:
    -h --help                   Prints this message.
    --songs FILE                Text file containing songs to download [default: songs.txt]
    --dest FOLDER               Destination folder (where downloaded songs are saved) [default: songs/]
    -v --verbose                Use verbose output
    --lyrics SONG               Name of song to find the lyrics of [default: ]
    --save FILE                 File to save the lyrics to [default: ]
    --hide                      Doesn't print the lyrics to the terminal
    --download SONGS...         List of songs to download
    --play FILES...             List of MP3 files to play
    --dir FOLDER                The folder containing the files to play [default: .]
    --show-lyrics               Prints lyrics of song to the screen
    --show-play-output          Does not use quiet flag when running play command
    --play-song SONGS...        List of songs to search for online and then play if found
    --keep                      Keep a saved .mp3 of the song
    --api-key KEY               The YouTube API key to use
    --api-file FILE             A file containing the YouTube API key
    --yt-ids SONGS...           List of ids (the part after "v=" in their YouTube URL) of YouTube videos to download
    --play-from-ids SONGS...    List of ids of YouTube videos to download and then play
)";

// TODO: Restructure this in a slightly cleaner way
vector<const char*> splitArgs(int argc, const char** argv, const char* type) {
    vector<const char*> args;
    for (int i = 0; i < argc; i++) {
        while (strcmp(argv[i], type) == 0) {
            while (argv[++i] && !starts_with(argv[i], "-")) {
                args.push_back(type);
                args.push_back(argv[i]);
            }
            if (i >= argc) break;
        }
        if (i < argc) args.push_back(argv[i]);
    }
    return args;
}

string get_key(docopt::value key, const docopt::value& file_name) {
    if (key.isString() && key.asString() != "") return key.asString();
    if (file_name.isString() && file_name.asString() != "") {
        string ret;
        read_file(file_name.asString(), ret);
        return ret;
    }
    return "";
}

int main(int argc, const char** argv) {
    static const string stars(100, '*');
    srand(time(nullptr));

    auto vargv = splitArgs(argc, argv, "--play");
    vargv = splitArgs(vargv.size(), &vargv[0], "--play-song");
    vargv = splitArgs(vargv.size(), &vargv[0], "--download");
    vargv = splitArgs(vargv.size(), &vargv[0], "--yt-ids");
    vargv = splitArgs(vargv.size(), &vargv[0], "--play-from-ids");

    map<string, docopt::value> args =
        docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
                                   {vargv.data()+1, vargv.data() + vargv.size()});

    string songList = args["--songs"].asString(), 
           saveFolder = args["--dest"].asString(),
           song = args["--lyrics"].asString(),
           saveFile = args["--save"].asString(),
           songFolder = args["--dir"].asString(),
           apikey = trim(get_key(args["--api-key"], args["--api-file"]));

    bool print = !args["--hide"].asBool(),
         verbose = args["--verbose"].asBool(),
         lyrics = args["--show-lyrics"].asBool(),
         playout = args["--show-play-output"].asBool(),
         keep = args["--keep"].asBool();

    vector<string> songs = args["--download"].asStringList(),
                   yt_ids = args["--yt-ids"].asStringList();

    map<string, set<string>> stats;
    saveFolder += ends_with(saveFolder, "/") ? "" : "/";
    songFolder += ends_with(songFolder, "/") ? "" : "/";

    converters.push_back(new ThreeTwentyYT);
    converters.push_back(new PointMP3);
    converters.push_back(new ConvertMP3);

    //if (verbose) cout<<"Using APIKEY: \""<<apikey<<"\""<<endl;

    if (song != "") {
        find_lyrics(song, saveFile, print, verbose);
    } else if (!songs.empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            cout<<"Downloading \""<<songs[i]<<"\" and saving song in \""<<saveFolder<<"\""<<endl
                <<endl;
            download_song(songs[i], saveFolder, verbose, stats, apikey);
        }
    } else if (!(songs = args["--play"].asStringList()).empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            play_song(songFolder + songs[i], lyrics, playout, verbose);
        }
    } else if (!(songs = args["--play-song"].asStringList()).empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            search_and_play(songs[i], keep, lyrics, verbose, apikey);
        }
    } else if (!yt_ids.empty()) {
        cout<<"Downloading songs and saving them in \""<<saveFolder<<"\""<<endl
            <<endl;
        for (const auto& id : yt_ids) {
            string title = get_title_from_id(id, verbose);
            download_song_given_id(id, title, saveFolder, verbose, stats);
            cout<<endl;
        }
        print_statistics(stats);
    } else if (!(yt_ids = args["--play-from-ids"].asStringList()).empty()) {
        for (int i = 0; i < yt_ids.size(); i++) {
            cout<<stars<<endl;
            search_and_play_given_id(yt_ids[i], keep, lyrics, verbose);
        }
    } else {
        cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<endl
            <<endl;

        ifstream songFile(songList.c_str());

        string song;
        while (getline(songFile, song)) {
            if (!starts_with(song, "added on:")) {
                // just to make sure statistics print in alphabetical order
                transform(song.begin(), song.end(), song.begin(), ::tolower);

                download_song(song, saveFolder, verbose, stats, apikey);
                cout<<endl;
            }
        }
        print_statistics(stats);
    }

    for (auto& converter : converters) {
        delete converter;
    }
    return 0;
}
