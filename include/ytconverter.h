#pragma once

#include <tuple>
#include <string>

#include "web.h"

class YTConverter {
private:
    const std::string m_name;
    const bool verbose;
    const size_t MAX_NUM_DOWNLOAD_ATTEMPTS;
    const size_t MAX_NUM_LINK_ATTEMPTS;
protected:
    bool check_successful_response(const cpr::Response& response) const {
        return ::check_successful_response(response, m_name, verbose);
    }
    virtual cpr::Response try_download(const std::string& url);
    virtual std::string try_get_link(const std::string& id) = 0;
public:
    YTConverter(const std::string& name, size_t mnda, size_t mnla, bool ver) : 
        m_name(name), MAX_NUM_DOWNLOAD_ATTEMPTS(mnda), MAX_NUM_LINK_ATTEMPTS(mnla), verbose(ver) {}
    virtual ~YTConverter() {}

    std::string get_link(const std::string& id);
    std::tuple<bool, std::string> download_song(const std::string& url);

    std::string get_name() const { return m_name; }
};

class ConvertMP3 : public YTConverter {
public:
    ConvertMP3(bool verbose) : YTConverter("ConvertMP3", 10, 3, verbose) {}
    std::string try_get_link(const std::string& id);
};

class PointMP3 : public YTConverter {
private:
    // Set by get_link and used in download_song
    std::string m_referrer;

    cpr::Response try_download(const std::string& url);
public:
    PointMP3(bool verbose) : YTConverter("PointMP3", 10, 3, verbose) {}
    std::string try_get_link(const std::string& id);
};

class ThreeTwentyYT : public YTConverter {
private:
public:
    ThreeTwentyYT(bool verbose) : YTConverter("320Youtube", 20, 3, verbose) {}
    std::string try_get_link(const std::string& id);
};

class MP3Convert : public YTConverter {
private:
public:
    MP3Convert(bool verbose) : YTConverter("mp3-convert", 30, 10, verbose) {}
    std::string try_get_link(const std::string& id);
};
