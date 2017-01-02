#include <iostream>
#include <fstream>

#include <docopt.h>

#include "util.h"
#include "web.h"
#include "youtube.h"

static const char* USAGE =
R"({progName}

Usage:
  {progName} (-h | --help)
  {progName} [--songs FILE] [--dest FOLDER]

Options:
  -h --help         Prints this message.
  --songs FILE      Text file containing songs to download [default: songs.txt]
  --dest FOLDER	    Destination folder (where downloaded songs are saved) [default: songs/]
)";

void download_songs(const std::string& apikey, const std::string& songList, const std::string& saveFolder) {
	std::cout<<"Downloading songs from file "<<songList<<" and saving them in folder "<<saveFolder<<std::endl;

	std::ifstream songFile(songList.c_str());

	std::string song;
	while (std::getline(songFile, song)) {
		if (starts_with(song, "added on:")) continue;

		std::string songId, songTitle;
		std::tie(songId, songTitle) = search_youtube_for_song(song, apikey);

		if (songId == "") {
			std::cout<<song<<" could not be found"<<std::endl;
		} else {
			std::cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<std::endl;

			const std::string downloadUrl = youtube_to_download(songId);
			std::cout<<TAB<<"Donwload url: "<<downloadUrl<<std::endl;

			std::string songData = download_song(to_http(downloadUrl), saveFolder);
			if (songData != "") {
			  write_to_mp3(saveFolder + songTitle, songData);
			}
		}
		std::cout<<std::endl;
	}
}

int main(int argc, char** argv) {
	std::map<std::string, docopt::value> args =
		docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
		               {argv+1, argv+argc});


	std::string apikey = "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc", 
				songList = args["--songs"].asString(), 
				saveFolder = args["--dest"].asString();
	download_songs(apikey, songList, saveFolder + (ends_with(saveFolder, "/") ? "" : "/"));

	return 0;
}
