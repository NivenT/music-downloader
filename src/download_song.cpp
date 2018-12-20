#include <iostream>
#include <fstream>

#include "util.h"
#include "youtube.h"
#include "web.h"
#include "download_song.h"

using namespace std;

void download_song_from_youtube(const string& song, const string& file_pattern, 
                                bool verbose, map<string, set<string>>& stats) {
    static const float SIMILARITY_THRESHOLD = 0.63;

    string songId, songTitle;
    tie(songId, songTitle) = search_youtube_for_song(song, verbose);

    string folder, file;
    tie(folder, file) = split_path(file_pattern);
    folder = folder == "" ? "./" : folder;
    string fileTitle = folder + (file == "" ? fileify(songTitle) : file);

    string match;
    if (songId == "") {
        cout<<"\""<<song<<"\" could not be found"<<endl;

        stats[NOT_FOUND_MSG].insert(song);
    } else if (song_exists(fileTitle)) {
        cout<<"\""<<song<<"\" has already been downloaded"<<endl;

        stats[ALREADY_EXISTED_MSG].insert(song);
    } else if ((match = song_probably_exists(fileTitle, folder)) != "") {
        cout<<"\""<<song<<"\" has likely already been downloaded"<<endl
            <<"\""<<match<<"\" was found which is a close match"<<endl;

        stats[MAYBE_ALREADY_EXISTED_MSG].insert(song + " -> " + match);
    } else {
        if (verbose) {
            cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<endl;
        }

        const string downloadUrl = youtube_to_download(songId);
        if (downloadUrl == "") {
            cout<<"Could not find song"<<endl
                <<endl;

            stats[NOT_FOUND_MSG].insert(song);
            return;
        } else if (verbose) {
            cout<<TAB<<"Donwload url: "<<downloadUrl<<endl;
        }

        string songData; bool succ;
        tie(succ, songData) = download_song(downloadUrl);
        if (succ) {
            cout<<"Successfully downloaded "<<songTitle<<endl;
            write_to_mp3(fileTitle, songData, verbose);

            if (title_distance(song, songTitle) >= SIMILARITY_THRESHOLD) {
                stats[DOWNLOAD_MISTAKE_MSG].insert(song + " -> " + songTitle);
            } else {
                stats[DOWNLOAD_SUCC_MSG].insert(song);
            }
        } else {
            cout<<"Failed to download "<<songTitle<<endl;

            stats[DOWNLOAD_FAIL_MSG].insert(song);
        }
    }
}

void download_song(const string& song, const string& saveFolder, 
                    bool verbose, map<string, set<string>>& stats) {
    download_song_from_youtube(song, saveFolder, verbose, stats);
}

void download_songs(const string& songList, const string& saveFolder, 
                    bool verbose, map<string, set<string>>& stats) {
    cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<endl
             <<endl;

    ifstream songFile(songList.c_str());

    string song;
    while (getline(songFile, song)) {
        if (!starts_with(song, "added on:")) {
            // just to make sure statistics print in alphabetical order
            transform(song.begin(), song.end(), song.begin(), ::tolower);

            download_song(song, saveFolder, verbose, stats);
            cout<<endl;
        }
    }
}

void print_statistics(map<string, set<string>> stats) {
    static const int PRINT_THRESHOLD = 15;

    cout<<"*******Download summary*******"<<endl
        <<endl;
    for (const auto& pair : stats) {
        const string& msg = pair.first;
        const set<string>& data = pair.second;

        cout<<data.size()<<" songs "<<msg<<". They were:"<<endl;
        if (data.size() <= PRINT_THRESHOLD) {
            for (const auto& song : data) {
                cout<<TAB<<song<<endl;
            }
        } else {
            int nend = 0.3*PRINT_THRESHOLD;
            int count = PRINT_THRESHOLD-nend;

            for (auto it = data.begin(); count > 0; ++it, --count) {
                cout<<TAB<<(*it)<<endl;
            }
            cout<<TAB<<"."<<endl
                <<TAB<<"."<<endl
                <<TAB<<"."<<endl;

            auto it = data.begin();
            for (int skip = data.size() - nend; skip > 0; ++it, --skip);
            while (nend-- > 0) cout<<TAB<<*(it++)<<endl;
        }
        cout<<endl;
    }
}
