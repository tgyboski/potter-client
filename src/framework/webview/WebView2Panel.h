#pragma once

#include <framework/ui/uiwidget.h>
#include <windows.h>
#include <WebView2.h>
#include <wrl.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <functional>

using namespace Microsoft::WRL;

class WebView2Panel : public UIWidget {
public:
    WebView2Panel(HWND parentHwnd);
    ~WebView2Panel();

    // Desabilitar cópia
    WebView2Panel(const WebView2Panel&) = delete;
    WebView2Panel& operator=(const WebView2Panel&) = delete;

    void setPosition(int x, int y);
    void setSize(int width, int height);
    void loadUrl(const std::string& url);
    void resize();
    HWND getHwnd() const { return hwnd; }
    void setOnInitialized(std::function<void()> callback) { m_onInitialized = callback; }

    // Funções herdadas de UIWidget
    void drawSelf(DrawPoolType drawPane) override;

    // Novos métodos para controle de visibilidade
    void show();
    void hide();
    bool isVisible() const { return m_visible; }

private:
    HWND hwnd;
    HWND parentHwnd;
    ComPtr<ICoreWebView2> webview;
    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2Controller> controller;
    std::function<void()> m_onInitialized;
    bool m_visible{ false };

    void InitializeWebView(std::function<void(bool)> callback);
    void CreateWebView(std::function<void(bool)> callback);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void handleResize();
};
