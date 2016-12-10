#include <fstream>

#include <cpr/cpr.h>
#include <cpr/util.h>
#include <json.hpp>

using json = nlohmann::json;

bool starts_with(const std::string& str, const std::string& prefix) {
	return str.find(prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
	return str.find_last_of(suffix) == str.size() - 1;
}

std::string prettify(const std::string& title) {
	std::string ret = title;
	std::transform(title.begin(), title.end(), ret.begin(), [](char c) {
		return c == ' ' ? '_' : c;
	});
	return ends_with(ret, ".mp3") ? ret : ret + ".mp3";
}

std::string urlify(const std::string& query) {
	std::string ret = query;
	for (int i = 0; i < ret.size(); ++i) {
		if (ret[i] == ' ') {
			ret[i] = '+';
		} else if (!isascii(ret[i])) {
			std::string code = std::to_string((int)ret[i]);
			ret.replace(i, 1, "%" + code);
			i += code.size();
		}
	}
	return ret;
}

void write_to_mp3(const std::string& title, const std::string& data) {
	std::string path = prettify(title);

	std::cout<<"Saving song to "<<path<<std::endl;
	std::ofstream file(path.c_str());

	file.write(data.c_str(), data.size());
	file.close();
}

std::string youtube_to_download(const std::string& id) {
	const static std::string BASE = "http://api.convert2mp3.cc/check.php?api=true&v=";

	int rand = std::rand()%3500000;
	return BASE + id + "&h=" + std::to_string(rand);
}

std::string to_http(const std::string& url) {
	return starts_with(url, "https") ? "http" + url.substr(5) :
		   starts_with(url, "http")  ? url : "http" + url;
}

void download_song(const std::string& url, const std::string& title = "") {
	// It sometimes takes multiple requests for the song to finish downloading
	static const int MAX_NUM_REQUESTS = 50;
	for (int i = 0; i < MAX_NUM_REQUESTS; ++i) {
		auto response = cpr::Get(cpr::Url{url});
		std::cout<<"    resp "<<i<<": "<<response.text<<std::endl;

		auto tokens = cpr::util::split(response.text, '|');
		if (tokens[0] == "OK" && tokens.size() == 4) {
			const std::string songUrl = "http://dl" + tokens[1] + ".downloader.space/dl.php?id=" + tokens[2];
			response = cpr::Get(cpr::Url{songUrl});

			write_to_mp3(title == "" ? tokens[3] : title, response.text);
			return;
		}
	}
}

std::string construct_query(const json& request) {
	const static std::string keys[6]{"part", "topicId", "maxResults", "type", "q", "key"};
	std::string query = "";

	for (const auto key : keys) {
		std::string value = request[key];
		query += key + "=" + value + "&";
	}
	return query;
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
	std::string query = construct_query(request);
	auto response = cpr::Get(cpr::Url{"https://www.googleapis.com/youtube/v3/search?" + query},
                             cpr::Header{{"Content-Type", "application/json"}},
                             cpr::Body{request.dump()});

	if (!response.status_code) {
		std::cout<<"Error occured ("<<(int)response.error.code<<"):"<<std::endl
				 <<response.error.message<<std::endl
				 <<std::endl;
		exit(0xBAD);
	} else if (response.status_code/100 != 2) {
		std::cout<<"YouTube response ("<<response.status_code<<"):"<<std::endl
				 <<response.text<<std::endl
				 <<std::endl;
		exit(0xBAD);
	} else {
		json resp = json::parse(response.text);
		std::vector<std::string> results;

		int num_downloads = std::min<int>(resp["items"].size(), MAX_NUM_DOWNLOADS_PER_SONG);
		std::cout<<"Retreiving Ids for top "<<num_downloads<<" results..."<<std::endl;
		for (int i = 0; i < num_downloads; ++i) {
			results.push_back(resp["items"][i]["id"]["videoId"]);
			std::cout<<"    "<<resp["items"][i]["id"]["videoId"]<<" -> "<<resp["items"][i]["snippet"]["title"]<<std::endl;
		}
		return results;
	}
}

void set_params(int argc, char** argv, std::string& apikey, std::string& song) {
	switch(argc) {
		case 1:
			std::cout<<"Must provide a valid api key as the first argument"<<std::endl;
			exit(0xBAD);
			break;

		case 2:
			apikey = argv[1];
			song = "Margo";
			break;

		default:
			apikey = argv[1];
			song = argv[2];
			break;
	}
}

int main(int argc, char** argv) {
	std::string apikey, song;
	set_params(argc, argv, apikey, song);

	std::vector<std::string> songIds = search_youtube_for_song(song, apikey);
	for (const auto& song : songIds) {
		std::cout<<std::endl;
		std::cout<<"Downloading video with Id "<<song<<"..."<<std::endl;
		const std::string downloadUrl = youtube_to_download(song);
		std::cout<<"Donwload url: "<<downloadUrl<<std::endl;

		download_song(to_http(downloadUrl));
	}
}
