#include <iostream>

#include "lyrics.h"
#include "util.h"
#include "play.h"

using namespace std;

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
        } else {
            if (!read_tag(data, title, artist)) {
                cout<<"Could not extract title and artist information from MP3"<<endl;
                title = extract_title(file);
            }

            cout<<"The song is \""<<title<<"\" by \""<<(artist == "" ? "unknown" : artist)<<"\""<<endl;
            find_lyrics(trim(artist + " " + title), "", true, verbose);
        }
    }

    string cmd = "play " + shellify(file) + (show_output ? "" : " -q");
    cout<<"Running command \""<<cmd<<"\""<<endl;
    // TODO: make this line OS agnostic
    system(cmd.c_str());
}
