#include <iostream>
#include <fstream>

#include "util.h"
#include "download.h"

void set_params(int argc, char** argv, std::string& apikey, std::string& songList, std::string& saveFolder) {
	switch(argc) {
		case 1:
			std::cout<<"Must provide a valid youtube api key as the first argument"<<std::endl;
			exit(0xBAD);
			break;

		case 2:
			apikey = argv[1];
			songList = "songs.txt";
			saveFolder = "songs";
			break;

		case 3:
			apikey = argv[1];
			songList = argv[2];
			saveFolder = "songs";
			break;

		default:
			apikey = argv[1];
			songList = argv[2];
			saveFolder = argv[3];
			break;
	}
}

void download_songs(const std::string& apikey, const std::string& songList, const std::string& saveFolder) {
	std::ifstream songFile(songList.c_str());

	std::string song;
	while (std::getline(songFile, song)) {
		if (starts_with(song, "added on:")) continue;
		for (const auto& songId : search_youtube_for_song(song, apikey)) {
			std::cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<std::endl;
			const std::string downloadUrl = youtube_to_download(songId);
			std::cout<<TAB<<"Donwload url: "<<downloadUrl<<std::endl;
			download_song(to_http(downloadUrl), saveFolder);
			std::cout<<std::endl;
		}
		std::cout<<std::endl;
	}
}

int main(int argc, char** argv) {
	std::string apikey, songList, saveFolder;
	set_params(argc, argv, apikey, songList, saveFolder);
	download_songs(apikey, songList, saveFolder);
}
