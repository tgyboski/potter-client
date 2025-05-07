WebView = {}

function init()
    g_logger.info("Módulo WebView: Inicializando...")
    
    -- Verificar se a classe WebViewWrapper está disponível
    if not WebViewWrapper then
        g_logger.warning("Módulo WebView: A classe WebViewWrapper não está disponível. O módulo será carregado, mas não funcionará.")
        return
    end
    
    g_logger.info("Módulo WebView: WebViewWrapper disponível, importando estilo...")
    
    g_ui.importStyle('webview')
    
    connect(g_game, {
        onGameStart = WebView.hide,
        onGameEnd = WebView.show
    })
    
    g_logger.info("Módulo WebView inicializado com sucesso.")
end

function terminate()
    g_logger.info("Módulo WebView: Finalizando...")
    
    disconnect(g_game, {
        onGameStart = WebView.hide,
        onGameEnd = WebView.show
    })
    
    WebView.hide()
    
    g_logger.info("Módulo WebView finalizado.")
end

function WebView.show()
    g_logger.info("WebView.show: Iniciando...")
    
    if WebView.window then
        g_logger.info("WebView já está visível, ignorando.")
        return
    end
    
    -- Verificar se a classe WebViewWrapper está disponível
    if not WebViewWrapper then
        g_logger.warning("WebView.show: A classe WebViewWrapper não está disponível.")
        return
    end
    
    -- Criar a janela da WebView
    WebView.window = g_ui.createWidget('WebViewModal', g_ui.getRootWidget())
    WebView.window:raise()
    WebView.window:focus()
    
    -- Criar instância da WebView
    g_logger.info("WebView.show: Criando instância de WebViewWrapper...")
    
    local success, result = pcall(function()
        -- Tentar criar a instância com tratamento de erro
        WebView.webview = WebViewWrapper.create(true)
        return "success"
    end)
    
    if not success then
        g_logger.error("WebView.show: Erro ao criar WebViewWrapper: " .. tostring(result))
        WebView.hide()
        return
    end
    
    -- Navegar para uma página de teste
    g_logger.info("WebView.show: Navegando para site de teste...")
    
    success, result = pcall(function()
        WebView.webview:navigate("https://google.com")
        return "success"
    end)
    
    if not success then
        g_logger.error("WebView.show: Erro ao navegar: " .. tostring(result))
        WebView.hide()
        return
    end
    
    -- Executar a WebView
    g_logger.info("WebView.show: Iniciando WebView...")
    
    success, result = pcall(function()
        WebView.webview:run()
        return "success"
    end)
    
    if not success then
        g_logger.error("WebView.show: Erro ao executar WebView: " .. tostring(result))
        WebView.hide()
        return
    end
    
    g_logger.info("WebView.show: WebView iniciado com sucesso.")
end

function WebView.hide()
    g_logger.info("WebView.hide: Iniciando...")
    
    -- Destruir a janela
    if WebView.window then
        WebView.window:destroy()
        WebView.window = nil
        g_logger.info("WebView.hide: Janela destruída.")
    end
    
    -- Liberar a instância da WebView
    if WebView.webview then
        WebView.webview = nil
        g_logger.info("WebView.hide: Instância da WebView liberada.")
    end
    
    g_logger.info("WebView.hide: Finalizado.")
end 