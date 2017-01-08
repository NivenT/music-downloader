#include "web.h"

#include "lyrics.h"

std::string get_metrolyrics(const std::string& url) {
	std::cout<<"Retrieving lyrics from MetroLyrics"<<std::endl
			 <<std::endl;

	auto response = cpr::Get(cpr::Url{url});
	if (check_successful_response(response, "MetroLyrics")) {
		// Terrible way to find the lyrics
		int start = response.text.find(R"(<div id="lyrics-body-text")");
		int end = response.text.find("</div>", start);
		return remove_html_tags(response.text.substr(start, end-start));
	}
}