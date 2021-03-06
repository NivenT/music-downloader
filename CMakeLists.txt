cmake_minimum_required(VERSION 2.8.7)
project(music-downloader)

if(NOT ${CMAKE_VERSION} LESS 3.2)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(STATUS "Checking compiler flags for C++11 support.")
    # Set C++11/14 support flags for various compilers
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    if(COMPILER_SUPPORTS_CXX14)
        message(STATUS "C++14 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
        endif()
    elseif(COMPILER_SUPPORTS_CXX11)
        message(STATUS "C++11 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} does not have recent enough C++ support. Please use a different C++ compiler.")
    endif()
endif()

add_subdirectory(opt)

add_executable(music-downloader src/main.cpp 
                                src/util.cpp 
                                src/youtube.cpp 
                                src/web.cpp 
                                src/lyrics.cpp
                                src/play.cpp
                                src/download_song.cpp
                                src/search_and_play.cpp
                                src/ytconverter.cpp)
target_link_libraries(music-downloader ${CPR_LIBRARIES} docopt)
include_directories(${CPR_INCLUDE_DIRS} ${JSON_INCLUDE_DIRS} ${DOCOPT_INCLUDE_DIRS} include)
