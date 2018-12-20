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
    ./music-downloader --lyrics SONG [--save FILE] [--hide] [-v | --verbose]
    ./music-downloader --download SONGS... [--dest FOLDER] [-v | --verbose]
    ./music-downloader --play FILES... [--dir FOLDER] [--show-lyrics] [--show-play-output] [-v | --verbose]
    ./music-downloader --play-song SONGS... [--keep] [--show-lyrics] [-v | --verbose]

Options:
    -h --help             Prints this message.
    --songs FILE          Text file containing songs to download [default: songs.txt]
    --dest FOLDER         Destination folder (where downloaded songs are saved) [default: songs/]
    -v --verbose          Use verbose output
    --lyrics SONG         Name of song to find the lyrics of [default: ]
    --save FILE           File to save the lyrics to [default: ]
    --hide                Doesn't print the lyrics to the terminal
    --download SONGS...   List of songs to download
    --play FILES...       List of MP3 files to play
    --dir FOLDER          The folder containing the files to play [default: .]
    --show-lyrics         Prints lyrics of song to the screen
    --show-play-output    Does not use quiet flag when running play command
    --play-song SONGS...  Song to search for online and then play if found
    --keep                Keep a saved .mp3 of the song
```
Examples include

* `./music-downloader` This will download all the songs in songs.txt and save them in a folder named songs
* `./music-downloader --lyrics "Thriller"` This will search for the song Thriller and print its lyrics
* `./music-downloader --dest music -v` This will download all the songs in songs.txt and save them in a folder named music. It will also print verbose output.
* `./music-downloader --download "canon in d"` This will download Canon in D and save is in a folder names songs.
* `./music-downloader --play songs/thriller.mp3 --show-lyrics` This will play Thriller from a local MP3 file, as well as search for and print its lyrics to the terminal.
* `./music-downloader --play-song "familia nicki" --show-lyrics` This will download Familia and then immediately start playing it (it will also search for and print its lyrics). The downloaded song is deleted whenever it finishes playing (use `--keep` if you want to keep the mp3).

## Advanced Examples

You could run the below command to play every song in a folder in a random order, and have the program print the lyrics of each song before playing it.

`./music-downloader --play $(ls songs | shuf) --dir songs --show-lyrics`

Since song files may have hard to remember names, if you want to play specific songs without typing out annoying file names, you can also run a command like below.

`./music-downloader --play mj/$(ls mj | grep -i thriller) kw/$(ls kw | grep -i stronger)`

The above will search for a song with thriller (respectively, strong) in the title, and play it. Since the two songs were in different folders, instead of using "--dir", we have to include the folder with the name of the file.

## Details
- The program retrieves the top 3 results for each song, and selects one to download
- You can specify `--play` multiple times. Either of the below is acceptable:
  - `./music-downloader --play thriller.mp3 --play manmirror.mp3`
  - `./music-downloader --play thriller.mp3 manmirror.mp3`
- `--play-song` and `--download` can also be given multiple songs in the same way.
- The program retrieves lyrics from multiple sites, scores them, and only keeps the lyrics with the highest score.
  - The scoring is based off of
    - order (top search results get a higher default score)
    - length (avoid sites with incomplete lyrics and favor sites that do things like label the chorus and different verses and whatnot)
    - similarity between url and song (try to make sure we're getting lyrics from the right song)
- When playing music using the `--play` flag, you can
  - skip a song by pressing `Ctrl+c`
  - pause a song by pressing `Crtl+z` and later resume by running `%` from your terminal

## TODO
This project is still not done/perfect. The following are things I would like to eventually do
- [ ] Make the program more interactive (menu or REPL like thing most likely)
- [ ] Add ability to supply Spotify credentials in order for program to download your saved songs

Note: These have been TODOs since practically [day](https://github.com/NivenT/music-downloader/tree/d6dd0f68a7ab92557f1a0d016c0e0823568cd28e) [one](https://github.com/NivenT/music-downloader/tree/e01d9f5c1b7b0570e15377b0eeee980bd74815cc) so don't hold your breath.
