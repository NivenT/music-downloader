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

void write_to_mp3(const std::string& title, const std::string& data) {
	std::string path = ends_with(title, ".mp3") ? title : title + ".mp3";

	std::cout<<"Saving song to "<<path<<std::endl;
	std::ofstream file(path.c_str());

	file.write(data.c_str(), data.size());
	file.close();
}

std::string youtube_to_download(const std::string& id) {
	std::string url = "http://savevideos.xyz/api?v=" + id;

	auto response = cpr::Get(cpr::Url{url});
	auto json = json::parse(response.text);
	/*
	std::cout<<"response:"<<std::endl
			 <<json.dump(4)<<std::endl
			 <<std::endl;
	*/
	return json["audio"][0]["url"];
}

std::string to_http(const std::string& url) {
	return starts_with(url, "https") ? "http" + url.substr(5) :
		   starts_with(url, "http")  ? url : 
		   							   "http" + url;
}

void download_song(const std::string& url) {
	auto response = cpr::Get(cpr::Url{url});
	//auto json = json::parse(response.text);
	/*
	std::cout<<"url 2: "<<url<<std::endl
			 <<"response 2:"<<std::endl
			 <<response.text<<std::endl
			 <<std::endl;
	*/
	std::cout<<response.text;
}

int main(int argc, char** argv) {
	const std::string christmas = "http://dl3.downloader.space/dl.php?id=e506ebfd91227183a20f223ec8479998";
	const std::string christmas2 = "pFjdfjrtf1Q";

	const std::string downloadUrl = youtube_to_download(christmas2);
	download_song(to_http(downloadUrl));
	/*
    auto response = cpr::Get(cpr::Url{christmas});
    write_to_mp3("Thats_Christmas_To_Me.mp3", response.text);
    */
}
