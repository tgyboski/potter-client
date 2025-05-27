#pragma once

#include "WebView2Panel.h"
#include <memory>

class WebView2Manager {
public:
    static WebView2Manager& getInstance() {
        static WebView2Manager instance;
        return instance;
    }

    void initialize(HWND parentHwnd);
    std::shared_ptr<WebView2Panel> getWebView() { return m_webView; }
    bool isInitialized() const { return m_initialized; }

private:
    WebView2Manager() = default;
    ~WebView2Manager() = default;
    WebView2Manager(const WebView2Manager&) = delete;
    WebView2Manager& operator=(const WebView2Manager&) = delete;

    std::shared_ptr<WebView2Panel> m_webView;
    bool m_initialized{ false };
}; 