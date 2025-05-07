#ifndef WEBVIEW_LUA_H
#define WEBVIEW_LUA_H

#include "framework/luaengine/luaobject.h"

#ifdef _WIN32
// Adicionar includes necessários para Windows
#include <windows.h>
#include <shlobj.h>
#include <knownfolders.h>

// Declaração da classe privada
class WebViewWrapperPrivate;

// Declaração da classe WebViewWrapper
class WebViewWrapper : public LuaObject {
public:
    WebViewWrapper(bool debug);
    ~WebViewWrapper();

    void navigate(const std::string& url);
    void run();
    
    // Método estático para verificar se o WebView deve ser carregado
    static bool shouldLoad() {
        // Verificar se o ambiente suporta WebView
        // Isso evita o instanciamento que pode causar crash
        
        // Armazena o resultado para evitar múltiplas verificações
        static bool checked = false;
        static bool result = false;
        
        if (!checked) {
            checked = true;
            
            // Verificar se o Microsoft Edge WebView2 Runtime está instalado
            try {
                // Apenas verificar se podemos obter a pasta de dados do usuário
                // Isso é um pré-requisito para WebView2
                PWSTR userDataFolder = nullptr;
                HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &userDataFolder);
                if (SUCCEEDED(hr)) {
                    CoTaskMemFree(userDataFolder);
                    result = true;
                }
            } catch(...) {
                // Em caso de erro, não carregar o WebView
                result = false;
            }
        }
        
        return result;
    }

private:
    WebViewWrapperPrivate* m_priv;
};
#endif

void initWebViewLua();

#endif // WEBVIEW_LUA_H 