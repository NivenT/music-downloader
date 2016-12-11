#include <cpr/cpr.h>
#include <cpr/util.h>
#include <json.hpp>

extern void write_to_mp3(const std::string& title, const std::string& data);
extern std::string youtube_to_download(const std::string& id);
extern void download_song(const std::string& url, const std::string& saveFolder = "");
extern std::string construct_query(const nlohmann::json& request, const std::vector<std::string>& keys);
extern bool check_successful_response(const cpr::Response& response, const std::string& server);
extern std::vector<std::string> search_youtube_for_song(const std::string& song, const std::string& apikey);
