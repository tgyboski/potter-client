#include "webview_lua.h"
#include "framework/luaengine/luaengine.h"
#include "framework/luaengine/luainterface.h"

#ifdef _WIN32
#include "libs/webview/webview.h"
#include <stdexcept>

// Adicionar flag para permitir depuração
static bool g_debugWebview = true;

// Adicionar função para logar mensagens
void webview_log(const std::string& type, const std::string& message) {
    if (g_debugWebview) {
        std::string logMsg = "[WebView] " + type + ": " + message;
        g_logger.info(logMsg);
        
        // Salvar em arquivo para diagnóstico
        FILE* file = fopen("webview_debug.log", "a");
        if (file) {
            fprintf(file, "%s\n", logMsg.c_str());
            fclose(file);
        }
    }
}

// Definição do membro privado da classe
class WebViewWrapperPrivate {
public:
    webview::webview* m_webview;
};

WebViewWrapper::WebViewWrapper(bool debug) {
    try {
        webview_log("INFO", "WebViewWrapper: Iniciando construtor...");
        
        // Inicializar ponteiro como nulo
        m_priv = nullptr;
        
        try {
            // Criamos o objeto privado
            m_priv = new WebViewWrapperPrivate();
            m_priv->m_webview = nullptr;
            
            webview_log("INFO", "WebViewWrapper: Objeto privado criado");

            // ⚠️ Inicializa o COM antes de criar a WebView
            webview_log("INFO", "WebViewWrapper: Inicializando COM...");
            HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
            if (FAILED(hr)) {
                if (hr == RPC_E_CHANGED_MODE) {
                    webview_log("WARNING", "WebViewWrapper: COM já inicializado com modo diferente. HRESULT: " + std::to_string(hr));
                    // Continuar mesmo com esse aviso
                } else {
                    webview_log("ERROR", "WebViewWrapper: Falha ao inicializar biblioteca COM. HRESULT: " + std::to_string(hr));
                    // Não retornar, continuar a execução para ver se funciona
                }
            } else {
                webview_log("INFO", "WebViewWrapper: COM inicializado com sucesso.");
            }
            
            webview_log("INFO", "WebViewWrapper: Criando instância de webview...");
            
            try {
                // Verificar se Microsoft Edge WebView2 Runtime está instalado
                PWSTR userDataFolder = nullptr;
                HRESULT hrFolder = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &userDataFolder);
                if (SUCCEEDED(hrFolder)) {
                    webview_log("INFO", "WebViewWrapper: Pasta de dados do usuário encontrada.");
                    CoTaskMemFree(userDataFolder);
                } else {
                    webview_log("WARNING", "WebViewWrapper: Não foi possível encontrar a pasta de dados do usuário. HRESULT: " + std::to_string(hrFolder));
                }
                
                webview_log("INFO", "WebViewWrapper: Criando objeto webview com debug=" + std::string(debug ? "true" : "false"));
                m_priv->m_webview = new webview::webview(debug, nullptr);
                webview_log("INFO", "WebViewWrapper: Instância de webview criada com sucesso.");
            } catch (const std::exception& e) {
                webview_log("ERROR", "WebViewWrapper: Exceção ao criar webview: " + std::string(e.what()));
                throw;
            } catch (...) {
                webview_log("ERROR", "WebViewWrapper: Exceção desconhecida ao criar webview");
                throw;
            }
        } catch (const std::exception& e) {
            webview_log("ERROR", "WebViewWrapper: Exceção na inicialização: " + std::string(e.what()));
            
            // Limpar recursos em caso de falha
            if (m_priv) {
                if (m_priv->m_webview) {
                    delete m_priv->m_webview;
                }
                delete m_priv;
                m_priv = nullptr;
            }
            
            CoUninitialize();
            throw;
        } catch (...) {
            webview_log("ERROR", "WebViewWrapper: Exceção desconhecida na inicialização");
            
            // Limpar recursos em caso de falha
            if (m_priv) {
                if (m_priv->m_webview) {
                    delete m_priv->m_webview;
                }
                delete m_priv;
                m_priv = nullptr;
            }
            
            CoUninitialize();
            throw;
        }
    } catch (const std::exception& e) {
        webview_log("ERROR", "WebViewWrapper: Exceção no construtor: " + std::string(e.what()));
    } catch (...) {
        webview_log("ERROR", "WebViewWrapper: Exceção desconhecida no construtor");
    }
}

