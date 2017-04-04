#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <regex>
#include <cassert>
#include <dirent.h>

#include <cpr/util.h>

#include "util.h"

bool song_exists(const std::string& title) {
	std::string path = fileify(title);
	std::ifstream file(path.c_str());
	return file.good();
}

bool starts_with(const std::string& str, const std::string& prefix) {
	return str.find(prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
	return str.rfind(suffix) == str.size() - suffix.size();
}

std::string song_probably_exists(const std::string& title, const std::string& folder) {
	// title_distance is not realiable enough for this function to work well
	return "";

	// Possible too generous?
	static const float MATCH_THRESHOLD = 0.35;

	std::string title_without_folder = title.substr(folder.size());

	DIR *dir;
	struct dirent *entry;
	if ((dir = opendir(folder.c_str())) != NULL) {
		while ((entry = readdir(dir)) != NULL) {
			if (title_distance(title_without_folder, entry->d_name) < MATCH_THRESHOLD) {
				return entry->d_name;
			}
		}
		closedir(dir);
	}
	return "";
}

std::string replace_all(const std::string& str, const std::string o, const std::string n) {
	std::string ret = str;

	int pos = 0;
	while ((pos = ret.find(o, pos)) != std::string::npos) {
		ret.replace(pos, o.size(), n);
		pos += n.size();
	}
	return ret;
}


std::string replace_all(const std::string& str, const std::vector<std::vector<std::string>> os, const std::vector<std::string> ns) {
	assert(os.size() == ns.size());

	std::string ret = str;
	// Could be done in one pass over the string but this is easier
	for (int i = 0; i < os.size(); ++i) {
		for (int j = 0; j < os[i].size(); ++j) {
			ret = replace_all(ret, os[i][j], ns[i]);
		}
	}
	return ret;
}

std::string trim(const std::string& str) {
	static const std::string whitespace = " \t\n";

	auto begin = str.find_first_not_of(whitespace);
	auto end = str.find_last_not_of(whitespace);

	return str.substr(begin, end-begin+1);
}

std::string to_hex(unsigned char c) {
	static const auto lambda = [](int digit) {
		// This feels like overkill
		return std::string(1, digit < 10 ? '0' + digit : 'A' + digit - 10);
	};
	return lambda(c/16) + lambda(c%16);
}

std::string fileify(const std::string& title) {
	std::string ret = title;
	std::transform(title.begin(), title.end(), ret.begin(), [](char c) {
		return c == ' ' ? '_' : c;
	});
	return ends_with(ret, ".mp3") ? ret : ret + ".mp3";
}

std::string urlify(const std::string& query) {
	static const std::unordered_set<char> SPECIAL_CHARS{'&', '/', ',', '$', '!', '?', ':', '=', '[', ']', '+', '(', ')', '\\', '{', '}', '\''};

	std::string ret = query;
	for (int i = 0; i < ret.size(); ++i) {
		if (ret[i] == ' ') {
			ret[i] = '+';
		} else if (SPECIAL_CHARS.find(ret[i]) != SPECIAL_CHARS.end()) {
			std::string hex = to_hex(ret[i]);
			ret.replace(i, 1, "%" + hex);
			i += hex.size();
		}
	}
	return ret;
}

int levenshtein(const std::string& str1, const std::string& str2) {
	int cost_matrix[str1.size()+1][str2.size()+1];
	for (int i = 0; i <= str1.size(); ++i) {
		cost_matrix[i][0] = i;
	}
	for (int j = 0; j <= str2.size(); ++j) {
		cost_matrix[0][j] = j;
	}

	for (int i = 1; i <= str1.size(); ++i) {
		for (int j = 1; j <= str2.size(); ++j) {
			int insert_cost = cost_matrix[i][j-1] + 1;
			int delete_cost = cost_matrix[i-1][j] + 1;
			int subs_cost = cost_matrix[i-1][j-1] + (tolower(str1[i-1]) != tolower(str2[j-1]));

			cost_matrix[i][j] = std::min(subs_cost, std::min(insert_cost, delete_cost));
		}
	}
	return cost_matrix[str1.size()][str2.size()];
}

// Slightly misleading name
float num_words_in_common(const std::string& str1, const std::string& str2) {
	std::unordered_set<std::string> bag1, bag2;
	std::vector<std::string> words1, words2;
	std::string s1(str1.size(), ' '), s2(str2.size(), ' ');

	std::transform(str1.begin(), str1.end(), s1.begin(), ::tolower);
	std::transform(str2.begin(), str2.end(), s2.begin(), ::tolower);

	words1 = cpr::util::split(s1, ' ');
	words2 = cpr::util::split(s2, ' ');

	bag1.insert(words1.begin(), words1.end());
	bag2.insert(words2.begin(), words2.end());

	float count = 0;
	for (const auto& word : bag1) {
		float max_score = 0;
		for (const auto& other : bag2) {
			std::string sub = word.size() > other.size() ? other : word;
			std::string sup = word.size() > other.size() ? word : other;
			if (sup.find(sub) != std::string::npos) {
				float score = sub.size()/float(sup.size());
				max_score = std::max(score, max_score);
			}
			if (max_score >= 0.999) {
				break;
			}
		}
		count += max_score;
	}
	return count;
}

// Idea for further improvements: Ignore words like "official" and "lyrics"
float title_distance(const std::string& str1, const std::string& str2) {
	return (levenshtein(str1, str2)-2*num_words_in_common(str1, str2))/std::max(str1.size(), str2.size());
}

void write_to_mp3(const std::string& title, const std::string& data, bool verbose) {
	std::string path = fileify(title);

	if (verbose) {
		std::cout<<TAB<<"Saving song to "<<path<<std::endl;
	}
	std::ofstream file(path.c_str());

	file.write(data.c_str(), data.size());
	file.close();
}

void save_lyrics(const std::string& path, const std::string& data) {
	std::cout<<"Writing lyrics to "<<path<<std::endl;
	std::ofstream file(path.c_str());

	file.write(data.c_str(), data.size());
	file.close();
}

std::unordered_set<std::string> match_regex(const std::string& text, const std::string& regex, int maxMatches, int index) {
	std::string search = text;
	std::regex re_links(regex);
	std::smatch match;

	std::unordered_set<std::string> matches;
	while ((matches.size() < maxMatches || maxMatches == -1) && std::regex_search(search, match, re_links)) {
		matches.insert(match[index]);
		search = match.suffix().str();
	}
	return matches;
}