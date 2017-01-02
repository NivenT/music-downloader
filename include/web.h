#include <cpr/cpr.h>
#include <json.hpp>

extern std::string download_song(const std::string& url, const std::string& saveFolder = "");
extern std::string construct_query(const nlohmann::json& request, const std::vector<std::string>& keys);
extern bool check_successful_response(const cpr::Response& response, const std::string& server);