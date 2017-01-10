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

Options:
  -h --help         Prints this message.
  --songs FILE      Text file containing songs to download [default: songs.txt]
  --dest FOLDER	    Destination folder (where downloaded songs are saved) [default: songs/]
  -v --verbose      Use verbose output
  --lyrics SONG     Name of song to find the lyrics of [default: ]
  --save FILE       File to save the lyrics to [default: ]
  --hide            Doesn't print the lyrics to the terminal

```
Examples include

* `./music-downloader` - This will download all the songs in songs.txt and save them in a folder named songs
* `./music-downloader --lyrics "Thriller"` - This will search for the song Thriller and print its lyrics

## Details
- The program retrieves the top 3 results for each song, and selects one to download
- Lyrics searching is still a work in progress

## TODO
This project is still not done/perfect. The following are things I would like to eventually do
- [ ] Make the program more interactive (menu or REPL like thing most likely)
- [ ] Add ability to supply Spotify credentials in order for program to download your saved songs (Not sure if I'll ever get around to this)
