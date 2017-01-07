#include <regex>

#include <cpr/util.h>

#include "util.h"
#include "web.h"

using json = nlohmann::json;

// Is server even the right name for that argument?
bool check_successful_response(const cpr::Response& response, const std::string& server) {
	if (!response.status_code) {
		std::cout<<"Error occured ("<<(int)response.error.code<<"):"<<std::endl
				 <<response.error.message<<std::endl
				 <<std::endl;
		exit(0xBAD);
	} else if (response.status_code/100 != 2) {
		std::cout<<server<<" response ("<<response.status_code<<"):"<<std::endl
				 <<response.text<<std::endl
				 <<std::endl;
		exit(0xBAD);
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

std::string download_song(const std::string& url, const std::string& saveFolder) {
	// It sometimes takes multiple requests for the song to finish downloading
	static const int MAX_NUM_REQUESTS = 100;
	for (int i = 0; i < MAX_NUM_REQUESTS; ++i) {
		auto response = cpr::Get(cpr::Url{url});
		//std::cout<<TAB<<TAB<<"resp "<<i<<": "<<response.text<<std::endl;

		auto tokens = cpr::util::split(response.text, '|');
		if (tokens.size() >= 4 && tokens[0] == "OK") {
			const std::string songUrl = "http://dl" + tokens[1] + ".downloader.space/dl.php?id=" + tokens[2];
			response = cpr::Get(cpr::Url{songUrl});

			//write_to_mp3(saveFolder + (ends_with(saveFolder, "/") ? "" : "/") + tokens[3], response.text);
			std::cout<<TAB<<TAB<<"Download successful"<<std::endl;
			return response.text;
		}
	}
	std::cout<<TAB<<TAB<<"Download failed"<<std::endl;
	return "";
}

std::string search_duckduckgo(const std::string& query) {
	std::string url = "https://duckduckgo.com/html/?q=" + urlify(query);

	std::cout<<"DuckDuckGo URL: "<<url<<std::endl;
	auto response = cpr::Get(cpr::Url{url});
	return check_successful_response(response, "DuckDuckGo") ? response.text : "";
}

// Idea: store results in a trie
std::set<std::string> get_links(const std::string& html, int maxMatches) {
	std::string search = html;
	std::regex re_links("href=\"(http[^\"]*)\"");
	std::smatch match;

	std::set<std::string> links;
	while (links.size() < maxMatches && std::regex_search(search, match, re_links)) {
		//std::cout<<"found: "<<match[1]<<std::endl;
		links.insert(match[1]);
		search = match.suffix().str();
	}
	return links;
}

std::string remove_html_tags(const std::string& html) {
	std::string tagless = html;

	int pos = 0;
	// Terrible way to do this, I think
	while ((pos = tagless.find('<', pos)) != std::string::npos) {
		int end = tagless.find('>', pos);

		std::string tag = tagless.substr(pos, end-pos+1);
		tagless.replace(pos, end-pos+1, tag == "<br>" ? "" : "\n");
	}
	return trim(tagless);
}