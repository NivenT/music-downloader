#include <fstream>

#include "util.h"
#include "web.h"
#include "youtube.h"

using json = nlohmann::json;

std::string youtube_to_download(const std::string& id) {
	json request;
	request["video"] = "http://www.youtube.com/watch?v=" + id;
	request["autostart"] = "1";

	std::string query = construct_query(request, {"video", "autostart"});
	std::string url = "http://www.youtubeinmp3.com/download/?" + query;

	auto response = cpr::Get(cpr::Url{url});
	if (check_successful_response(response, "YouTubeInMP3")) {
		auto links = match_regex(response.text, "href=\"([^\"]*)\"", -1, 1);
		for (const auto& link : links) {
			if (starts_with(link, "/download")) {
				return "http://www.youtubeinmp3.com" + link;
			}
		}
	}
	return "";
}

// YouTube API https://developers.google.com/youtube/v3/docs/search/list
std::tuple<std::string, std::string> search_youtube_for_song(const std::string& song, const std::string& apikey, bool verbose) {
	// Top result isn't guaranteed to be the correct result
	static const int MAX_NUM_RESULTS_PER_SONG = 3;

	json request;
	request["part"] = "snippet";
	request["topicId"] = "/m/04rlf"; // music
	request["maxResults"] = std::to_string(MAX_NUM_RESULTS_PER_SONG);
	request["type"] = "video";
	request["q"] = urlify(song);
	request["key"] = apikey;

	std::cout<<"Searching YouTube for song: \""<<song<<"\""<<std::endl;
	if (verbose) {
		std::cout<<"Song title in url: "<<request["q"]<<std::endl;
	}

	std::string query = construct_query(request, {"part", "topicId", "maxResults", "type", "q", "key"});
	std::string url = "https://www.googleapis.com/youtube/v3/search?" + query;
	
	auto response = cpr::Get(cpr::Url{url}, cpr::VerifySsl{false});
	if (check_successful_response(response, "YouTube")) {
		json resp = json::parse(response.text);

		int num_downloads = std::min<int>(resp["items"].size(), MAX_NUM_RESULTS_PER_SONG);
		if (verbose) {
			std::cout<<"Retreiving Ids for top "<<num_downloads<<" results..."<<std::endl;
		}
		
		float lowest_dist = 1.0;
		std::string winner = "";
		json winner_title = "";

		for (int i = 0; i < num_downloads; ++i) {
			json videoId = resp["items"][i]["id"]["videoId"];
			json title = resp["items"][i]["snippet"]["title"];
			float dist = title_distance(song, title);

			if (verbose) {
				std::cout<<TAB<<videoId<<" -> "<<title<<" ("<<dist<<")"<<std::endl;
			}

			winner = dist < lowest_dist ? videoId.get<std::string>() : winner;
			winner_title = dist < lowest_dist ? title : winner_title;
			lowest_dist = std::min(dist, lowest_dist);
		}
		if (verbose) {
			std::cout<<"The following video was chosen: "<<winner_title<<std::endl;
		}
		return std::make_tuple(winner, winner_title.get<std::string>());
	}
	return std::make_tuple("", "");
}
