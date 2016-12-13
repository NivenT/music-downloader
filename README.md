# Music Downloader

Automatically downloads music

This is a simple program to download music for you. You provide it a file containing a list of songs (one title per line). It searches for them on youtube, and then uses a youtube -> mp3 converter to download them as .mp3 files to a directory of your choice.

## How to Build
To build, clone this repository and build using cmake and make. The commands you need to run should be similar to those below
```
git clone https://github.com/NivenT/music-downloader
cd music-downloader
mkdir build
cd build
cmake ../
make
```

## How to Run
Run the program from terminal
```
./music-downloader <apikey> <song file> <song folder>
```
Up to three parameters can be specified (the first is required, the last two optional)

* apikey - A valid YouTube API key
* song file - A text file containing a list of songs to download (defualt: songs.txt)
* song folder - The directory to save the downloaded song to (defualt: songs/)

## Details
- The program retrieves the top 3 results for each song, and selects one to download
- Sometimes, none of the top 3 results are the song you wanted
- Sometimes, no results are found for your song
- Sometimes, the wrong song is chosen
- Most of the time, the correct song is found and downloaded

## TODO
This project is still not done/perfect. The following are things I would like to eventually do
- [ ] Add --help flag
- [ ] Add flags for specifying certain parameters so, for example, you can change the folder without chaning the file
- [ ] Add more parameters
- [ ] Add ability to supply Spotify credentials in order for program to download your saved songs (Not sure if I'll ever get around to this)
