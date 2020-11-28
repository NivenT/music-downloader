#include <stack>

#include <cpr/util.h>

#include "util.h"
#include "web.h"

using json = nlohmann::json;
using namespace std;

// Is server even the right name for that argument?
bool check_successful_response(const cpr::Response& response, const string& server,
                               bool verbose) {
    if (!response.status_code) {
        cout<<"Error occurred ("<<(int)response.error.code<<"):"<<endl;
        if (verbose) cout<<response.error.message<<endl<<endl;
        cout<<"Program used to exit in this case, but no longer does."<<endl
            <<"Unclear how properly it will handle this error..."<<endl
            <<endl;
        return false;
    } else if (response.status_code/100 == 4) {
        cout<<server<<" response ("<<response.status_code<<"):"<<endl;
        if (verbose) cout<<response.text<<endl<<endl;
        cout<<"Program used to exit in this case, but no longer does."<<endl
            <<"Unclear how properly it will handle this error..."<<endl
            <<endl;
        return false;
    } else if (response.status_code/100 != 2) {
        cout<<server<<" response ("<<response.status_code<<"):"<<endl
            <<response.text<<endl
            <<endl;
        return false;
    }
    return true;
}

string construct_query(const json& request, const vector<string>& keys) {
    string query = "";
    for (const auto key : keys) {
        string value = request[key];
        query += key + "=" + value + "&";
    }
    return query;
}

string search_duckduckgo(const string& query, bool verbose) {
    string url = "https://duckduckgo.com/html/?q=" + urlify(query);

    cout<<"DuckDuckGo URL: "<<url<<endl;
    auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
    return check_successful_response(response, "DuckDuckGo", verbose) ? response.text : "";
}

string remove_html_tags(const string& html, const string& rpl) {
    string tagless = html;

    int pos = 0;
    // Terrible way to do this, I think
    while ((pos = tagless.find('<', pos)) != string::npos) {
        int end = tagless.find('>', pos);

        string tag = tagless.substr(pos, end-pos+1);
        tagless.replace(pos, end-pos+1, tag == "<br>" ? "" : rpl);
    }
    return trim(tagless);
}

// should actually resolve these to what they're supposed to be instead of just getting rid of them
// at some point
string remove_html_entites(string html, const string& rpl) {
    int pos = 0;
    while ((pos = html.find('&', pos)) != string::npos) {
        int end = pos+1;
        while (isalnum(html[end]) || html[end] == '#') end++;
        if (html[end] == ';') html.replace(pos, end-pos+1, rpl);
    }
    return trim(html);
}