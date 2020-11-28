#include <iostream>

#include "util.h"
#include "search_and_play.h"
#include "download_song.h"
#include "play.h"
#include "youtube.h"
#include "lyrics.h"

using namespace std;

static void play(map<string, set<string>>& stats, bool show_lyrics, bool keep,
                 const string& fileTitle, const string& song, bool verbose) {
    if (!stats[DOWNLOAD_MISTAKE_MSG].empty() || !stats[DOWNLOAD_SUCC_MSG].empty()
        || !stats[ALREADY_EXISTED_MSG].empty()) {
        if (show_lyrics) {
            find_lyrics(trim(song), "", true, verbose);
        }
        play_song(fileTitle, false, false, verbose);
        if (!keep) {
            if (verbose) {
                cout<<"Deleting file "<<fileTitle<<endl;
            }
            remove(fileTitle.c_str());
        }
    } else {
        cout<<"Could not find "<<song<<endl;
    }
}

void search_and_play(const string& song, bool keep, bool show_lyrics, 
                     bool verbose, const string& apikey) {
    map<string, set<string>> stats;
    string fileTitle = keep ? fileify(song) : gen_tmp_file_title();

    download_song(song, fileTitle, verbose, stats, apikey);
    play(stats, show_lyrics, keep, fileTitle, song, verbose);
}

void search_and_play_given_id(const string& id, bool keep, bool show_lyrics,
                              bool verbose) {
    map<string, set<string>> stats;

    string songTitle = get_title_from_id(id, verbose);
    string fileTitle = keep ? fileify(songTitle) : gen_tmp_file_title();

    download_song_given_id(id, songTitle, songTitle, fileTitle, verbose, stats);
    play(stats, show_lyrics, keep, fileTitle, songTitle, verbose);
}