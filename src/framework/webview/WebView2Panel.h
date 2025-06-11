#pragma once

#include <framework/ui/uiwidget.h>
#include <windows.h>
#include <WebView2.h>
#include <wrl.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <framework/luaengine/luainterface.h>

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
    void InitializeWebView(std::function<void(bool)> callback);

    // Sistema de eventos
    using MessageCallback = std::function<void(const std::string&)>;
    void onMessage(const std::string& eventName, MessageCallback callback);
    void removeMessageListener(const std::string& eventName);

    // Suporte para Lua
    void onLuaMessage(const std::string& eventName, const std::function<void(const std::string&)>& callback);

    // Destruir recursos
    void destroy();

    // Variáveis de controle do build
    bool isBuilding() const { return m_isBuilding; }
    uint16_t getBuildingItemId() const { return m_buildingItemId; }
    void setBuildingState(bool building, uint16_t itemId = 0) {
        m_isBuilding = building;
        m_buildingItemId = itemId;
    }

private:
    HWND hwnd;
    HWND parentHwnd;
    ComPtr<ICoreWebView2> webview;
    ComPtr<ICoreWebView2Environment> environment;
    ComPtr<ICoreWebView2Controller> controller;
    std::function<void()> m_onInitialized;
    bool m_visible{ false };
    std::map<std::string, MessageCallback> m_messageCallbacks;
    bool m_destroyed{ false };

    // Variáveis de controle do build
    bool m_isBuilding = false;
    uint16_t m_buildingItemId = 0;

    void CreateWebView(std::function<void(bool)> callback);
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    void handleResize();
    void handleWebMessage(const std::string& message);
    void registerDefaultCallbacks();
};
