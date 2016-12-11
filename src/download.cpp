#include <fstream>

#include "util.h"
#include "download.h"

using json = nlohmann::json;

void write_to_mp3(const std::string& title, const std::string& data) {
	std::string path = fileify(title);

	std::cout<<TAB<<"Saving song to "<<path<<std::endl;
	std::ofstream file(path.c_str());

	file.write(data.c_str(), data.size());
	file.close();
}

std::string youtube_to_download(const std::string& id) {
	const static std::string BASE = "http://api.convert2mp3.cc/check.php?api=true&v=";

	int rand = std::rand()%3500000;
	return BASE + id + "&h=" + std::to_string(rand);
}

void download_song(const std::string& url, const std::string& saveFolder) {
	// It sometimes takes multiple requests for the song to finish downloading
	static const int MAX_NUM_REQUESTS = 100;
	for (int i = 0; i < MAX_NUM_REQUESTS; ++i) {
		auto response = cpr::Get(cpr::Url{url});
		std::cout<<TAB<<TAB<<"resp "<<i<<": "<<response.text<<std::endl;

		auto tokens = cpr::util::split(response.text, '|');
		if (tokens.size() >= 4 && tokens[0] == "OK") {
			const std::string songUrl = "http://dl" + tokens[1] + ".downloader.space/dl.php?id=" + tokens[2];
			response = cpr::Get(cpr::Url{songUrl});

			write_to_mp3(saveFolder + (ends_with(saveFolder, "/") ? "" : "/") + tokens[3], response.text);
			return;
		}
	}
}

std::string construct_query(const json& request, const std::vector<std::string>& keys) {
	std::string query = "";
	for (const auto key : keys) {
		std::string value = request[key];
		query += key + "=" + value + "&";
	}
	return query;
}

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

// YouTube API https://developers.google.com/youtube/v3/docs/search/list
std::vector<std::string> search_youtube_for_song(const std::string& song, const std::string& apikey) {
	// Top result isn't guaranteed to be the correct result
	static const int MAX_NUM_DOWNLOADS_PER_SONG = 2;

	json request;
	request["part"] = "snippet";
	request["topicId"] = "/m/04rlf"; // music
	request["maxResults"] = std::to_string(MAX_NUM_DOWNLOADS_PER_SONG);
	request["type"] = "video";
	request["q"] = urlify(song);
	request["key"] = apikey;

	std::cout<<"Searching YouTube for song: \""<<song<<"\""<<std::endl;
	std::cout<<"Song title in url: "<<request["q"]<<std::endl;

	std::string query = construct_query(request, {"part", "topicId", "maxResults", "type", "q", "key"});
	auto response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/search?" + query});

	if (check_successful_response(response, "YouTube")) {
		json resp = json::parse(response.text);
		std::vector<std::string> results;

		int num_downloads = std::min<int>(resp["items"].size(), MAX_NUM_DOWNLOADS_PER_SONG);
		std::cout<<"Retreiving Ids for top "<<num_downloads<<" results..."<<std::endl;
		for (int i = 0; i < num_downloads; ++i) {
			results.push_back(resp["items"][i]["id"]["videoId"]);
			std::cout<<TAB<<TAB<<resp["items"][i]["id"]["videoId"]<<" -> "<<resp["items"][i]["snippet"]["title"]<<std::endl;
		}
		return results;
	}
}
