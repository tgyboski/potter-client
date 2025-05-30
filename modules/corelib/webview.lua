-- Variável global para rastrear a WebView atual
local currentWebView = nil
local webviewShowEvent = nil

local function getUrlBase()
  return "file://" .. g_resources.getWorkDir() .. "/modules/game_build/dist/index.html#/"
end

disconnect(g_game, { onGameEnd = webviewOnGameEnd })

function openWebView(url)
  local fullUrl = getUrlBase() .. url
  g_logger.info("Abrindo WebView: " .. fullUrl)
  
  -- Se já existe uma WebView, apenas atualiza a URL
  if currentWebView then
    currentWebView:loadUrl(fullUrl)

    if webviewShowEvent then
      removeEvent(webviewShowEvent)
      webviewShowEvent = nil
    end
    webviewShowEvent = scheduleEvent(function()
      currentWebView:show()
    end, 100)
    
    return currentWebView
  end
  
  -- Cria nova WebView se não existir
  currentWebView = WebView2Panel.create(fullUrl)
  
  -- Registra o callback mantendo a referência
  currentWebView:onMessage("close", webviewCloseCallback)
  rootWidget:addChild(currentWebView)
  return currentWebView
end

webviewCloseCallback = function(parameters)
  g_logger.info("WEBVIEW CLOSE")
  currentWebView:hide()
end

function webviewOnGameEnd()
  g_logger.info("WEBVIEW ON GAME END")
  if currentWebView then
    currentWebView:hide()
  end
end