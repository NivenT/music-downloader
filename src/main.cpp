#include <fstream>

#include <cpr/cpr.h>
#include <json.hpp>

using json = nlohmann::json;

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
	auto html = cpr::Get(cpr::Url{url});

	std::cout<<"url: "<<url<<std::endl
		 	 <<"status code: "<<html.status_code<<std::endl
			 <<"HTML Response:"<<std::endl
			 <<html.text;
	return html.text;
}

int main(int argc, char** argv) {
	const std::string christmas = "http://dl3.downloader.space/dl.php?id=e506ebfd91227183a20f223ec8479998";
	const std::string christmas2 = "pFjdfjrtf1Q";

	youtube_to_download(christmas2);
	/*
    auto response = cpr::Get(cpr::Url{christmas});
    write_to_mp3("Thats_Christmas_To_Me.mp3", response.text);
    */
}
