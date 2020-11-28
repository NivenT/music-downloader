#include <set>

#include <cpr/cpr.h>
#include <json.hpp>

extern bool check_successful_response(const cpr::Response& response, 
                                      const std::string& server, bool verbose);
extern std::string construct_query(const nlohmann::json& request, const std::vector<std::string>& keys);
extern std::string search_duckduckgo(const std::string& query, bool verbose);
extern std::string remove_html_tags(const std::string& html, const std::string& rpl);
extern std::string remove_html_entites(std::string html, const std::string& rpl);
extern std::tuple<bool, std::string> download_song(const std::string& url);