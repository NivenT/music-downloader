#pragma once

#include <cpr/cpr.h>
#include <json.hpp>

extern std::tuple<std::string, std::string> search_youtube_for_song(const std::string& song, 
                                                                    bool verbose,
                                                                    const std::string& apikey);
extern std::string get_title_from_id(const std::string& id, bool verbose);
