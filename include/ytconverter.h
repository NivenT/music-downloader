#pragma once

#include <tuple>
#include <string>

class YTConverter {
private:
    const std::string m_name;
public:
    YTConverter(const std::string& name) : m_name(name) {}
    virtual ~YTConverter() {}

    virtual std::string get_link(const std::string& id) = 0;
    virtual std::tuple<bool, std::string> download_song(const std::string& url) = 0;
};

class ConvertMP3 : public YTConverter {
public:
    ConvertMP3() : YTConverter("ConvertMP3") {}
};
