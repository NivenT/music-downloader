#include <set>

#include <cpr/cpr.h>
#include <json.hpp>

extern bool check_successful_response(const cpr::Response& response, const std::string& server);
extern std::string construct_query(const nlohmann::json& request, const std::vector<std::string>& keys);
extern std::string download_song(const std::string& url, const std::string& saveFolder = "");
extern std::string search_duckduckgo(const std::string& query);
extern std::set<std::string> get_links(const std::string& html, int maxMatches = 7);
extern std::string remove_html_tags(const std::string& html);