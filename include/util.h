#define TAB "  "

#include <string>

extern bool starts_with(const std::string& str, const std::string& prefix);
extern bool ends_with(const std::string& str, const std::string& suffix);
extern std::string fileify(const std::string& title);
extern std::string urlify(const std::string& query);
extern std::string to_http(const std::string& url);