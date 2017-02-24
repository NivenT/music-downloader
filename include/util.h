#define TAB "  "

#include <string>
#include <vector>
#include <unordered_set>

extern bool song_exists(const std::string& title);
extern bool starts_with(const std::string& str, const std::string& prefix);
extern bool ends_with(const std::string& str, const std::string& suffix);
extern std::string replace_all(const std::string& str, const std::string o, const std::string n);
extern std::string replace_all(const std::string& str, const std::vector<std::vector<std::string>> os, const std::vector<std::string> ns);
extern std::string trim(const std::string& str);
extern std::string to_hex(unsigned char c);
extern std::string fileify(const std::string& title);
extern std::string urlify(const std::string& query);
extern int levenshtein(const std::string& str1, const std::string& str2);
extern std::vector<std::string> get_words(const std::string& str);
extern float num_words_in_common(const std::string& str1, const std::string& str2);
extern float title_distance(const std::string& str1, const std::string& str2);
extern void write_to_mp3(const std::string& title, const std::string& data, bool verbose);
extern void save_lyrics(const std::string& path, const std::string& data);
extern std::unordered_set<std::string> match_regex(const std::string& text, const std::string& regex, int maxMatches, int index = 0);