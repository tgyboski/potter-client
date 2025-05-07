-- WebView = {}

-- function init()
--     g_ui.importStyle('webview')
    
--     connect(g_game, {
--         onGameStart = WebView.hide,
--         onGameEnd = WebView.show
--     })
    
--     -- Inicializa a webview
--     WebView.webview = WebView.create(true, nil)
--     WebView.webview:set_title("WebView Modal")
--     WebView.webview:set_size(800, 600, WEBVIEW_HINT_NONE)
-- end

-- function terminate()
--     disconnect(g_game, {
--         onGameStart = WebView.hide,
--         onGameEnd = WebView.show
--     })
    
--     if WebView.window then
--         WebView.window:destroy()
--         WebView.window = nil
--     end
    
--     if WebView.webview then
--         WebView.webview:destroy()
--         WebView.webview = nil
--     end
-- end

-- function WebView.show()
--     if WebView.window then
--         return
--     end
    
--     WebView.window = g_ui.createWidget('WebViewModal', g_ui.getRootWidget())
--     WebView.window:raise()
--     WebView.window:focus()
    
--     -- Navega para uma página de teste
--     if WebView.webview then
--         WebView.webview:navigate("https://google.com")
--         WebView.webview:run()
--     end
-- end

-- function WebView.hide()
--     if WebView.window then
--         WebView.window:destroy()
--         WebView.window = nil
--     end
    
--     if WebView.webview then
--         WebView.webview:destroy()
--         WebView.webview = nil
--     end
-- end 