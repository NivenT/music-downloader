#include "util.h"
#include "web.h"
#include "lyrics.h"

std::tuple<bool, std::string> get_metrolyrics(const std::string& url) {
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	if (check_successful_response(response, "MetroLyrics")) {
		int start = response.text.find(R"(<div id="lyrics-body-text")");
		if (start == std::string::npos) {
			return std::make_tuple(false, "");
		} else {
			std::cout<<"Retrieving lyrics from MetroLyrics ("<<url<<")"<<std::endl
			         <<std::endl;

			int end = response.text.find("</div>", start);
			std::string lyrics = remove_html_tags(response.text.substr(start, end-start), "\n");
			return std::make_tuple(true, lyrics);
		}
	}
}

std::tuple<bool, std::string> get_genius(const std::string& url) {
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	if (check_successful_response(response, "Genius")) {
		int start = response.text.find(R"(<div class="song_body-lyrics")");
		if (start == std::string::npos) {
			return std::make_tuple(false, "");
		} else {
			std::cout<<"Retrieving lyrics from Genius ("<<url<<")"<<std::endl
			         <<std::endl;

			int end = response.text.find("</div>", start);
			std::string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
			
			if (ends_with(lyrics, "More on Genius")) {
				lyrics = trim(lyrics.substr(0, lyrics.size()-14));
			}
			return std::make_tuple(true, lyrics);
		}
	}
}

std::tuple<bool, std::string> get_lyricsbox(const std::string& url) {
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	if (check_successful_response(response, "LyricsBox")) {
		int start = response.text.find(R"(<DIV id=lyrics)");
		if (start == std::string::npos) {
			return std::make_tuple(false, "");
		} else {
			std::cout<<"Retrieving lyrics from LyricsBox ("<<url<<")"<<std::endl
			         <<std::endl;

			int end = response.text.find("</DIV>", start);
			// This is so bad (I really need to write a proper function for finding matching html tags)
			while (response.text[end-1] == '>') {
				end = response.text.find("</DIV>", end+1);
			}
			std::string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
			return std::make_tuple(true, lyrics);
		}
	}
}

std::tuple<bool, std::string> get_songlyrics(const std::string& url) {
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	if (check_successful_response(response, "SongLyrics")) {
		int start = response.text.find(R"(<p id="songLyricsDiv")");
		if (start == std::string::npos) {
			return std::make_tuple(false, "");
		} else {
			std::cout<<"Retrieving lyrics from SongLyrics ("<<url<<")"<<std::endl
			         <<std::endl;

			int end = response.text.find("</p>", start);
			std::string lyrics = remove_html_tags(response.text.substr(start, end-start), "");
			return std::make_tuple(true, lyrics);
		}
	}
}