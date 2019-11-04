#include "ytconverter.h"
#include "util.h"
#include "web.h"

#define APIKEY "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc"

using json = nlohmann::json;
using namespace std;

static bool doAgain(const string& mp3) {
    // Magic number is hopefully not so magic
    return starts_with(trim(mp3), "<html>") || mp3.size() < 100*1024;
};

cpr::Response YTConverter::try_download(const string& url) {
    return cpr::Get(cpr::Url{url});
}

tuple<bool, string> YTConverter::download_song(const string& url, bool verbose) {
    bool fail;
    auto response = try_download(url);
    for (int i = 0; (fail = doAgain(response.text)) && check_successful_response(response) && i < MAX_NUM_ATTEMPTS; ++i) {
        if (verbose && i%10 == 9) cout<<TAB<<TAB<<TAB<<"Attempt "<<(i+1)<<" / "<<MAX_NUM_ATTEMPTS<<"..."<<endl;
        response = try_download(url);
    }
    // check_successful_response here may not be needed
    return make_tuple(!fail && check_successful_response(response), response.text);
}

string ConvertMP3::get_link(const string& id) {
    json request;
    request["video"] = "http://www.youtube.com/watch?v=" + id;
    request["autostart"] = "1";

    string query = construct_query(request, {"video", "autostart"});
    string url = "http://www.convertmp3.io/download/?" + query;

    auto response = cpr::Get(cpr::Url{url});
    if (check_successful_response(response)) {
        auto links = match_regex(response.text, "href=\"([^\"]*)\"", -1, 1);
        for (const auto& link : links) {
            if (starts_with(link, "/download")) {
                return "http://www.convertmp3.io" + link;
            }
        }
    }
    return "";
}

cpr::Response PointMP3::try_download(const string& url) {
    return cpr::Get(cpr::Url{url}, cpr::Header{{"Referer", m_referrer}});
}

string PointMP3::get_link(const string& id) {
    auto response = cpr::Get(cpr::Url{"http://api.pointmp3.com/dl/v2/"},
                             cpr::Parameters{{"format", "mp3"},
                                             {"req", "http://www.youtube.com/watch?v=" + id}
                             });
    if (!check_successful_response(response)) return "";

    json resp = json::parse(response.text);
    response = cpr::Get(cpr::Url{resp["url"].get<string>()});
    if (!check_successful_response(response) || resp["error"] == true) return "";
    
    m_referrer = resp["url"];
    resp = json::parse(response.text);
    return resp["error"] == true ? "" : resp["url"];
}
