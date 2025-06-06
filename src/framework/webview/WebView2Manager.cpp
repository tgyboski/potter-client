#include "WebView2Manager.h"
#include <framework/core/application.h>
#include <framework/core/eventdispatcher.h>

void WebView2Manager::initialize(HWND parentHwnd) {
    g_logger.info("WebView2Manager::initialize() CALLED");
    if (m_initialized) return;

    m_webView = std::make_shared<WebView2Panel>(parentHwnd);
    
    // Inicializa a WebView em uma thread separada para não bloquear o cliente
    g_dispatcher.addEvent([this]() {
        m_webView->InitializeWebView([this](bool success) {
            if (success) {
                m_initialized = true;
                g_logger.info("WebView2 inicializada com sucesso");
            } else {
                g_logger.error("Falha ao inicializar WebView2");
            }
        });
    });
} 