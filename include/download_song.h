#pragma once

#include <string>
#include <map>
#include <set>

#define NOT_FOUND_MSG "not found"
#define ALREADY_EXISTED_MSG "already existed"
#define MAYBE_ALREADY_EXISTED_MSG "probably already existed, and so were not downloaded"
#define DOWNLOAD_MISTAKE_MSG "downloaded, but were likely not the songs you wanted"
#define DOWNLOAD_SUCC_MSG "successfully downloaded"
#define DOWNLOAD_FAIL_MSG "could not be downloaded"

extern void download_song(const std::string&, const std::string&, bool,
                          std::map<std::string, std::set<std::string>>&,
                          const std::string&);
extern void download_song_given_id(const std::string&, const std::string&, 
                                   const std::string&, const std::string&, bool,
                                   std::map<std::string, std::set<std::string>>&);
extern void print_statistics(std::map<std::string, std::set<std::string>>);
