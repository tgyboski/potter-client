#include "webview/webview.h"
#include "framework/luaengine/luaengine.h"
#include "framework/luaengine/luainterface.h"

class WebViewWrapper : public LuaObject {
public:
    WebViewWrapper(bool debug, void* owner) : m_webview(new webview::webview(debug, owner)) {}
    ~WebViewWrapper() { delete m_webview; }

    void navigate(const std::string& url) {
        if (m_webview) {
            m_webview->navigate(url);
        }
    }

    void run() {
        if (m_webview) {
            m_webview->run();
        }
    }

private:
    webview::webview* m_webview;
};

class WebViewLua {
public:
    static void init() {
        g_lua.registerClass("WebView", "WebView");
        g_lua.bindClassStaticFunction("WebView", "create", [](bool debug, void* owner) {
            auto wrapper = std::make_shared<WebViewWrapper>(debug, owner);
            g_lua.pushObject(wrapper);
        });
        g_lua.bindClassStaticFunction("WebView", "destroy", [](WebViewWrapper* wrapper) {
            if (wrapper) {
                delete wrapper;
            }
        });
        g_lua.bindClassStaticFunction("WebView", "navigate", [](WebViewWrapper* wrapper, const std::string& url) {
            if (wrapper) {
                wrapper->navigate(url);
            }
        });
        g_lua.bindClassStaticFunction("WebView", "run", [](WebViewWrapper* wrapper) {
            if (wrapper) {
                wrapper->run();
            }
        });
    }
};

void initWebViewLua() {
    WebViewLua::init();
} 