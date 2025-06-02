local buildingItems = require('building_items')
local OPCODE_CODE_BUILDING_ITEMS = 10

function init()
  modules.game_build = {
    open = open
  }
  scheduleEvent(open, 1000)
  ProtocolGame.registerExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, onExtendedOpcode)
end

function terminate()
  webviewOnGameEnd()
  ProtocolGame.unregisterExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, onExtendedOpcode)
end

local function getBuildableItems()
  local player = g_game.getLocalPlayer()
  if not player then
      return false
  end
  
  g_game.getProtocolGame():sendExtendedOpcode(10, json.encode({ action = "getBuildingItems" }))
end

function onExtendedOpcode(protocol, opcode, buffer)
  if opcode ~= OPCODE_CODE_BUILDING_ITEMS then return end
  local json_status, json_data =
    pcall(
    function()
      return json.decode(buffer)
    end
  )
  if not json_status then
    g_logger.error("WEBVIEW json error: " .. json_data)
    return false
  end
  local action = json_data["action"]
  local data = json_data["data"]
  

  if not action or not data then
    return false
  end
  
  if action == "getBuildingItems" then
    local url = string.format('BuildWindow')
  
    webview = openWebView(url, data)
    webview:show()
    webview:onMessage("build", function()
      print("WEBVIEW BUILD!!")
    end)
  end
end

function open()
  getBuildableItems()
  local url = string.format('BuildWindow')
  
  webview = openWebView(url, { loading = true })
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
