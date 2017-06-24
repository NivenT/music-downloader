#include "util.h"
#include "web.h"
#include "lyrics.h"

using namespace std;

tuple<bool, string> get_metrolyrics(const string& url) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "MetroLyrics")) {
        int start = response.text.find(R"(<div id="lyrics-body-text")");
        if (start != string::npos) {
            cout<<"Retrieving lyrics from MetroLyrics ("<<url<<")"<<endl
                <<endl;

            int end = response.text.find("</div>", start);
            string lyrics = remove_html_tags(response.text.substr(start, end-start), "\n");
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}

tuple<bool, string> get_genius(const string& url) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "Genius")) {
        int start = response.text.find(R"(<div class="song_body-lyrics")");
        if (start != string::npos) {
            cout<<"Retrieving lyrics from Genius ("<<url<<")"<<endl
                <<endl;

            int end = response.text.find("</div>", start);
            string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
            
            if (ends_with(lyrics, "More on Genius")) {
                lyrics = trim(lyrics.substr(0, lyrics.size()-14));
            }
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}

tuple<bool, string> get_lyricsbox(const string& url) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "LyricsBox")) {
        int start = response.text.find(R"(<DIV id=lyrics)");
        if (start != string::npos) {
            cout<<"Retrieving lyrics from LyricsBox ("<<url<<")"<<endl
                <<endl;

            int end = response.text.find("</DIV>", start);
            // This is so bad (I really need to write a proper function for finding matching html tags)
            while (response.text[end-1] == '>') {
                end = response.text.find("</DIV>", end+1);
            }
            string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}

tuple<bool, string> get_songlyrics(const string& url) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "SongLyrics")) {
        int start = response.text.find(R"(<p id="songLyricsDiv")");
        if (start != string::npos) {
            cout<<"Retrieving lyrics from SongLyrics ("<<url<<")"<<endl
                <<endl;

            int end = response.text.find("</p>", start);
            string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}

tuple<bool, string> get_langmanual(const string& url) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "LangManual")) {
        int start = response.text.find(R"(<div class="livedescription")");
        if (start != string::npos) {
            cout<<"Retrieving lyrics from LangManual ("<<url<<")"<<endl
                <<endl;

            int end = response.text.find("</div>", start);
            string lyrics = remove_html_tags(response.text.substr(start, end-start), "\n");
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}