#pragma once

#include <windows.h>
#include <WebView2.h>
#include <wrl.h>
#include <wrl/client.h>
#include <string>
#include <memory>

using namespace Microsoft::WRL;

class WebView2Panel {
private:
    HWND hwnd;
    ComPtr<ICoreWebView2> webview;
    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2Controller> controller;

    void InitializeWebView();
    void CreateWebView();

public:
    WebView2Panel(HWND parentHwnd);
    ~WebView2Panel();

    // Desabilitar cópia
    WebView2Panel(const WebView2Panel&) = delete;
    WebView2Panel& operator=(const WebView2Panel&) = delete;
};
