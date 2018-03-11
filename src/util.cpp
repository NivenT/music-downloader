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

using namespace std;

bool song_exists(const string& title) {
    string path = fileify(title);
    ifstream file(path.c_str());
    return file.good();
}

bool starts_with(const string& str, const string& prefix) {
    return str.find(prefix) == 0;
}

bool ends_with(const string& str, const string& suffix) {
    return str.rfind(suffix) == str.size() - suffix.size();
}

string song_probably_exists(const string& title, const string& folder) {
    // title_distance is not realiable enough for this function to work well
    return "";

    // Possible too generous?
    static const float MATCH_THRESHOLD = 0.35;

    string title_without_folder = title.substr(folder.size());

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

string replace_all(const string& str, const string o, const string n) {
    string ret = str;

    int pos = 0;
    while ((pos = ret.find(o, pos)) != string::npos) {
        ret.replace(pos, o.size(), n);
        pos += n.size();
    }
    return ret;
}


string replace_all(const string& str, const vector<vector<string>> os, const vector<string> ns) {
    assert(os.size() == ns.size());

    string ret = str;
    // Could be done in one pass over the string but this is easier
    for (int i = 0; i < os.size(); ++i) {
        for (int j = 0; j < os[i].size(); ++j) {
            ret = replace_all(ret, os[i][j], ns[i]);
        }
    }
    return ret;
}

string trim(const string& str) {
    static const string whitespace = " \t\n\0";

    auto begin = str.find_first_not_of(whitespace);
    auto end = str.find_last_not_of(whitespace);

    return str.substr(begin, end-begin+1);
}

string to_hex(unsigned char c) {
    static const auto lambda = [](int digit) {
        // This feels like overkill
        return string(1, digit < 10 ? '0' + digit : 'A' + digit - 10);
    };
    return lambda(c/16) + lambda(c%16);
}

string fileify(const string& title) {
    string ret = title;
    transform(title.begin(), title.end(), ret.begin(), [](char c) {
        return c == ' ' ? '_' : c;
    });
    return ends_with(ret, ".mp3") ? ret : ret + ".mp3";
}

string urlify(const string& query) {
    // Maybe I should just convert anything that isn't alphanumeric just to be safe
    static const unordered_set<char> SPECIAL_CHARS{'&', '/', ',', '$', '!', '?', ':', '=', '[', ']', '+', '(', ')', '\\', '{', '}', '\'', '#'};

    string ret = query;
    for (int i = 0; i < ret.size(); ++i) {
        if (ret[i] == ' ') {
            ret[i] = '+';
        } else if (SPECIAL_CHARS.find(ret[i]) != SPECIAL_CHARS.end()) {
            string hex = to_hex(ret[i]);
            ret.replace(i, 1, "%" + hex);
            i += hex.size();
        }
    }
    return ret;
}

string shellify(const string& cmd) {
    static const unordered_set<char> SPECIAL_CHARS{'&', '(', ')', '\"', '\''};

    string ret = cmd;
    for (int i = 0; i < ret.size(); ++i) {
        if (SPECIAL_CHARS.find(ret[i]) != SPECIAL_CHARS.end()) {
            ret.replace(i, 0, "\\");
            i++;
        }
    }
    return ret;
}

int levenshtein(const string& s1, const string& s2) {
    string str1(s1.size(), ' '), str2(s2.size(), ' ');

    transform(s1.begin(), s2.end(), str1.begin(), ::tolower);
    transform(s1.begin(), s2.end(), str1.begin(), ::tolower);

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

            cost_matrix[i][j] = min(subs_cost, min(insert_cost, delete_cost));
        }
    }
    return cost_matrix[str1.size()][str2.size()];
}

// Slightly misleading name
float num_words_in_common(const string& str1, const string& str2) {
    unordered_set<string> bag1, bag2;
    vector<string> words1, words2;
    string s1(str1.size(), ' '), s2(str2.size(), ' ');

    transform(str1.begin(), str1.end(), s1.begin(), ::tolower);
    transform(str2.begin(), str2.end(), s2.begin(), ::tolower);

    words1 = cpr::util::split(s1, ' ');
    words2 = cpr::util::split(s2, ' ');

    bag1.insert(words1.begin(), words1.end());
    bag2.insert(words2.begin(), words2.end());

    float count = 0;
    for (const auto& word : bag1) {
        float max_score = 0;
        for (const auto& other : bag2) {
            string sub = word.size() > other.size() ? other : word;
            string sup = word.size() > other.size() ? word : other;
            if (sup.find(sub) != string::npos) {
                float score = sub.size()/float(sup.size());
                max_score = max(score, max_score);
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
float title_distance(const string& str1, const string& str2) {
    return (levenshtein(str1, str2)-2*num_words_in_common(str1, str2))/max(str1.size(), str2.size());
}

void write_to_mp3(const string& title, const string& data, bool verbose) {
    string path = fileify(title);

    if (verbose) {
        cout<<TAB<<"Saving song to "<<path<<endl;
    }
    ofstream file(path.c_str());

    file.write(data.c_str(), data.size());
    file.close();
}

void save_lyrics(const string& path, const string& data) {
    cout<<"Writing lyrics to "<<path<<endl;
    ofstream file(path.c_str());

    file.write(data.c_str(), data.size());
    file.close();
}

bool read_file(const string& path, string& data) {
    ifstream file(path.c_str());
    if (!file) return false;

    file.seekg(0, file.end);
    data.resize(file.tellg());
    file.seekg(0, file.beg);

    file.read(&data[0], data.capacity());
    return true;
}

unordered_set<string> match_regex(const string& text, const string& regex, int maxMatches, int index) {
    string search = text;
    std::regex re_links(regex);
    smatch match;

    unordered_set<string> matches;
    while ((matches.size() < maxMatches || maxMatches == -1) && regex_search(search, match, re_links)) {
        matches.insert(match[index]);
        search = match.suffix().str();
    }
    return matches;
}

unsigned int rev_bytes(unsigned int num) {
    unsigned int bytes[] = {num & 0xFF, (num & 0xFF00) >> 8, 
                            (num & 0xFF0000) >> 16, (num & 0xFF000000) >> 24};
    return bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
}

unsigned int from_synchsafe(unsigned int num) {
    unsigned int bytes[] = {num & 0xFF, (num & 0xFF00), 
                            (num & 0xFF0000), (num & 0xFF000000)};
    return bytes[0] | (bytes[1] >> 1) | (bytes[2] >> 2) | (bytes[3] >> 3);
}