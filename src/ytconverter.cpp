#include "ytconverter.h"
#include "util.h"
#include "web.h"

#define APIKEY "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc"

using json = nlohmann::json;
using namespace std;

string ConvertMP3::get_link(const string& id) {
    json request;
    request["video"] = "http://www.youtube.com/watch?v=" + id;
    request["autostart"] = "1";

    string query = construct_query(request, {"video", "autostart"});
    string url = "http://www.convertmp3.io/download/?" + query;

    auto response = cpr::Get(cpr::Url{url});
    if (check_successful_response(response, "YouTubeInMP3")) {
        auto links = match_regex(response.text, "href=\"([^\"]*)\"", -1, 1);
        for (const auto& link : links) {
            if (starts_with(link, "/download")) {
                return "http://www.convertmp3.io" + link;
            }
        }
    }
    return "";
}

tuple<bool, string> ConvertMP3::download_song(const string& url) {
    static const int MAX_NUM_ATTEMPTS = 100;

    static const auto doAgain = [](const string& mp3) {
        // Magic number is hopefully not so magic
        return starts_with(trim(mp3), "<html>") || mp3.size() < 100*1024;
    };

    auto response = cpr::Get(cpr::Url{url}); bool fail;
    for (int i = 0; (fail = doAgain(response.text)) && check_successful_response(response, "YouTubeInMP3") && i < MAX_NUM_ATTEMPTS; ++i) {
        response = cpr::Get(cpr::Url{url});
    }
    // check_successful_response here may not be needed
    return make_tuple(!fail && check_successful_response(response, "YouTubeInMP3"), response.text);
}