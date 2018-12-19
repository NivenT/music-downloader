#include <iostream>
#include <cstring>

#include <docopt.h>

#include "util.h"
#include "download_song.h"
#include "lyrics.h"
#include "play.h"

using namespace std;

static const char* USAGE =
R"({progName}

Usage:
  {progName} (-h | --help)
  {progName} [--songs FILE] [--dest FOLDER] [-v | --verbose]
  {progName} --lyrics SONG [--save FILE] [--hide] [-v | --verbose]
  {progName} --download SONG [--dest FOLDER] [-v | --verbose]
  {progName} --play FILES... [--dir FOLDER] [--show-lyrics] [--show-play-output] [-v | --verbose]

Options:
  -h --help             Prints this message.
  --songs FILE          Text file containing songs to download [default: songs.txt]
  --dest FOLDER         Destination folder (where downloaded songs are saved) [default: songs/]
  -v --verbose          Use verbose output
  --lyrics SONG         Name of song to find the lyrics of [default: ]
  --save FILE           File to save the lyrics to [default: ]
  --hide                Doesn't print the lyrics to the terminal
  --download SONG       Downloads a single song [default: ]
  --play FILES...       List of MP3 files to play
  --dir FOLDER          The folder containing the files to play [default: .]
  --show-lyrics         Prints lyrics of song to the screen
  --show-play-output    Does not use quiet flag when running play command
)";

// TODO: Restructure this in a slightly cleaner way
vector<char*> splitPlayArgs(int argc, char** argv) {
    vector<char*> args;
    for (int i = 0; i < argc; i++) {
        while (strcmp(argv[i], "--play") == 0) {
            while (argv[++i] && !starts_with(argv[i], "-")) {
                args.push_back(((char*)"--play"));
                args.push_back(argv[i]);
            }
        }
        if (i < argc) args.push_back(argv[i]);
    }
    return args;
}

int main(int argc, char** argv) {
    auto vargv = splitPlayArgs(argc, argv);
    map<string, docopt::value> args =
        docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
                       {vargv.data()+1, vargv.data() + vargv.size()});

    string apikey = "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc", 
           songList = args["--songs"].asString(), 
           saveFolder = args["--dest"].asString(),
           song = args["--lyrics"].asString(),
           saveFile = args["--save"].asString(),
           songFolder = args["--dir"].asString();

    bool print = !args["--hide"].asBool(),
         verbose = args["--verbose"].asBool(),
         lyrics = args["--show-lyrics"].asBool(),
         playout = args["--show-play-output"].asBool();

    vector<string> songs = args["--play"].asStringList();

    map<string, set<string>> stats;
    saveFolder += ends_with(saveFolder, "/") ? "" : "/";
    songFolder += ends_with(songFolder, "/") ? "" : "/"; 

    if (song != "") {
        find_lyrics(song, saveFile, print, verbose);
    } else if ((song = args["--download"].asString()) != "") {
        cout<<"Downloading \""<<song<<"\" and saving song in \""<<saveFolder<<"\""<<endl
             <<endl;
        download_song(apikey, song, saveFolder, verbose, stats);
    } else if (!songs.empty()) {
        static const string stars(100, '*');

        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            play_song(songFolder + songs[i], lyrics, playout, verbose);
        }
    } else {
        download_songs(apikey, songList, saveFolder, verbose, stats);
        print_statistics(stats);
    }
    return 0;
}
