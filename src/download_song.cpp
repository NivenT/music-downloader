#include <iostream>
#include <fstream>

#include "util.h"
#include "youtube.h"
#include "web.h"
#include "download_song.h"

#define ILLEGAL_CHARCTERS {".", "|", ":", "\"", "'", "(", ")", "&", "[", "]"}

using namespace std;

void download_song(const string& apikey, const string& song, const string& saveFolder, 
                    bool verbose, map<string, set<string>>& stats) {
    static const float SIMILARITY_THRESHOLD = 0.63;

    string songId, songTitle;
    tie(songId, songTitle) = search_youtube_for_song(song, apikey, verbose);

    string fileTitle = saveFolder + replace_all(songTitle, {{"/", "\\"}, ILLEGAL_CHARCTERS}, {"_", ""});
    string match;

    if (songId == "") {
        cout<<"\""<<song<<"\" could not be found"<<endl;

        stats["not found"].insert(song);
    } else if (song_exists(fileTitle)) {
        cout<<"\""<<song<<"\" has already been downloaded"<<endl;

        stats["already existed"].insert(song);
    } else if ((match = song_probably_exists(fileTitle, saveFolder)) != "") {
        cout<<"\""<<song<<"\" has likely already been downloaded"<<endl
            <<"\""<<match<<"\" was found which is a close match"<<endl;

        stats["probably already existed, and so were not downloaded"].insert(song + " -> " + match);
    } else {
        if (verbose) {
            cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<endl;
        }

        const string downloadUrl = youtube_to_download(songId);
        if (downloadUrl == "") {
            cout<<"Could not find song"<<endl
                <<endl;

            stats["not found"].insert(song);
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
                stats["downloaded, but were likely not the songs you wanted"].insert(song + " -> " + songTitle);
            } else {
                stats["successfully downloaded"].insert(song);
            }
        } else {
            cout<<"Failed to download "<<songTitle<<endl;

            stats["could not be downloaded"].insert(song);
        }
    }
}

void download_songs(const string& apikey, const string& songList, const string& saveFolder, 
                    bool verbose, map<string, set<string>>& stats) {
    cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<endl
             <<endl;

    ifstream songFile(songList.c_str());

    string song;
    while (getline(songFile, song)) {
        if (!starts_with(song, "added on:")) {
            // just to make sure statistics print in alphabetical order
            transform(song.begin(), song.end(), song.begin(), ::tolower);

            download_song(apikey, song, saveFolder, verbose, stats);
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