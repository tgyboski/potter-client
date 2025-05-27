#include "WebView2Panel.h"
#include <wrl.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

WebView2Panel::WebView2Panel(HWND parentHwnd) : hwnd(parentHwnd) {
    InitializeWebView();
}

WebView2Panel::~WebView2Panel() {
    webview.Reset();
    controller.Reset();
    environment.Reset();
}

void WebView2Panel::InitializeWebView() {
    // Criar o ambiente WebView2
    HRESULT hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) return result;

                environment = env;
                CreateWebView();
                return S_OK;
            }).Get());
}

void WebView2Panel::CreateWebView() {
    environment->CreateCoreWebView2Controller(
        hwnd,
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this](HRESULT result, ICoreWebView2Controller* ctrl) -> HRESULT {
                if (FAILED(result)) return result;

                controller = ctrl;
                if (!controller) return E_FAIL;

                controller->get_CoreWebView2(&webview);

                if (!webview) return E_FAIL;

                // Configurar a janela da WebView
                RECT bounds;
                GetClientRect(hwnd, &bounds);
                controller->put_Bounds(bounds);

                // Navegar para o Google
                webview->Navigate(L"https://www.google.com");
                return S_OK;
            }).Get());
}
