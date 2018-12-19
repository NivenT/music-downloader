#pragma once
#include <string>
#include <map>
#include <set>

extern void download_song(const std::string&, const std::string&, const std::string&,
                          bool, std::map<std::string, std::set<std::string>>&);
extern void download_songs(const std::string&, const std::string&, const std::string&,
                           bool, std::map<std::string, std::set<std::string>>&);
extern void print_statistics(std::map<std::string, std::set<std::string>>);
