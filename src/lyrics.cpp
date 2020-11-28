#include <iostream>

#include "util.h"
#include "web.h"
#include "lyrics.h"

using namespace std;

static bool verbose_should_be_a_global_but_im_too_lazy_to_make_that_change_so_heres_a_hack;

tuple<bool, string, string> get_lyrics(const string& url, const string& domain, const string& beg_tag,
                                       const string& end_tag, const string& rpl, bool ugly_while_loop) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, domain, verbose_should_be_a_global_but_im_too_lazy_to_make_that_change_so_heres_a_hack)) {
        int start = response.text.find(beg_tag);
        if (start != string::npos) {
            cout<<"Retrieving lyrics from "<<domain<<" ("<<url<<")"<<endl;

            int end = response.text.find(end_tag, start);
            while (ugly_while_loop && response.text[end-1] == '>') {
                end = response.text.find(end_tag, end+1);
            }
            string lyrics = remove_html_tags(response.text.substr(start, end-start), rpl);
            return make_tuple(true, remove_html_entites(lyrics, ""), domain);
        }
    }
    return make_tuple(false, "", "");
}

void find_lyrics(const string& song, const string& saveFile, bool print, bool verbose) {
    static const int NUM_RESULTS = 25;
    static const float DECAY = 0.95;

    verbose_should_be_a_global_but_im_too_lazy_to_make_that_change_so_heres_a_hack = verbose;

    cout<<"Searching for the lyrics of \""<<song<<"\""<<endl
        <<endl;

    string search_results = search_duckduckgo(song + " lyrics", verbose);
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
        } else if (starts_with(url, "naijaproper")) {
            // ditto x2
            tie(found, lyrics, site) = get_lyrics(url, "NaijaProper", "<b>Lyrics: ", "Download Lyrics");
        } else if (starts_with(url, "kamerlyrics")) {
            tie(found, lyrics, site) = get_lyrics(url, "KamerLyrics", "TOUTES SES PAROLES", "Paroles");
            lyrics = replace_all(lyrics, "(adsbygoogle = window.adsbygoogle || []).push({});", "");
        } else if (starts_with(url, "musixmatch")) {
            tie(found, lyrics, site) = get_lyrics(url, "MusixMatch", R"(<p class="mxm-lyrics__content)",
                                                  "</p>");
        } else if (starts_with(url, "greatsong")) {
            tie(found, lyrics, site) = get_lyrics(url, "GreatSong", R"(<div class="share-lyrics")",
                                                  "</div>", "\n");
            found = false;
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
}
