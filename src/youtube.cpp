#include <fstream>

#include "util.h"
#include "web.h"
#include "youtube.h"

using json = nlohmann::json;
using namespace std;

// Top result isn't guaranteed to be the correct result
#define MAX_NUM_RESULTS_PER_SONG 3

// YouTube API https://developers.google.com/youtube/v3/docs/search/list
tuple<string, string> search_youtube_with_api(const string& song, const string& song_q,
                                              bool verbose, const string& apikey) {
    json request;
    request["part"] = "snippet";
    request["topicId"] = "/m/04rlf"; // music
    request["maxResults"] = to_string(MAX_NUM_RESULTS_PER_SONG);
    request["type"] = "video";
    request["q"] = song_q;
    request["key"] = apikey;

    string query = construct_query(request, {"part", "topicId", "maxResults", "type", "q", "key"});
    string url = "https://www.googleapis.com/youtube/v3/search?" + query;
    if (verbose) {
        cout<<TAB<<"api call: "<<url<<endl;
    }
    
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "YouTube")) {
        json resp = json::parse(response.text);

        int num_downloads = min<int>(resp["items"].size(), MAX_NUM_RESULTS_PER_SONG);
        if (verbose) {
            cout<<"Retrieving Ids for top "<<num_downloads<<" results..."<<endl;
        }
        
        float lowest_dist = 1.1;
        string winner = "";
        json winner_title = "";

        for (int i = 0; i < num_downloads; ++i) {
            json videoId = resp["items"][i]["id"]["videoId"];
            json title = resp["items"][i]["snippet"]["title"];
            float dist = title_distance(song, title);

            if (verbose) {
                cout<<TAB<<videoId<<" -> "<<title<<" ("<<dist<<")"<<endl;
            }

            winner = dist < lowest_dist ? videoId.get<string>() : winner;
            winner_title = dist < lowest_dist ? title : winner_title;
            lowest_dist = min(dist, lowest_dist);
        }
        if (verbose) {
            cout<<"The following video was chosen: "<<winner_title<<endl;
        }
        return make_tuple(winner, winner_title.get<string>());
    }
    return make_tuple("", "");
}

// TODO: unify below and above functions more

#define SCRAPE_START_SENTINEL "// scraper_data_begin"
#define SCRAPE_END_SENTINEL   "// scraper_data_end"
tuple<string, string> search_youtube_without_api(const string& song, const string& song_q,
                                                 bool verbose) {
    json request;
    request["search_query"] = song_q;
    request["sp"] = "EgIQAQ%253D%253D";

    string query = construct_query(request, {"search_query", "sp"});
    string url = "https://www.youtube.com/results?" + query;
    if (verbose) {
        cout<<TAB<<"youtube search link: "<<url<<endl;
    }
    
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, "YouTube")) {
        size_t beg = response.text.find(SCRAPE_START_SENTINEL);
        size_t end = response.text.find(SCRAPE_END_SENTINEL);
        string meta_data = response.text.substr(beg + sizeof(SCRAPE_START_SENTINEL),
                                                      end - beg - sizeof(SCRAPE_START_SENTINEL));
        beg = meta_data.find_first_of('{');
        end = meta_data.find_last_of('}');
        meta_data = meta_data.substr(beg, end - beg + 1);

        json data = json::parse(meta_data);
        // yeesh
        json content = data["contents"]["twoColumnSearchResultsRenderer"]["primaryContents"]["sectionListRenderer"]["contents"][0]["itemSectionRenderer"]["contents"];

        int num_downloads = min<int>(content.size(), MAX_NUM_RESULTS_PER_SONG);
        if (verbose) {
            cout<<"Retrieving Ids for top "<<num_downloads<<" results..."<<endl;
        }

        float lowest_dist = 1.1;
        string winner = "";
        json winner_title = "";

        for (int i = 0; i < num_downloads; ++i) {
            json videoId = content[i]["videoRenderer"]["videoId"];
            json title = content[i]["videoRenderer"]["title"]["runs"][0]["text"];
            if (videoId.is_null() || title.is_null()) {
                if (num_downloads < content.size()) num_downloads++;
                continue;
            }

            float dist = title_distance(song, title);

            if (verbose) {
                cout<<TAB<<videoId<<" -> "<<title<<" ("<<dist<<")"<<endl;
            }

            winner = dist < lowest_dist ? videoId.get<string>() : winner;
            winner_title = dist < lowest_dist ? title : winner_title;
            lowest_dist = min(dist, lowest_dist);
        }
        if (verbose) {
            cout<<"The following video was chosen: "<<winner_title<<endl;
        }
        return make_tuple(winner, winner_title.get<string>());
    }
    return make_tuple("", "");
}

tuple<string, string> search_youtube_for_song(const string& song, bool verbose, 
                                              const string& apikey) {
    const string song_q = urlify(song);
    cout<<"Searching YouTube for song: \""<<song<<"\""<<endl;
    if (verbose) {
        cout<<"Song title in url: "<<song_q<<endl;
    }
    return apikey == "" ? search_youtube_without_api(song, song_q, verbose) :
                          search_youtube_with_api(song, song_q, verbose, apikey);
}

#define TITLE_UNKNOWN(id) "UKNOWN_TITLE_ID_" + id
string get_title_from_id(const string& id, bool verbose) {
    string url = "https://www.youtube.com/watch?v=" + id;
    if (verbose) {
        cout<<"Attempting to get the title of video with id "<<id<<"..."<<endl;
    }
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (!check_successful_response(response, "YouTube")) {
        if (verbose) cout<<"Could not find title"<<endl;
        return TITLE_UNKNOWN(id);
    }
    // titles.size() <= 1 because of call to match_regex(blah, blah, 1, blah)
    auto titles = match_regex(response.text, 
                              R"#(meta name="title".*content="([^"]*)")#",
                              1, 1);
    if (titles.empty()) {
        if (verbose) cout<<"Could not find title"<<endl;
        return TITLE_UNKNOWN(id);
    }
    // just to make sure statistics print in alphabetical order
    string title = *titles.begin();
    transform(title.begin(), title.end(), title.begin(), ::tolower);
    if (verbose) {
        cout<<"YT video with id "<<id<<" has title \""<<title<<"\""<<endl;
    }
    return title;
}
