#include <cpr/cpr.h>
#include <json.hpp>

extern std::string youtube_to_download(const std::string& id);
extern std::tuple<std::string, std::string> search_youtube_for_song(const std::string& song, bool verbose);
