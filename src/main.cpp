// TODO: Organize code in the file/pull some functions out into their own file

#include <iostream>
#include <fstream>

#include <docopt.h>

#include "util.h"
#include "web.h"
#include "youtube.h"
#include "lyrics.h"

#define ILLEGAL_CHARCTERS {".", "|", ":", "\"", "'", "(", ")", "&", "[", "]"}

using namespace std;

// TODO: Split the code here among several files
// TODO: Add quiet flag
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
  --add-lyrics          Adds the songs lyrics to the downloaded mp3 file
)";

bool add_lyrics_tag(string&);

void download_song(const string& apikey, const string& song, const string& saveFolder, 
                    bool verbose, bool add_lyrics, map<string, set<string>>& stats) {
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

            if (add_lyrics) {
                if (!add_lyrics_tag(songData)) {
                    cout<<"Failed to add lyrics to the mp3 file"<<endl;
                } else if (verbose) {
                    cout<<"Successfully added lyrics to the mp3 file"<<endl;
                }
            }
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
                    bool verbose, bool add_lyrics, map<string, set<string>>& stats) {
    cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<endl
             <<endl;

    ifstream songFile(songList.c_str());

    string song;
    while (getline(songFile, song)) {
        if (!starts_with(song, "added on:")) {
            // just to make sure statistics print in alphabetical order
            transform(song.begin(), song.end(), song.begin(), ::tolower);

            download_song(apikey, song, saveFolder, verbose, add_lyrics, stats);
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

void find_lyrics(const string& song, const string& saveFile, bool print, bool verbose) {
    static const int NUM_RESULTS = 25;
    static const float DECAY = 0.95;

    cout<<"Searching for the lyrics of \""<<song<<"\""<<endl
        <<endl;

    string search_results = search_duckduckgo(song + " lyrics");
    auto links = match_regex(search_results, R"([[:alpha:]]+\.(com|net)[_[:alnum:]/\.-]+)", NUM_RESULTS);

    bool found = false;

    string lyrics, best_lyrics;
    string site, best_site;
    string best_url;
    float best_score = 0;
    float scale = 1;

    // The more I work on the project, the more ridiculous the code becomes
    for (auto it = links.begin(); it != links.end(); ++it) {
        const auto url = *it;

        if (ends_with(url, ".")) {
            continue;
        } else if (starts_with(url, "metrolyrics")) {
            tie(found, lyrics, site) = get_lyrics(url, "MetroLyrics", R"(<div id="lyrics-body-text")",
                                                  "</div>", "\n");
        } else if (starts_with(url, "genius")) {
            tie(found, lyrics, site) = get_lyrics(url, "Genius", R"(<div class="song_body-lyrics")",
                                                  "</div>");
            if (ends_with(lyrics, "More on Genius")) {
                lyrics = trim(lyrics.substr(0, lyrics.size()-14));
            }
        } else if (starts_with(url, "lyricsbox")) {
            tie(found, lyrics, site) = get_lyrics(url, "LyricsBox", "<DIV id=lyrics", "</DIV>", 
                                                  "", true);
        } else if (starts_with(url, "songlyrics")) {
            tie(found, lyrics, site) = get_lyrics(url, "SongLyrics", R"(<p id="songLyricsDiv")",
                                                  "</p>");
        } else if (starts_with(url, "langmanual")) {
            tie(found, lyrics, site) = get_lyrics(url, "LangManual", R"(<div class="livedescription")",
                                                  "</div>", "\n");
        } else if (starts_with(url, "islandlyrics")) {
            // not sure how well this will work in general
            tie(found, lyrics, site) = get_lyrics(url, "IslandLyrics", "<h2>By <a", "<script");
        } else if (starts_with(url, "socalyrics")) {
            // ditto
            tie(found, lyrics, site) = get_lyrics(url, "SocaLyrics", R"(<div class="entry-content")", "</div>");
            found = found && !starts_with(lyrics, "Bio");
        } else {
            continue;
        }

        if (found) {
            // Should I use num_words_in_common instead of title_distance?
            float score = lyrics.size() * scale / 
                            title_distance(song, replace_all(url, {{"+","-","_","/","."}}, {" "}));
            if (verbose) cout<<TAB<<"score: "<<score<<endl;
            if (score > best_score) {
                best_score = score;
                best_lyrics = lyrics;
                best_site = site;
                best_url = url;
            }
            scale *= DECAY;
        }
    }

    // Best to change this to still make use of found in case it later becomes possible
    // to return nonempty lyrics when they weren't found (i.e. if you instead found a
    // message saying that the lyrics weren't available yet)
    if (best_lyrics.size() == 0) {
        cout<<"Unable to find lyrics"<<endl;
    } else {
        if (print) {
            cout<<endl
                <<"Showing Lyrics from "<<best_site<<" ("<<best_url<<")"<<endl
                <<endl
                <<best_lyrics<<endl
                <<endl;
        }
        if (saveFile != "") {
            save_lyrics(saveFile, best_lyrics);
        }
    }
    return best_lyrics;
} 

bool read_tag(const string& data, string& title, string& artist) {
    static const int HEADER_SIZE = 10;

    bool extendedHeader = data[5] & 0x40;
    // If this isn't a clear line, I don't know what is
    int size = from_synchsafe(rev_bytes(*(int*)&data[6]));
    int offset = extendedHeader ? from_synchsafe(rev_bytes(*(int*)&data[HEADER_SIZE])) : 0;

    for (int pos = HEADER_SIZE+offset; pos < HEADER_SIZE+size;) {
        string frameID = data.substr(pos, 4);
        int frame_size = from_synchsafe(rev_bytes(*(int*)&data[pos+4]));

        // offset by 1 to ignore encoding byte
        if (frameID == "TIT2") title = trim(data.substr(pos+HEADER_SIZE+1, frame_size-1));
        else if (frameID == "TPE1") artist = trim(data.substr(pos+HEADER_SIZE+1, frame_size-1));

        pos += frame_size + HEADER_SIZE;
    }

    return title != "" /* && artist != "" */; // Just title might be enough
}

void play_song(const string& file, bool show_lyrics, bool show_output, bool verbose) {
    cout<<"Playing "<<file<<endl;

    if (!ends_with(file, ".mp3")) {
        cout<<"Must supply an MP3 file"<<endl;
        return;
    } else if (show_lyrics) {
        string data;
        if (!read_file(file, data)) {
            // If we can't read file, play probably can't either, so exit
            cout<<"Could not read file"<<endl;
            return;
        }

        // If something goes wrong with lyrics, can still play song
        string title, artist;
        if (!starts_with(data, "ID3")) {
            cout<<"MP3 file must use ID3v2 tag if you want lyrics"<<endl;
        } else if (!read_tag(data, title, artist)) {
            cout<<"Could not extract title and artist information from MP3"<<endl;
        } else {
            cout<<"The song is \""<<title<<"\" by \""<<(artist == "" ? "unkown" : artist)<<"\""<<endl;
            find_lyrics(trim(artist + " " + title), "", true, verbose);
        }
    }

    string cmd = "play " + shellify(file) + (show_output ? "" : " -q");
    cout<<"Running command \""<<cmd<<"\""<<endl;
    // TODO: make this line OS agnostic
    system(cmd.c_str());
}

bool add_lyrics_tag(string& data) {
    static const int HEADER_SIZE = 10;

    // Note: Could only pass over header file once by adding lyrics to end instead of beginning
    string title, artist;
    if (!read_tag(data, title, artist)) {
        cout<<"Could not extract title and artist information from MP3"<<endl;
        return false;
    }
    string lyrics = get_lyrics(trim(artist + " " + title), "", false);
    if (lyrics == "") {
        cout<<"Could not find lyrics for the song"<<endl;
        return false;
    }

    lyrics = replace_all(lyrics, "\n", "[CR][LF]\n");
    lyrics += ends_with(lyrics, "[CR][LF]") ? "" : "[CR][LF]";
    string lyrics_len = to_string(lyrics.size());
    string lyrics_tag = 
        string("LYRICSBEGIN\n") +
        "IND000003\n" +
        "100\n" +
        "LYR" + string(5-lyrics_len.size(), '0') + lyrics_len + "\n" +
        lyrics + "\n";
    string lyrics_tag_len = to_string(lyrics_tag.size());
    lyrics_tag += string(6-lyrics_tag_len.size(), '0') + lyrics_tag_len + "LYRICS200";

    bool extendedHeader = data[5] & 0x40;
    int size = from_synchsafe(rev_bytes(*(int*)&data[6]));
    int offset = extendedHeader ? from_synchsafe(rev_bytes(*(int*)&data[HEADER_SIZE])) : 0;

    data += lyrics_tag;

    return true;
}

// TODO: Restructure this in a slightly cleaner way
vector<char*> splitPlayArgs(int argc, char** argv) {
    vector<char*> args;
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "--play") == 0) {
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
         playout = args["--show-play-output"].asBool(),
         add_lyrics = args["--add-lyrics"].asBool();

    vector<string> songs = args["--play"].asStringList();

    map<string, set<string>> stats;
    saveFolder += ends_with(saveFolder, "/") ? "" : "/";
    songFolder += ends_with(songFolder, "/") ? "" : "/"; 

    if (song != "") {
        find_lyrics(song, saveFile, print, verbose);
    } else if ((song = args["--download"].asString()) != "") {
        cout<<"Downloading \""<<song<<"\" and saving song in \""<<saveFolder<<"\""<<endl
             <<endl;
        download_song(apikey, song, saveFolder, verbose, add_lyrics, stats);
    } else if (!songs.empty()) {
        static const string stars(100, '*');

        for (int i = 0; i < songs.size(); i++) {
            cout<<stars<<endl;
            play_song(songFolder + songs[i], lyrics, playout, verbose);
        }
    } else {
        download_songs(apikey, songList, saveFolder, verbose, add_lyrics, stats);
        print_statistics(stats);
    }
    return 0;
}
