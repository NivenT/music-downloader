#include <string>

extern std::tuple<bool, std::string> get_metrolyrics(const std::string& url);
extern std::tuple<bool, std::string> get_genius(const std::string& url);
extern std::tuple<bool, std::string> get_lyricsbox(const std::string& url);
extern std::tuple<bool, std::string> get_songlyrics(const std::string& url);