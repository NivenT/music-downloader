#include <iostream>
#include <fstream>

#include <docopt.h>

#include "util.h"
#include "web.h"
#include "youtube.h"
#include "lyrics.h"

static const char* USAGE =
R"({progName}

Usage:
  {progName} (-h | --help)
  {progName} [--songs FILE] [--dest FOLDER] [-v | --verbose]
  {progName} --lyrics SONG [--save FILE] [--hide]
  {progName} --download SONG [--dest FOLDER] [-v | --verbose]

Options:
  -h --help         Prints this message.
  --songs FILE      Text file containing songs to download [default: songs.txt]
  --dest FOLDER	    Destination folder (where downloaded songs are saved) [default: songs/]
  -v --verbose      Use verbose output
  --lyrics SONG     Name of song to find the lyrics of [default: ]
  --save FILE       File to save the lyrics to [default: ]
  --hide            Doesn't print the lyrics to the terminal
  --download SONG   Downloads a single song [default: ]
)";

void download_song(const std::string& apikey, const std::string& song, const std::string& saveFolder, bool verbose) {
	std::string songId, songTitle;
	std::tie(songId, songTitle) = search_youtube_for_song(song, apikey, verbose);

	std::string fileTitle = saveFolder + replace_all(replace_all(songTitle, "/", "_"), ".", "");

	if (songId == "") {
		std::cout<<"\""<<song<<"\" could not be found"<<std::endl;
	} else if (song_exists(fileTitle)) {
		std::cout<<"\""<<song<<"\" has already been downloaded"<<std::endl;
	} else {
		if (verbose) {
			std::cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<std::endl;
		}

		const std::string downloadUrl = youtube_to_download(songId);
		if (downloadUrl == "") {
			std::cout<<"Could not find song"<<std::endl
					 <<std::endl;
			return;
		} else if (verbose) {
			std::cout<<TAB<<"Donwload url: "<<downloadUrl<<std::endl;
		}

		std::string songData = download_song(downloadUrl);
		if (songData != "") {
			std::cout<<"Successfully downloaded "<<songTitle<<std::endl;
		  	write_to_mp3(fileTitle, songData, verbose);
		} else {
			std::cout<<"Failed to download "<<songTitle<<std::endl;
		}
	}
}

void download_songs(const std::string& apikey, const std::string& songList, const std::string& saveFolder, bool verbose) {
	std::cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<std::endl
			 <<std::endl;

	std::ifstream songFile(songList.c_str());

	std::string song;
	while (std::getline(songFile, song)) {
		if (!starts_with(song, "added on:")) {
			download_song(apikey, song, saveFolder, verbose);
			std::cout<<std::endl;
		}
	}
}

void get_lyrics(const std::string& song, const std::string& saveFile, bool print) {
	std::cout<<"Searching for the lyrics of \""<<song<<"\""<<std::endl
	         <<std::endl;

	std::string search_results = search_duckduckgo(song + " lyrics");
	auto links = match_regex(search_results, R"([[:alpha:]]+\.com[[:alnum:]/\.-]+)", 20);

	bool found = false;
	std::string lyrics;
	for (auto it = links.begin(); it != links.end() && !found; ++it) {
		const auto url = *it;
		if (starts_with(url, "metrolyrics")) {
			std::tie(found, lyrics) = get_metrolyrics(url);
		} else if (starts_with(url, "genius")) {
			std::tie(found, lyrics) = get_genius(url);
		} else if (starts_with(url, "lyricsbox")) {
			std::tie(found, lyrics) = get_lyricsbox(url);
		} else if (starts_with(url, "songlyrics")) {
			std::tie(found, lyrics) = get_songlyrics(url);
		}
	}

	if (!found) {
		std::cout<<"Unable to find lyrics"<<std::endl;
	} else {
		if (print) {
			std::cout<<lyrics<<std::endl
					 <<std::endl;
		}
		if (saveFile != "") {
			save_lyrics(saveFile, lyrics);
		}
	}
}

int main(int argc, char** argv) {
	std::map<std::string, docopt::value> args =
		docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
		               {argv+1, argv+argc});

	std::string apikey = "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc", 
				songList = args["--songs"].asString(), 
				saveFolder = args["--dest"].asString(),
				song = args["--lyrics"].asString(),
				saveFile = args["--save"].asString();
	bool print = !args["--hide"].asBool(),
		 verbose = args["--verbose"].asBool();

	saveFolder = saveFolder + (ends_with(saveFolder, "/") ? "" : "/");
	if (song != "") {
		get_lyrics(song, saveFile, print);
	} else if ((song = args["--download"].asString()) != "") {
		std::cout<<"Downloading \""<<song<<"\" and saving song in \""<<saveFolder<<"\""<<std::endl
	         <<std::endl;
		download_song(apikey, song, saveFolder, verbose);
	} else {
		download_songs(apikey, songList, saveFolder, verbose);
	}
	return 0;
}
