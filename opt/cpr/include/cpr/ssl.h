#ifndef CPR_SSL_H
#define CPR_SSL_H

#include <cstring>

#include <initializer_list>
#include <string>

#include "defines.h"

// Literally just copied Body 5 times...
namespace cpr {

class SSLCert : public std::string {
  public:
    SSLCert() = default;
    SSLCert(const SSLCert& rhs) = default;
    SSLCert(SSLCert&& rhs) = default;
    SSLCert& operator=(const SSLCert& rhs) = default;
    SSLCert& operator=(SSLCert&& rhs) = default;
    explicit SSLCert(const char* raw_string) : std::string(raw_string) {}
    explicit SSLCert(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit SSLCert(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit SSLCert(const std::string& std_string) : std::string(std_string) {}
    explicit SSLCert(const std::string& std_string, size_t position, size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit SSLCert(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit SSLCert(InputIterator first, InputIterator last)
            : std::string(first, last) {}
};

class SSLCertType : public std::string {
  public:
    SSLCertType() = default;
    SSLCertType(const SSLCertType& rhs) = default;
    SSLCertType(SSLCertType&& rhs) = default;
    SSLCertType& operator=(const SSLCertType& rhs) = default;
    SSLCertType& operator=(SSLCertType&& rhs) = default;
    explicit SSLCertType(const char* raw_string) : std::string(raw_string) {}
    explicit SSLCertType(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit SSLCertType(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit SSLCertType(const std::string& std_string) : std::string(std_string) {}
    explicit SSLCertType(const std::string& std_string, size_t position, size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit SSLCertType(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit SSLCertType(InputIterator first, InputIterator last)
            : std::string(first, last) {}
};

class SSLKey : public std::string {
  public:
    SSLKey() = default;
    SSLKey(const SSLKey& rhs) = default;
    SSLKey(SSLKey&& rhs) = default;
    SSLKey& operator=(const SSLKey& rhs) = default;
    SSLKey& operator=(SSLKey&& rhs) = default;
    explicit SSLKey(const char* raw_string) : std::string(raw_string) {}
    explicit SSLKey(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit SSLKey(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit SSLKey(const std::string& std_string) : std::string(std_string) {}
    explicit SSLKey(const std::string& std_string, size_t position, size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit SSLKey(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit SSLKey(InputIterator first, InputIterator last)
            : std::string(first, last) {}
};

class SSLKeyType : public std::string {
  public:
    SSLKeyType() = default;
    SSLKeyType(const SSLKeyType& rhs) = default;
    SSLKeyType(SSLKeyType&& rhs) = default;
    SSLKeyType& operator=(const SSLKeyType& rhs) = default;
    SSLKeyType& operator=(SSLKeyType&& rhs) = default;
    explicit SSLKeyType(const char* raw_string) : std::string(raw_string) {}
    explicit SSLKeyType(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit SSLKeyType(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit SSLKeyType(const std::string& std_string) : std::string(std_string) {}
    explicit SSLKeyType(const std::string& std_string, size_t position, size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit SSLKeyType(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit SSLKeyType(InputIterator first, InputIterator last)
            : std::string(first, last) {}
};

class SSLKeyPass : public std::string {
  public:
    SSLKeyPass() = default;
    SSLKeyPass(const SSLKeyPass& rhs) = default;
    SSLKeyPass(SSLKeyPass&& rhs) = default;
    SSLKeyPass& operator=(const SSLKeyPass& rhs) = default;
    SSLKeyPass& operator=(SSLKeyPass&& rhs) = default;
    explicit SSLKeyPass(const char* raw_string) : std::string(raw_string) {}
    explicit SSLKeyPass(const char* raw_string, size_t length) : std::string(raw_string, length) {}
    explicit SSLKeyPass(size_t to_fill, char character) : std::string(to_fill, character) {}
    explicit SSLKeyPass(const std::string& std_string) : std::string(std_string) {}
    explicit SSLKeyPass(const std::string& std_string, size_t position, size_t length = std::string::npos)
            : std::string(std_string, position, length) {}
    explicit SSLKeyPass(std::initializer_list<char> il) : std::string(il) {}
    template <class InputIterator>
    explicit SSLKeyPass(InputIterator first, InputIterator last)
            : std::string(first, last) {}
};

} // namespace cpr

#endif
