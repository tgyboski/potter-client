#include "WebView2Panel.h"
#include <wrl.h>
#include <wrl/client.h>
#include <framework/core/logger.h>
#include <framework/luaengine/luainterface.h>
#include <framework/core/application.h>
#include <mutex>
#include <condition_variable>

using namespace Microsoft::WRL;

// Ponteiro global para a instância atual (para uso no WindowProc)
static WebView2Panel* g_currentInstance = nullptr;

WebView2Panel::WebView2Panel(HWND parentHwnd) : UIWidget(), hwnd(nullptr), parentHwnd(parentHwnd) {
    g_logger.info("Iniciando construtor do WebView2Panel");
    
    // Criar uma janela filha para a WebView
    WNDCLASSEXA wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "WebView2PanelClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassExA(&wc);

    // Obter as dimensões da janela pai
    RECT parentRect;
    GetClientRect(parentHwnd, &parentRect);

    hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "WebView2PanelClass",
        "WebView2Panel",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
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

    g_logger.info("Janela da WebView criada com sucesso");

    // Armazenar o ponteiro da instância
    g_currentInstance = this;

    // Inicializar WebView de forma assíncrona
    InitializeWebView([this](bool success) {
        if (success) {
            g_logger.info("WebView2 inicializada com sucesso");
            // Carregar uma página inicial
            loadUrl("https://www.google.com");
        } else {
            g_logger.error("Falha ao inicializar WebView2");
        }
    });
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
        SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(hwnd);
    }
}

void WebView2Panel::setSize(int width, int height) {
    if (hwnd) {
        SetWindowPos(hwnd, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(hwnd);
        resize();
    }
}

void WebView2Panel::resize() {
    if (controller) {
        RECT bounds;
        GetClientRect(hwnd, &bounds);
        controller->put_Bounds(bounds);
        // Garantir que a janela está visível após o redimensionamento
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }
}

void WebView2Panel::loadUrl(const std::string& url) {
    g_logger.info("Tentando carregar URL: " + url);
    if (webview) {
        g_logger.info("WebView está disponível, convertendo URL para wide string");
        std::wstring wideUrl(url.begin(), url.end());
        g_logger.info("Navegando para URL");
        HRESULT hr = webview->Navigate(wideUrl.c_str());
        if (FAILED(hr)) {
            g_logger.error("Falha ao navegar para URL: " + std::to_string(hr));
        } else {
            g_logger.info("Navegação iniciada com sucesso");
        }
    } else {
        g_logger.error("WebView não inicializada");
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
        case WM_CREATE:
            // Armazenar o ponteiro da instância
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(g_currentInstance));
            break;
            
        case WM_SIZE:
            if (panel) {
                panel->handleResize();
            }
            break;
            
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                // Preencher o fundo com branco
                RECT rect;
                GetClientRect(hwnd, &rect);
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));
                FillRect(hdc, &rect, hBrush);
                DeleteObject(hBrush);
                EndPaint(hwnd, &ps);
            }
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WebView2Panel::InitializeWebView(std::function<void(bool)> callback) {
    g_logger.info("Iniciando criação do ambiente WebView2");
    
    // Verificar se o WebView2 Runtime está instalado
    wchar_t* versionInfo = nullptr;
    HRESULT hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &versionInfo);
    if (FAILED(hr)) {
        g_logger.error("WebView2 Runtime não está instalado: " + std::to_string(hr));
        callback(false);
        return;
    }
    std::wstring versionStr(versionInfo);
    std::string versionStrA(versionStr.begin(), versionStr.end());
    g_logger.info("WebView2 Runtime encontrado: " + versionStrA);
    CoTaskMemFree(versionInfo);
    
    // Criar o ambiente WebView2
    hr = CreateCoreWebView2EnvironmentWithOptions(
        nullptr, nullptr, nullptr,
        Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
            [this, callback](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {
                if (FAILED(result)) {
                    g_logger.error("Falha ao criar ambiente WebView2: " + std::to_string(result));
                    callback(false);
                    return result;
                }

                g_logger.info("Ambiente WebView2 criado com sucesso");
                environment = env;
                CreateWebView(callback);
                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        g_logger.error("Falha ao iniciar criação do ambiente WebView2: " + std::to_string(hr));
        callback(false);
    }
}

void WebView2Panel::CreateWebView(std::function<void(bool)> callback) {
    if (!environment) {
        g_logger.error("Ambiente WebView2 não inicializado");
        callback(false);
        return;
    }

    g_logger.info("Criando controlador WebView2");
    HRESULT hr = environment->CreateCoreWebView2Controller(
        hwnd,
        Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
            [this, callback](HRESULT result, ICoreWebView2Controller* ctrl) -> HRESULT {
                if (FAILED(result)) {
                    g_logger.error("Falha ao criar controlador WebView2: " + std::to_string(result));
                    callback(false);
                    return result;
                }

                g_logger.info("Controlador WebView2 criado com sucesso");
                controller = ctrl;
                if (!controller) {
                    g_logger.error("Controlador WebView2 é nulo");
                    callback(false);
                    return E_FAIL;
                }

                HRESULT hr = controller->get_CoreWebView2(&webview);
                if (FAILED(hr) || !webview) {
                    g_logger.error("Falha ao obter WebView2: " + std::to_string(hr));
                    callback(false);
                    return E_FAIL;
                }

                g_logger.info("WebView2 obtida com sucesso");
                
                // Configurar a janela da WebView
                resize();

                // Habilitar a WebView
                controller->put_IsVisible(TRUE);

                // Configurar eventos de navegação
                EventRegistrationToken token;
                webview->add_NavigationStarting(
                    Callback<ICoreWebView2NavigationStartingEventHandler>(
                        [](ICoreWebView2* sender, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT {
                            g_logger.info("Navegação iniciada");
                            return S_OK;
                        }).Get(),
                    &token);

                webview->add_NavigationCompleted(
                    Callback<ICoreWebView2NavigationCompletedEventHandler>(
                        [](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT {
                            BOOL success;
                            args->get_IsSuccess(&success);
                            if (success) {
                                g_logger.info("Navegação concluída com sucesso");
                            } else {
                                g_logger.error("Falha na navegação");
                            }
                            return S_OK;
                        }).Get(),
                    &token);

                g_logger.info("WebView2 inicializada com sucesso");
                callback(true);
                return S_OK;
            }).Get());

    if (FAILED(hr)) {
        g_logger.error("Falha ao iniciar criação do controlador WebView2: " + std::to_string(hr));
        callback(false);
    }
}
