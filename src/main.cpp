#include <fstream>

#include <cpr/cpr.h>
#include <json.hpp>

using json = nlohmann::json;

bool starts_with(const std::string& str, const std::string& prefix) {
	return str.find(prefix) == 0;
}

bool ends_with(const std::string& str, const std::string& suffix) {
	return str.find_last_of(suffix) == str.size() - 1;
}

std::vector<std::string> split(const std::string& str, char delim) {
	std::vector<std::string> elems;
	std::stringstream ss(str);

	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

void write_to_mp3(const std::string& title, const std::string& data) {
	std::string path = ends_with(title, ".mp3") ? title : title + ".mp3";

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
		   starts_with(url, "http")  ? url : 
		   							   "http" + url;
}

void download_song(const std::string& url, const std::string& title = "") {
	// I think it sometimes takes multiple requests for the song to finish downloading
	// Could be wrong. Here in the mean time just in case
	static const int MAX_NUM_REQUESTS = 10;
	for (int i = 0; i < MAX_NUM_REQUESTS; ++i) {
		auto response = cpr::Get(cpr::Url{url});
		std::cout<<response.text<<std::endl;

		auto tokens = split(response.text, '|');
		if (tokens[0] == "OK" && tokens.size() == 4) {
			const std::string songUrl = "http://dl" + tokens[1] + ".downloader.space/dl.php?id=" + tokens[2];
			response = cpr::Get(cpr::Url{songUrl});

			write_to_mp3(title == "" ? tokens[3] : title, response.text);
			return;
		}
	}
}

void set_params(int argc, char** argv, std::string& title, std::string& ytID) {
	switch(argc) {
		case 1:
			title = "Thats_Christmas_To_Me";
			ytID = "pFjdfjrtf1Q";
			break;

		case 2:
			title = "";
			ytID = argv[1];
			break;

		default:
			title = argv[2];
			ytID = argv[1];
			break;
	}
}

int main(int argc, char** argv) {
	std::string title, ytID;
	set_params(argc, argv, title, ytID);

	std::cout<<"Downloading "<<title<<" (ID = "<<ytID<<")..."<<std::endl;
	const std::string downloadUrl = youtube_to_download(ytID);
	std::cout<<"Donwload url: "<<downloadUrl<<std::endl;

	download_song(to_http(downloadUrl), title);
}
