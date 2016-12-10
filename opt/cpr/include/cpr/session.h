#ifndef CPR_SESSION_H
#define CPR_SESSION_H

#include <memory>

#include "auth.h"
#include "body.h"
#include "cookies.h"
#include "cprtypes.h"
#include "digest.h"
#include "multipart.h"
#include "parameters.h"
#include "payload.h"
#include "proxies.h"
#include "response.h"
#include "timeout.h"
#include "ssl.h"

namespace cpr {

class Session {
  public:
    Session();
    ~Session();

    void SetUrl(const Url& url);
    void SetParameters(const Parameters& parameters);
    void SetParameters(Parameters&& parameters);
    void SetHeader(const Header& header);
    void SetTimeout(const Timeout& timeout);
    void SetAuth(const Authentication& auth);
    void SetDigest(const Digest& auth);
    void SetPayload(Payload&& payload);
    void SetPayload(const Payload& payload);
    void SetProxies(Proxies&& proxies);
    void SetProxies(const Proxies& proxies);
    void SetMultipart(Multipart&& multipart);
    void SetMultipart(const Multipart& multipart);
    void SetRedirect(const bool& redirect);
    void SetMaxRedirects(const long& max_redirects);
    void SetCookies(const Cookies& cookies);
    void SetBody(Body&& body);
    void SetBody(const Body& body);
    void SetCert(SSLCert&& cert);
    void SetCert(const SSLCert& cert);
    void SetCertType(SSLCertType&& certType);
    void SetCertType(const SSLCertType& certType);
    void SetKey(SSLKey&& key);
    void SetKey(const SSLKey& key);
    void SetKeyType(SSLKeyType&& keyType);
    void SetKeyType(const SSLKeyType& keyType);
    void SetKeyPass(SSLKeyPass&& keyPass);
    void SetKeyPass(const SSLKeyPass& keyPass);

    // Used in templated functions
    void SetOption(const Url& url);
    void SetOption(const Parameters& parameters);
    void SetOption(Parameters&& parameters);
    void SetOption(const Header& header);
    void SetOption(const Timeout& timeout);
    void SetOption(const Authentication& auth);
    void SetOption(const Digest& auth);
    void SetOption(Payload&& payload);
    void SetOption(const Payload& payload);
    void SetOption(Proxies&& proxies);
    void SetOption(const Proxies& proxies);
    void SetOption(Multipart&& multipart);
    void SetOption(const Multipart& multipart);
    void SetOption(const bool& redirect);
    void SetOption(const long& max_redirects);
    void SetOption(const Cookies& cookies);
    void SetOption(Body&& body);
    void SetOption(const Body& body);
    void SetOption(SSLCert&& cert);
    void SetOption(const SSLCert& cert);
    void SetOption(SSLCertType&& certType);
    void SetOption(const SSLCertType& certType);
    void SetOption(SSLKey&& key);
    void SetOption(const SSLKey& key);
    void SetOption(SSLKeyType&& keyType);
    void SetOption(const SSLKeyType& keyType);
    void SetOption(SSLKeyPass&& keyPass);
    void SetOption(const SSLKeyPass& keyPass);

    Response Delete();
    Response Get();
    Response Head();
    Response Options();
    Response Patch();
    Response Post();
    Response Put();

  private:
    class Impl;
    std::unique_ptr<Impl> pimpl_;
};

} // namespace cpr

#endif
