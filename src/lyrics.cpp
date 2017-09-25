#include "util.h"
#include "web.h"
#include "lyrics.h"

using namespace std;

tuple<bool, string> get_lyrics(const string& url, const string& domain, const string& beg_tag,
                                const string& end_tag, const string& rpl, bool ugly_while_loop) {
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    if (check_successful_response(response, domain)) {
        int start = response.text.find(beg_tag);
        if (start != string::npos) {
            cout<<"Retrieving lyrics from "<<domain<<" ("<<url<<")"<<endl;

            int end = response.text.find(end_tag, start);
            while (ugly_while_loop && response.text[end-1] == '>') {
                end = response.text.find(end_tag, end+1);
            }
            string lyrics = remove_html_tags(response.text.substr(start, end-start), rpl);
            return make_tuple(true, lyrics);
        }
    }
    return make_tuple(false, "");
}
