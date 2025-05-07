#ifndef WEBVIEW_LUA_H
#define WEBVIEW_LUA_H

#include "framework/luaengine/luaobject.h"

#ifdef _WIN32
// Declaração da classe privada
class WebViewWrapperPrivate;

// Declaração da classe WebViewWrapper
class WebViewWrapper : public LuaObject {
public:
    WebViewWrapper(bool debug);
    ~WebViewWrapper();

    void navigate(const std::string& url);
    void run();

private:
    WebViewWrapperPrivate* m_priv;
};
#endif

void initWebViewLua();

#endif // WEBVIEW_LUA_H 