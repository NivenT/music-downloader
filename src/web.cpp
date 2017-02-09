#include <stack>

#include <cpr/util.h>

#include "util.h"
#include "web.h"

using json = nlohmann::json;

// Is server even the right name for that argument?
bool check_successful_response(const cpr::Response& response, const std::string& server) {
	if (!response.status_code) {
		std::cout<<"Error occured ("<<(int)response.error.code<<"):"<<std::endl
				 <<response.error.message<<std::endl
				 <<std::endl
				 <<"Exiting program"<<std::endl
				 <<std::endl;
		exit(0xBAD);
	} else if (response.status_code/100 == 4) {
		std::cout<<server<<" response ("<<response.status_code<<"):"<<std::endl
				 <<response.text<<std::endl
				 <<std::endl
				 <<"Exiting program"<<std::endl
				 <<std::endl;
		exit(0xBAD);
	} else if (response.status_code/100 != 2) {
		std::cout<<server<<" response ("<<response.status_code<<"):"<<std::endl
				 <<response.text<<std::endl
				 <<std::endl;
		return false;
	}
	return true;
}

std::string construct_query(const json& request, const std::vector<std::string>& keys) {
	std::string query = "";
	for (const auto key : keys) {
		std::string value = request[key];
		query += key + "=" + value + "&";
	}
	return query;
}

std::tuple<bool, std::string> download_song(const std::string& url) {
	static const int MAX_NUM_ATTEMPTS = 100;

	static const auto doAgain = [](const std::string& mp3) {
		// Magic number is hopefully not so magic
		return starts_with(trim(mp3), "<html>") || mp3.size() < 100*1024;
	};

	auto response = cpr::Get(cpr::Url{url}); bool fail;
	for (int i = 0; fail = doAgain(response.text) && check_successful_response(response, "YouTubeInMP3") && i < MAX_NUM_ATTEMPTS; ++i) {
		response = cpr::Get(cpr::Url{url});
	}
	// check_successful_response here may not be needed
	return std::make_tuple(!fail && check_successful_response(response, "YouTubeInMP3"), response.text);
}

std::string search_duckduckgo(const std::string& query) {
	std::string url = "https://duckduckgo.com/html/?q=" + urlify(query);

	std::cout<<"DuckDuckGo URL: "<<url<<std::endl;
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	return check_successful_response(response, "DuckDuckGo") ? response.text : "";
}

std::string remove_html_tags(const std::string& html, const std::string& rpl) {
	std::string tagless = html;

	int pos = 0;
	// Terrible way to do this, I think
	while ((pos = tagless.find('<', pos)) != std::string::npos) {
		int end = tagless.find('>', pos);

		std::string tag = tagless.substr(pos, end-pos+1);
		tagless.replace(pos, end-pos+1, tag == "<br>" ? "" : rpl);
	}
	return trim(tagless);
}