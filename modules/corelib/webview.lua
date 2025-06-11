-- Variável global para rastrear a WebView atual
local currentWebView = nil
local webviewShowEvent = nil

local function WebViewEncodeAccents(str)
  if str then
    str = string.gsub(str, "á", "\\u00e1")
    str = string.gsub(str, "à", "\\u00e0")
    str = string.gsub(str, "ã", "\\u00e3")
    str = string.gsub(str, "â", "\\u00e2")
    str = string.gsub(str, "é", "\\u00e9")
    str = string.gsub(str, "ê", "\\u00ea")
    str = string.gsub(str, "í", "\\u00ed")
    str = string.gsub(str, "ó", "\\u00f3")
    str = string.gsub(str, "õ", "\\u00f5")
    str = string.gsub(str, "ô", "\\u00f4")
    str = string.gsub(str, "ú", "\\u00fa")
    str = string.gsub(str, "ü", "\\u00fc")
    str = string.gsub(str, "ç", "\\u00e7")
    str = string.gsub(str, "Á", "\\u00c1")
    str = string.gsub(str, "À", "\\u00c0")
    str = string.gsub(str, "Ã", "\\u00c3")
    str = string.gsub(str, "Â", "\\u00c2")
    str = string.gsub(str, "É", "\\u00c9")
    str = string.gsub(str, "Ê", "\\u00ca")
    str = string.gsub(str, "Í", "\\u00cd")
    str = string.gsub(str, "Ó", "\\u00d3")
    str = string.gsub(str, "Õ", "\\u00d5")
    str = string.gsub(str, "Ô", "\\u00d4")
    str = string.gsub(str, "Ú", "\\u00da")
    str = string.gsub(str, "Ü", "\\u00dc")
    str = string.gsub(str, "Ç", "\\u00c7")
  end
  return str
end

local function getUrlBase()
  return "file://" .. g_resources.getWorkDir() .. "/data/webview/start.html#/"
end

disconnect(g_game, { onGameEnd = webviewOnGameEnd })

function openWebView(url, params)
  local fullUrl = getUrlBase() .. url
  
  if params then
    -- Converte a tabela de parâmetros para JSON e codifica apenas os acentos
    local jsonParams = json.encode(params)
    local encodedParams = WebViewEncodeAccents(jsonParams)
    fullUrl = fullUrl .. "?params=" .. encodedParams
  end
  
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