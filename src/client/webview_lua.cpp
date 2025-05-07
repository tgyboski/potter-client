#include "webview_lua.h"
#include "framework/luaengine/luaengine.h"
#include "framework/luaengine/luainterface.h"

#ifdef _WIN32
#include "webview/webview.h"

// Definição do membro privado da classe
class WebViewWrapperPrivate {
public:
    webview::webview* m_webview;
};

WebViewWrapper::WebViewWrapper(bool debug) {
    // Criamos o objeto privado
    m_priv = new WebViewWrapperPrivate();
    
    // ⚠️ Inicializa o COM antes de criar a WebView
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        g_logger.error("Failed to initialize COM library.");
        return;
    }

    m_priv->m_webview = new webview::webview(debug, nullptr);
}

WebViewWrapper::~WebViewWrapper() {
    if (m_priv) {
        delete m_priv->m_webview;
        delete m_priv;
    }
    CoUninitialize();  // opcional (depende de como você gerencia o ciclo de vida)
}

void WebViewWrapper::navigate(const std::string& url) {
    if (m_priv && m_priv->m_webview) {
        m_priv->m_webview->navigate(url);
    }
}

void WebViewWrapper::run() {
    if (m_priv && m_priv->m_webview) {
        m_priv->m_webview->run();
    }
}
#endif

class WebViewLua {
public:
    static void init() {
        // O registro da classe foi movido para luafunctions.cpp
    }
};

void initWebViewLua() {
    WebViewLua::init();
}
