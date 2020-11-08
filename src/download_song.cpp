#include <iostream>
#include <fstream>

#include "util.h"
#include "youtube.h"
#include "web.h"
#include "download_song.h"
#include "ytconverter.h"

using namespace std;

// Gotta love globals
// I think writing C code recently has made me much more accepting of globals
std::vector<YTConverter*> converters;

void download_song_given_id(const string& songId, const string& songTitle,
                            const string& saveFolder, bool verbose,
                            map<string, set<string>>& stats) {
    static const float SIMILARITY_THRESHOLD = 0.63;

    string folder, file;
    tie(folder, file) = split_path(saveFolder);
    folder = folder == "" ? "./" : folder;
    string fileTitle = folder + (file == "" ? fileify(songTitle) : file);

    const string& song = songTitle;
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
            cout<<"Downloading video with Id "<<songId<<"..."<<endl;
        }

        for (int i = 0; i < converters.size(); i++) {
            auto& converter = converters[i];
            cout<<TAB<<"Attempting to use "<<converter->get_name()<<" to download video..."<<endl;
            const string downloadUrl = converter->get_link(songId);
            if (downloadUrl == "") {
                cout<<TAB<<"Could not find song"<<endl
                    <<endl;
                continue;

                // TODO: Properly collect stats
                stats[NOT_FOUND_MSG].insert(song);
                return;
            } else if (verbose) {
                cout<<TAB<<TAB<<"Download url: "<<downloadUrl<<endl;
            }

            string songData; bool succ;
            tie(succ, songData) = converter->download_song(downloadUrl, verbose);
            if (succ) {
                cout<<TAB<<"Successfully downloaded "<<songTitle<<endl;
                write_to_mp3(fileTitle, songData, verbose);

                if (title_distance(song, songTitle) >= SIMILARITY_THRESHOLD) {
                    stats[DOWNLOAD_MISTAKE_MSG].insert(song + " -> " + songTitle);
                } else {
                    stats[DOWNLOAD_SUCC_MSG].insert(song);
                }
                break;
            } else {
                cout<<TAB<<converter->get_name()<<" failed to download "<<songTitle<<endl;
                if (i == converters.size() - 1) stats[DOWNLOAD_FAIL_MSG].insert(song);
            }
        }
    }
}

void download_song(const string& song, const string& saveFolder, 
                    bool verbose, map<string, set<string>>& stats,
                    const string& apikey) {
    string songId, songTitle;
    tie(songId, songTitle) = search_youtube_for_song(song, verbose, apikey);

    download_song_given_id(songId, songTitle, saveFolder, verbose, stats);
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
