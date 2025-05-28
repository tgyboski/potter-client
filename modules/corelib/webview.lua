-- Variável global para rastrear a WebView atual
local currentWebView = nil
local webviewShowEvent = nil

function openWebView(url)
  g_logger.info("Abrindo WebView: " .. url)
  
  -- Se já existe uma WebView, apenas atualiza a URL
  if currentWebView then
    currentWebView:loadUrl(url)

    if webviewShowEvent then
      removeEvent(webviewShowEvent)
      webviewShowEvent = nil
    end
    webviewShowEvent = scheduleEvent(function()
      currentWebView:show()
    end, 100)
    
    return
  end
  
  -- Cria nova WebView se não existir
  currentWebView = WebView2Panel.create(url)

  
  -- Registra o callback mantendo a referência
  currentWebView:onMessage("close", closeCallback)
  rootWidget:addChild(currentWebView)
end

closeCallback = function(parameters)
  g_logger.info("WEBVIEW CLOSE")
  currentWebView:hide()
end