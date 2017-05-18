#include <iostream>
#include <fstream>

#include <docopt.h>

#include "util.h"
#include "web.h"
#include "youtube.h"
#include "lyrics.h"

#define ILLEGAL_CHARCTERS {".", "|", ":", "\"", "'", "(", ")", "&", "[", "]"}

using namespace std;

static const char* USAGE =
R"({progName}

Usage:
  {progName} (-h | --help)
  {progName} [--songs FILE] [--dest FOLDER] [-v | --verbose]
  {progName} --lyrics SONG [--save FILE] [--hide]
  {progName} --download SONG [--dest FOLDER] [-v | --verbose]
  {progName} --play FILES... [--dir FOLDER] [--show-lyrics] [--show-play-output]

Options:
  -h --help             Prints this message.
  --songs FILE          Text file containing songs to download [default: songs.txt]
  --dest FOLDER	        Destination folder (where downloaded songs are saved) [default: songs/]
  -v --verbose          Use verbose output
  --lyrics SONG         Name of song to find the lyrics of [default: ]
  --save FILE           File to save the lyrics to [default: ]
  --hide                Doesn't print the lyrics to the terminal
  --download SONG       Downloads a single song [default: ]
  --play FILES...       List of MP3 files to play
  --dir FOLDER          The folder containing the files to play [default: .]
  --show-lyrics         Prints lyrics of song to the screen
  --show-play-output    Does not use quiet flag when running play command
)";

void download_song(const string& apikey, const string& song, const string& saveFolder, 
					bool verbose, map<string, set<string>>& stats) {
	static const float SIMILARITY_THRESHOLD = 0.63;

	string songId, songTitle;
	tie(songId, songTitle) = search_youtube_for_song(song, apikey, verbose);

	string fileTitle = saveFolder + replace_all(songTitle, {{"/", "\\"}, ILLEGAL_CHARCTERS}, {"_", ""});
	string match;

	if (songId == "") {
		cout<<"\""<<song<<"\" could not be found"<<endl;

		stats["not found"].insert(song);
	} else if (song_exists(fileTitle)) {
		cout<<"\""<<song<<"\" has already been downloaded"<<endl;

		stats["already existed"].insert(song);
	} else if ((match = song_probably_exists(fileTitle, saveFolder)) != "") {
		cout<<"\""<<song<<"\" has likely already been downloaded"<<endl
			<<"\""<<match<<"\" was found which is a close match"<<endl;

		stats["probably already existed, and so were not downloaded"].insert(song + " -> " + match);
	} else {
		if (verbose) {
			cout<<TAB<<"Downloading video with Id "<<songId<<"..."<<endl;
		}

		const string downloadUrl = youtube_to_download(songId);
		if (downloadUrl == "") {
			cout<<"Could not find song"<<endl
				<<endl;

			stats["not found"].insert(song);
			return;
		} else if (verbose) {
			cout<<TAB<<"Donwload url: "<<downloadUrl<<endl;
		}

		string songData; bool succ;
		tie(succ, songData) = download_song(downloadUrl);
		if (succ) {
			cout<<"Successfully downloaded "<<songTitle<<endl;
		  	write_to_mp3(fileTitle, songData, verbose);

		  	if (title_distance(song, songTitle) >= SIMILARITY_THRESHOLD) {
		  		stats["downloaded, but were likely not the songs you wanted"].insert(song + " -> " + songTitle);
		  	} else {
		  		stats["successfully downloaded"].insert(song);
		  	}
		} else {
			cout<<"Failed to download "<<songTitle<<endl;

			stats["could not be downloaded"].insert(song);
		}
	}
}

void download_songs(const string& apikey, const string& songList, const string& saveFolder, 
					bool verbose, map<string, set<string>>& stats) {
	cout<<"Downloading songs from file \""<<songList<<"\" and saving them in folder \""<<saveFolder<<"\""<<endl
			 <<endl;

	ifstream songFile(songList.c_str());

	string song;
	while (getline(songFile, song)) {
		if (!starts_with(song, "added on:")) {
			// just to make sure statistics print in alphabetical order
			transform(song.begin(), song.end(), song.begin(), ::tolower);

			download_song(apikey, song, saveFolder, verbose, stats);
			cout<<endl;
		}
	}
}

void print_statistics(map<string, set<string>> stats) {
	static const int PRINT_THRESHOLD = 15;

	cout<<"*******Download summary*******"<<endl
			 <<endl;
	for (const auto& pair : stats) {
		const string& msg = pair.first;
		const set<string>& data = pair.second;

		cout<<data.size()<<" songs "<<msg<<". They were:"<<endl;
		if (data.size() <= PRINT_THRESHOLD) {
			for (const auto& song : data) {
				cout<<TAB<<song<<endl;
			}
		} else {
			int nend = 0.3*PRINT_THRESHOLD;
			int count = PRINT_THRESHOLD-nend;

			for (auto it = data.begin(); count > 0; ++it, --count) {
				cout<<TAB<<(*it)<<endl;
			}
			cout<<TAB<<"."<<endl
				<<TAB<<"."<<endl
				<<TAB<<"."<<endl;

			auto it = data.begin();
			for (int skip = data.size() - nend; skip > 0; ++it, --skip);
			while (nend-- > 0) cout<<TAB<<*(it++)<<endl;
		}
		cout<<endl;
	}
}

