#include <iostream>
#include <cstring>

#include <docopt.h>

#include "util.h"
#include "download_song.h"
#include "lyrics.h"
#include "play.h"
#include "search_and_play.h"

using namespace std;

// TODO: Give --play-song a better name
static const char* USAGE =
R"({progName}

Usage:
    {progName} (-h | --help)
    {progName} [--songs FILE] [--dest FOLDER] [-v | --verbose]
    {progName} --lyrics SONG [--save FILE] [--hide] [-v | --verbose]
    {progName} --download SONGS... [--dest FOLDER] [-v | --verbose]
    {progName} --play FILES... [--dir FOLDER] [--show-lyrics] [--show-play-output] [-v | --verbose]
    {progName} --play-song SONGS... [--keep] [--show-lyrics] [-v | --verbose]

Options:
    -h --help             Prints this message.
    --songs FILE          Text file containing songs to download [default: songs.txt]
    --dest FOLDER         Destination folder (where downloaded songs are saved) [default: songs/]
    -v --verbose          Use verbose output
    --lyrics SONG         Name of song to find the lyrics of [default: ]
    --save FILE           File to save the lyrics to [default: ]
    --hide                Doesn't print the lyrics to the terminal
    --download SONGS...   List of songs to download
    --play FILES...       List of MP3 files to play
    --dir FOLDER          The folder containing the files to play [default: .]
    --show-lyrics         Prints lyrics of song to the screen
    --show-play-output    Does not use quiet flag when running play command
    --play-song SONGS...  Song to search for online and then play if found
    --keep                Keep a saved .mp3 of the song
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

int main(int argc, const char** argv) {
    static const string stars(100, '*');
    srand(time(nullptr));

    auto vargv = splitArgs(argc, argv, "--play");
    vargv = splitArgs(vargv.size(), &vargv[0], "--play-song");
    vargv = splitArgs(vargv.size(), &vargv[0], "--download");

    map<string, docopt::value> args =
        docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
                                   {vargv.data()+1, vargv.data() + vargv.size()});

    string songList = args["--songs"].asString(), 
           saveFolder = args["--dest"].asString(),
           song = args["--lyrics"].asString(),
           saveFile = args["--save"].asString(),
           songFolder = args["--dir"].asString();

    bool print = !args["--hide"].asBool(),
         verbose = args["--verbose"].asBool(),
         lyrics = args["--show-lyrics"].asBool(),
         playout = args["--show-play-output"].asBool(),
         keep = args["--keep"].asBool();

    vector<string> songs = args["--download"].asStringList();

    map<string, set<string>> stats;
    saveFolder += ends_with(saveFolder, "/") ? "" : "/";
    songFolder += ends_with(songFolder, "/") ? "" : "/"; 

    if (song != "") {
        find_lyrics(song, saveFile, print, verbose);
    } else if (!songs.empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            cout<<"Downloading \""<<songs[i]<<"\" and saving song in \""<<saveFolder<<"\""<<endl
                <<endl;
            download_song(songs[i], saveFolder, verbose, stats);
        }
    } else if (!(songs = args["--play"].asStringList()).empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            play_song(songFolder + songs[i], lyrics, playout, verbose);
        }
    } else if (!(songs = args["--play-song"].asStringList()).empty()) {
        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            search_and_play(songs[i], keep, lyrics, verbose);
        }
    } else {
        download_songs(songList, saveFolder, verbose, stats);
        print_statistics(stats);
    }
    return 0;
}
