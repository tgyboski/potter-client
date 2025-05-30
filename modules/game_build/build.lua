local buildingItems = require('building_items')

function init()
  modules.game_build = {}
  scheduleEvent(open, 1000)
end

function terminate()
  webviewOnGameEnd()
end

local function getBuildableItems()
  return buildingItems.tabs
end

function open()
  local items = getBuildableItems()
  local url = string.format('BuildWindow')
  
  webview = openWebView(url, { items = items })
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
