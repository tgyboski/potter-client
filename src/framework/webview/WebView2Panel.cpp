#include "WebView2Panel.h"
#include <wrl.h>
#include <wrl/client.h>
#include <framework/core/logger.h>
#include <framework/luaengine/luainterface.h>
#include <framework/core/application.h>

using namespace Microsoft::WRL;

// Ponteiro global para a instância atual (para uso no WindowProc)
static WebView2Panel* g_currentInstance = nullptr;

WebView2Panel::WebView2Panel(HWND parentHwnd) : UIWidget(), hwnd(nullptr), parentHwnd(parentHwnd) {
    // Criar uma janela filha para a WebView
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "WebView2PanelClass";
    RegisterClassExA(&wc);

    // Obter as dimensões da janela pai
    RECT parentRect;
    GetClientRect(parentHwnd, &parentRect);

    hwnd = CreateWindowExA(
        0,
        "WebView2PanelClass",
        "WebView2Panel",
        WS_CHILD | WS_VISIBLE,
        0, 0, parentRect.right - parentRect.left, parentRect.bottom - parentRect.top,
        parentHwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    if (!hwnd) {
        g_logger.error("Falha ao criar janela da WebView");
        return;
    }

    // Armazenar o ponteiro da instância
    g_currentInstance = this;

    InitializeWebView();
}

WebView2Panel::~WebView2Panel() {
    if (g_currentInstance == this) {
        g_currentInstance = nullptr;
    }
    
    if (hwnd) {
        DestroyWindow(hwnd);
    }
    webview.Reset();
    controller.Reset();
    environment.Reset();
}

void WebView2Panel::drawSelf(DrawPoolType drawPane) {
    // Não precisamos desenhar nada aqui, pois a WebView2 se desenha sozinha
}

void WebView2Panel::setPosition(int x, int y) {
    if (hwnd) {
        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
}

void WebView2Panel::setSize(int width, int height) {
    if (hwnd) {
        SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
        resize();
    }
}

void WebView2Panel::resize() {
    if (controller) {
        RECT bounds;
        GetClientRect(hwnd, &bounds);
        controller->put_Bounds(bounds);
    }
}

void WebView2Panel::loadUrl(const std::string& url) {
    if (webview) {
        std::wstring wideUrl(url.begin(), url.end());
        webview->Navigate(wideUrl.c_str());
    }
}

void WebView2Panel::handleResize() {
    if (controller) {
        RECT bounds;
        GetClientRect(hwnd, &bounds);
        controller->put_Bounds(bounds);
    }
}

LRESULT CALLBACK WebView2Panel::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WebView2Panel* panel = reinterpret_cast<WebView2Panel*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_SIZE:
            if (panel) {
                panel->handleResize();
            }
            break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WebView2Panel::InitializeWebView() {
    // Criar o ambiente WebView2
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    g_logger.error("Falha ao criar ambiente WebView2");
                    return result;
                }

                environment = env;
                CreateWebView();
                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        g_logger.error("Falha ao iniciar criação do ambiente WebView2");
    }
}

void WebView2Panel::CreateWebView() {
    if (!environment) {
        g_logger.error("Ambiente WebView2 não inicializado");
        return;
    }

    HRESULT hr = environment->CreateCoreWebView2Controller(
        hwnd,
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Controller* ctrl) -> HRESULT {
                if (FAILED(result)) {
                    g_logger.error("Falha ao criar controlador WebView2");
                    return result;
                }

                controller = ctrl;
                if (!controller) {
                    g_logger.error("Controlador WebView2 é nulo");
                    return E_FAIL;
                }

                HRESULT hr = controller->get_CoreWebView2(&webview);
                if (FAILED(hr) || !webview) {
                    g_logger.error("Falha ao obter WebView2");
                    return E_FAIL;
                }

                // Configurar a janela da WebView
                resize();

                // Habilitar a WebView
                controller->put_IsVisible(TRUE);

                g_logger.info("WebView2 inicializada com sucesso");
                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        g_logger.error("Falha ao iniciar criação do controlador WebView2");
    }
}
