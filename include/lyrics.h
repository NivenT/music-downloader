#include <string>

extern std::tuple<bool, std::string, std::string> get_lyrics(const std::string& url, const std::string& domain,
												             const std::string& beg_tag, 
                                                             const std::string& end_tag, 
												             const std::string& rpl = "", 
                                                             bool ugly_while_loop = false);