WebViewWrapper::~WebViewWrapper() {
    try {
        webview_log("INFO", "WebViewWrapper: Destruindo...");
        if (m_priv) {
            if (m_priv->m_webview) {
                webview_log("INFO", "WebViewWrapper: Liberando webview...");
                delete m_priv->m_webview;
            }
            delete m_priv;
        }
        webview_log("INFO", "WebViewWrapper: Finalizando COM...");
        CoUninitialize();
        webview_log("INFO", "WebViewWrapper: Destruído com sucesso.");
    } catch (const std::exception& e) {
        webview_log("ERROR", "WebViewWrapper: Exceção no destrutor: " + std::string(e.what()));
    } catch (...) {
        webview_log("ERROR", "WebViewWrapper: Exceção desconhecida no destrutor");
    }
}

void WebViewWrapper::navigate(const std::string& url) {
    try {
        webview_log("INFO", "WebViewWrapper: Navegando para URL: " + url);
        if (!m_priv) {
            webview_log("ERROR", "WebViewWrapper: Objeto privado é nulo");
            return;
        }
        
        if (!m_priv->m_webview) {
            webview_log("ERROR", "WebViewWrapper: Instância de webview é nula");
            return;
        }
        
        m_priv->m_webview->navigate(url);
        webview_log("INFO", "WebViewWrapper: Navegação iniciada com sucesso");
    } catch (const std::exception& e) {
        webview_log("ERROR", "WebViewWrapper: Exceção ao navegar: " + std::string(e.what()));
    } catch (...) {
        webview_log("ERROR", "WebViewWrapper: Exceção desconhecida ao navegar");
    }
}

void WebViewWrapper::run() {
    try {
        webview_log("INFO", "WebViewWrapper: Iniciando método run()...");
        if (!m_priv) {
            webview_log("ERROR", "WebViewWrapper: Objeto privado é nulo");
            return;
        }
        
        if (!m_priv->m_webview) {
            webview_log("ERROR", "WebViewWrapper: Instância de webview é nula");
            return;
        }
        
        webview_log("INFO", "WebViewWrapper: Chamando webview->run()");
        m_priv->m_webview->run();
        webview_log("INFO", "WebViewWrapper: run() finalizado com sucesso");
    } catch (const std::exception& e) {
        webview_log("ERROR", "WebViewWrapper: Exceção ao executar run(): " + std::string(e.what()));
    } catch (...) {
        webview_log("ERROR", "WebViewWrapper: Exceção desconhecida ao executar run()");
    }
}
#endif

class WebViewLua {
public:
    static void init() {
        try {
            g_logger.info("WebViewLua: Inicializando...");
            // O registro da classe foi movido para luafunctions.cpp
        } catch (const std::exception& e) {
            g_logger.error("WebViewLua init: " + std::string(e.what()));
        } catch (...) {
            g_logger.error("WebViewLua init: Erro desconhecido");
        }
    }
};

void initWebViewLua() {
    try {
        g_logger.info("initWebViewLua: Inicializando módulo WebViewLua...");
        WebViewLua::init();
        g_logger.info("initWebViewLua: Módulo inicializado com sucesso.");
    } catch (const std::exception& e) {
        g_logger.error("initWebViewLua: Exceção ao inicializar: " + std::string(e.what()));
    } catch (...) {
        g_logger.error("initWebViewLua: Exceção desconhecida ao inicializar");
    }
}
