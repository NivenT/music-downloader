#include <algorithm>
#include <unordered_set>
#include <sstream>

#include <cpr/util.h>

#include "util.h"

bool starts_with(const std::string& str, const std::string& prefix) {
	return str.find(prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
	return str.find_last_of(suffix) == str.size() - 1;
}

std::string fileify(const std::string& title) {
	std::string ret = title;
	std::transform(title.begin(), title.end(), ret.begin(), [](char c) {
		return c == ' ' ? '_' : c;
	});
	return ends_with(ret, ".mp3") ? ret : ret + ".mp3";
}

std::string urlify(const std::string& query) {
	static const std::unordered_set<char> STATIC_CHARS{'-', '.', '(', ')'};

	std::string ret = query;
	for (int i = 0; i < ret.size(); ++i) {
		if (ret[i] == ' ') {
			ret[i] = '+';
		} else if (ret[i] == '\'') {
			ret.replace(i, 1, "%27");
			i += 2;
		} else if (!isalpha(ret[i]) && STATIC_CHARS.find(ret[i]) == STATIC_CHARS.end()) {
			std::string code = std::to_string((unsigned char)ret[i]);
			ret.replace(i, 1, "%" + code);
			i += code.size();
		}
	}
	return ret;
}

std::string to_http(const std::string& url) {
	return starts_with(url, "https") ? "http" + url.substr(5) :
		   starts_with(url, "http")  ? url : "http" + url;
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