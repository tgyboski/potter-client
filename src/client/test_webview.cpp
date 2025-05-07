#include "webview/webview.h"
#include "framework/otml/otml.h"
#include "framework/core/application.h"
#include "framework/core/eventdispatcher.h"
#include "framework/core/modulemanager.h"
#include "framework/ui/uimanager.h"
#include "framework/ui/uiwidget.h"

int main() {
    // Inicializa a aplicação
    g_app.init();
    
    // Cria a webview
    webview::webview w(true, nullptr);
    w.set_title("WebView Modal");
    w.set_size(800, 600, WEBVIEW_HINT_NONE);
    
    // Cria um modal para conter a webview
    auto modal = g_ui.createWidget("ModalDialog", g_ui.getRootWidget());
    modal->setText("WebView Modal");
    modal->setSize(800, 600);
    
    // Navega para uma página de teste
    w.navigate("https://google.com");
    
    // Executa a webview
    w.run();
    
    return 0;
}
