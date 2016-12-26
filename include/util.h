#define TAB "  "

#include <string>
#include <vector>

extern bool starts_with(const std::string& str, const std::string& prefix);
extern bool ends_with(const std::string& str, const std::string& suffix);
extern std::string to_hex(unsigned char c);
extern std::string fileify(const std::string& title);
extern std::string urlify(const std::string& query);
extern std::string to_http(const std::string& url);
extern int levenshtein(const std::string& str1, const std::string& str2);
extern std::vector<std::string> get_words(const std::string& str);
extern float num_words_in_common(const std::string& str1, const std::string& str2);
extern float title_distance(const std::string& str1, const std::string& str2);