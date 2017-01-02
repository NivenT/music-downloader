#include <cpr/cpr.h>
#include <json.hpp>

extern std::string youtube_to_download(const std::string& id);
extern bool check_successful_response(const cpr::Response& response, const std::string& server);
extern std::tuple<std::string, std::string> search_youtube_for_song(const std::string& song, const std::string& apikey);