void get_lyrics(const string& song, const string& saveFile, bool print) {
	static const int NUM_RESULTS = 25;

	cout<<"Searching for the lyrics of \""<<song<<"\""<<endl
	    <<endl;

	string search_results = search_duckduckgo(song + " lyrics");
	auto links = match_regex(search_results, R"([[:alpha:]]+\.com[[:alnum:]/\.-]+)", NUM_RESULTS);

	bool found = false;
	string lyrics;
	for (auto it = links.begin(); it != links.end() && !found; ++it) {
		const auto url = *it;

		if (ends_with(url, ".")) {
			continue;
		} else if (starts_with(url, "metrolyrics")) {
			tie(found, lyrics) = get_metrolyrics(url);
		} else if (starts_with(url, "genius")) {
			tie(found, lyrics) = get_genius(url);
		} else if (starts_with(url, "lyricsbox")) {
			tie(found, lyrics) = get_lyricsbox(url);
		} else if (starts_with(url, "songlyrics")) {
			tie(found, lyrics) = get_songlyrics(url);
		}
	}

	if (!found) {
		cout<<"Unable to find lyrics"<<endl;
	} else {
		if (print) {
			cout<<lyrics<<endl
					 <<endl;
		}
		if (saveFile != "") {
			save_lyrics(saveFile, lyrics);
		}
	}
}

bool read_tag(const string& data, string& title, string& artist) {
	static const int HEADER_SIZE = 10;

	bool extendedHeader = data[5] & 0x40;
	// If this isn't a clear line, I don't know what is
	int size = from_synchsafe(rev_bytes(*(int*)&data[6]));
	int offset = extendedHeader ? from_synchsafe(rev_bytes(*(int*)&data[HEADER_SIZE])) : 0;

	for (int pos = HEADER_SIZE+offset; pos < HEADER_SIZE+size;) {
		string frameID = data.substr(pos, 4);
		int frame_size = from_synchsafe(rev_bytes(*(int*)&data[pos+4]));

		// offset by 1 to ignore encoding byte
		if (frameID == "TIT2") title = trim(data.substr(pos+HEADER_SIZE+1, frame_size-1));
		else if (frameID == "TPE1") artist = trim(data.substr(pos+HEADER_SIZE+1, frame_size-1));

		pos += frame_size + HEADER_SIZE;
	}

	return title != "" /* && artist != "" */; // Just title might be enough
} 

void play_song(const string& file, bool show_lyrics, bool show_output) {
	cout<<"Playing "<<file<<endl;

	if (!ends_with(file, ".mp3")) {
		cout<<"Must supply an MP3 file"<<endl;
		return;
	} else if (show_lyrics) {
		string data;
		if (!read_file(file, data)) {
			// If we can't read file, play probably can't either, so exit
			cout<<"Could not read file"<<endl;
			return;
		}

		// If something goes wrong with lyrics, can still play song
		string title, artist;
		if (!starts_with(data, "ID3")) {
			cout<<"MP3 file must use ID3v2 tag if you want lyrics"<<endl;
		} else if (!read_tag(data, title, artist)) {
			cout<<"Could not extract title and artist information from MP3"<<endl;
		} else {
			cout<<"The song is \""<<title<<"\" by \""<<(artist == "" ? "unkown" : artist)<<"\""<<endl;
			get_lyrics(trim(artist + " " + title), "", true);
		}
	}

	string cmd = "play " + shellify(file) + (show_output ? "" : " -q");
	cout<<"Running command \""<<cmd<<"\""<<endl;
	// TODO: make this line OS agnostic
	system(cmd.c_str());
}

// TODO: Restructure this in a slightly cleaner way
vector<char*> splitPlayArgs(int argc, char** argv) {
	vector<char*> args;
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--play") == 0) {
			while (argv[++i] && !starts_with(argv[i], "-")) {
				args.push_back(((char*)"--play"));
				args.push_back(argv[i]);
			}
		}
		if (i < argc) args.push_back(argv[i]);
	}
	return args;
}

int main(int argc, char** argv) {
	auto vargv = splitPlayArgs(argc, argv);
	map<string, docopt::value> args =
		docopt::docopt(replace_all(USAGE, "{progName}", argv[0]),
		               {vargv.data()+1, vargv.data() + vargv.size()});

	string apikey = "AIzaSyDxmk_iusdpHuj5VfFnqyvweW1Lep0j2oc", 
		   songList = args["--songs"].asString(), 
	       saveFolder = args["--dest"].asString(),
		   song = args["--lyrics"].asString(),
		   saveFile = args["--save"].asString(),
		   songFolder = args["--dir"].asString();

	bool print = !args["--hide"].asBool(),
		 verbose = args["--verbose"].asBool(),
		 lyrics = args["--show-lyrics"].asBool(),
		 playout = args["--show-play-output"].asBool();

	vector<string> songs = args["--play"].asStringList();

	map<string, set<string>> stats;
	saveFolder += ends_with(saveFolder, "/") ? "" : "/";
	songFolder += ends_with(songFolder, "/") ? "" : "/"; 

	if (song != "") {
		get_lyrics(song, saveFile, print);
	} else if ((song = args["--download"].asString()) != "") {
		cout<<"Downloading \""<<song<<"\" and saving song in \""<<saveFolder<<"\""<<endl
	         <<endl;
		download_song(apikey, song, saveFolder, verbose, stats);
	} else if (!songs.empty()) {
		static const string stars(100, '*');

		for (int i = 0; i < songs.size(); i++) {
			cout<<stars<<endl;
			play_song(songFolder + songs[i], lyrics, playout);
		}
	} else {
		download_songs(apikey, songList, saveFolder, verbose, stats);
		print_statistics(stats);
	}
	return 0;
}
