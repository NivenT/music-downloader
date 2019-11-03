#pragma once

#include <tuple>
#include <string>

#include "web.h"

class YTConverter {
private:
    const std::string m_name;
    const size_t MAX_NUM_ATTEMPTS;
protected:
    bool check_successful_response(const cpr::Response& response) const {
        return ::check_successful_response(response, m_name);
    }
public:
    YTConverter(const std::string& name, size_t mna) : m_name(name), MAX_NUM_ATTEMPTS(mna) {}
    virtual ~YTConverter() {}

    virtual std::string get_link(const std::string& id) = 0;
    virtual std::tuple<bool, std::string> download_song(const std::string& url, bool verbose);

    std::string get_name() const { return m_name; }
};

class ConvertMP3 : public YTConverter {
public:
    ConvertMP3() : YTConverter("ConvertMP3", 20) {}
    std::string get_link(const std::string& id);
};

class PointMP3 : public YTConverter {
public:
    PointMP3() : YTConverter("PointMP3", 40) {}
    std::string get_link(const std::string& id);
};