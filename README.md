# Music Downloader
[![Build Status](https://travis-ci.org/NivenT/music-downloader.svg?branch=master)](https://travis-ci.org/NivenT/music-downloader)

Automatically downloads music

This is a simple program to download music for you. You provide it a file containing a list of songs (one title per line). It searches for them on youtube, and then uses a youtube -> mp3 converter to download them as .mp3 files to a directory of your choice.

## How to Build
To build, clone this repository and then use cmake and make. The commands you need to run should be similar to those below
```
git clone https://github.com/NivenT/music-downloader
cd music-downloader
git submodule update --init --recursive
mkdir build
cd build
cmake ../
make
```

## How to Run
Run the program from terminal. The usage is as follows:
```
./music-downloader

Usage:
  ./music-downloader (-h | --help)
  ./music-downloader [--songs FILE] [--dest FOLDER] [-v | --verbose]
  ./music-downloader --lyrics SONG [--save FILE] [--hide]
  ./music-downloader --download SONG [--dest FOLDER] [-v | --verbose]
  ./music-downloader --play FILES... [--show-lyrics] [--show-play-output]

Options:
  -h --help             Prints this message.
  --songs FILE          Text file containing songs to download [default: songs.txt]
  --dest FOLDER         Destination folder (where downloaded songs are saved) [default: songs/]
  -v --verbose          Use verbose output
  --lyrics SONG         Name of song to find the lyrics of [default: ]
  --save FILE           File to save the lyrics to [default: ]
  --hide                Doesn't print the lyrics to the terminal
  --download SONG       Downloads a single song [default: ]
  --play FILES...       List of MP3 files to play
  --show-lyrics         Prints lyrics of song to the screen
  --show-play-output    Does not use quiet flag when running play command
```
Examples include

* `./music-downloader` This will download all the songs in songs.txt and save them in a folder named songs
* `./music-downloader --lyrics "Thriller"` This will search for the song Thriller and print its lyrics
* `./music-downloader --dest music -v` This will download all the songs in songs.txt and save them in a folder named music. It will also print verbose output.
* `./music-downloader --download "Thriller"` This will download Thriller and save is in a folder names songs.
* `./music-downloader --play songs/thriller.mp3 --show-lyrics` This will play Thriller from a local MP3 file, as well as search for and print its lyrics to the terminal.

## Details
- The program retrieves the top 3 results for each song, and selects one to download
- You can specify `--play` multiple times, but each time only gets one file
  - `./music-downloader --play thriller.mp3 --play manmirror.mp3` is acceptable
  - `./music-downloader --play thriller.mp3 manmirror.mp3` is not acceptable

## TODO
This project is still not done/perfect. The following are things I would like to eventually do
- [ ] Make the program more interactive (menu or REPL like thing most likely)
- [ ] Add ability to supply Spotify credentials in order for program to download your saved songs (Not sure if I'll ever get around to this)
