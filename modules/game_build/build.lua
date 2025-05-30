
function init()
  modules.game_build = {}
  scheduleEvent(open, 10000)
end

function terminate()
  webviewOnGameEnd()
end

function open()
  webview = openWebView('BuildWindow')
  webview:show()
  webview:onMessage("build", function()
    print("WEBVIEW BUILD!!")
  end)
end

function buildItem(item)
  -- Por enquanto apenas mostra uma mensagem
  g_ui.createWidget('MessageBox', buildWindow)
    :setText('build ' .. item.name)
    :setTextSize(14)
    :show()
end 
