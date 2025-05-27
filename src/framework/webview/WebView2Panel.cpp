#include "WebView2Panel.h"
#include <wrl.h>
#include <wrl/client.h>
#include <framework/core/logger.h>
#include <framework/luaengine/luainterface.h>
#include <framework/core/application.h>
#include <mutex>
#include <condition_variable>
#include <comdef.h>
#include <WebView2.h>

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

    // Armazenar o ponteiro da instância antes de criar a janela
    g_currentInstance = this;

    hwnd = CreateWindowExA(
        WS_EX_CLIENTEDGE,
        "WebView2PanelClass",
        "WebView2Panel",
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, parentRect.right - parentRect.left, parentRect.bottom - parentRect.top,
        parentHwnd,
        NULL,
        GetModuleHandle(NULL),
        this  // Passar o ponteiro this como lParam
    );

    if (!hwnd) {
        g_logger.error("Falha ao criar janela da WebView");
        return;
    }

    g_logger.info("Janela da WebView criada com sucesso");

    // Inicializar WebView de forma assíncrona
    InitializeWebView([this](bool success) {
        if (success) {
            g_logger.info("WebView2 inicializada com sucesso");
            if (m_onInitialized) {
                m_onInitialized();
            }
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

std::string GetErrorMessage(HRESULT hr) {
    _com_error err(hr);
    LPCTSTR errorMessage = err.ErrorMessage();
    if (errorMessage) {
        #ifdef UNICODE
            std::wstring wstr(errorMessage);
            return std::string(wstr.begin(), wstr.end());
        #else
            return std::string(errorMessage);
        #endif
    }
    return "Erro desconhecido";
}

void WebView2Panel::loadUrl(const std::string& url) {
    g_logger.info("Tentando carregar URL: " + url);
    if (webview) {
        g_logger.info("Postando mensagem para thread principal");
        BOOL result = PostMessage(hwnd, WM_APP + 1, 0, (LPARAM)new std::string(url));
        if (!result) {
            DWORD error = GetLastError();
            g_logger.error("Falha ao postar mensagem: " + std::to_string(error));
        } else {
            g_logger.info("Mensagem postada com sucesso");
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
    WebView2Panel* panel = nullptr;

    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        panel = reinterpret_cast<WebView2Panel*>(cs->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(panel));
        g_logger.info("WM_CREATE: Panel armazenado");
    } else {
        panel = reinterpret_cast<WebView2Panel*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (!panel) {
        g_logger.error("Panel não encontrado no WindowProc");
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_SIZE:
            panel->handleResize();
            break;
            
        case WM_APP + 1: // Mensagem personalizada para navegação
            g_logger.info("Mensagem de navegação recebida");
            if (panel->webview) {
                g_logger.info("WebView disponível");
                std::string* url = reinterpret_cast<std::string*>(lParam);
                if (url) {
                    g_logger.info("URL recebida: " + *url);
                    std::wstring wideUrl(url->begin(), url->end());
                    HRESULT hr = panel->webview->Navigate(wideUrl.c_str());
                    if (FAILED(hr)) {
                        g_logger.error("Falha ao navegar para URL: " + GetErrorMessage(hr) + " (HRESULT: " + std::to_string(hr) + ")");
                    } else {
                        g_logger.info("Navegação iniciada com sucesso");
                    }
                    delete url;
                } else {
                    g_logger.error("URL é nula");
                }
            } else {
                g_logger.error("WebView não disponível");
            }
            return 0;
            
        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
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

                webview->add_WebMessageReceived(
                    Microsoft::WRL::Callback<ICoreWebView2WebMessageReceivedEventHandler>(
                        [](ICoreWebView2* sender, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT {
                            LPWSTR rawMessage = nullptr;
                            HRESULT hr = args->get_WebMessageAsJson(&rawMessage);
                            if (SUCCEEDED(hr) && rawMessage) {
                                std::wstring wmsg(rawMessage);
                                std::string msg(wmsg.begin(), wmsg.end());
                                CoTaskMemFree(rawMessage);

                                // Aqui você pode fazer o parsing do JSON ou enviar pro g_logger
                                g_logger.info("[WebView2 JS -> C++] Mensagem recebida: " + msg);
                            } else {
                                g_logger.error("Falha ao obter WebMessage JSON");
                            }
                            return S_OK;
                        }).Get(), nullptr);
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

void WebView2Panel::show() {
    if (!m_visible && hwnd) {
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
        m_visible = true;
    }
}

void WebView2Panel::hide() {
    if (m_visible && hwnd) {
        ShowWindow(hwnd, SW_HIDE);
        UpdateWindow(hwnd);
        m_visible = false;
    }
}
