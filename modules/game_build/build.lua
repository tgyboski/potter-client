local OPCODE_CODE_BUILDING_ITEMS = 10
Webview = {
  Build = {
    currentItem = nil,
    allowedPosition = false
  },
  webview = nil
}

function init()
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

function getWebview()
  if not Webview.webview then
    print("webview not found")
  end
  print("webview found")
  return Webview.webview
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
  
    Webview.webview = openWebView(url, data)
    Webview.webview:show()
    Webview.webview:onMessage("buildItemSelected", function(parameters)
      local params = json.decode(parameters)
      local itemId = params.itemId
      startBuilding(itemId)
    end)
  end
end

function open()
  getBuildableItems()
  local url = string.format('BuildWindow')
  
  Webview.webview = openWebView(url, { success = true, loading = true })
  Webview.webview:show()
end

function buildCurrentItem()
  local item = Webview.Build.currentItem
  if not item then return end
  
  Webview.Build.currentItem = nil
  g_game.build(item)
end

function startBuilding(item)
  print("startBuilding", item)
  Webview.Build.currentItem = item
  Webview.Build.allowedPosition = true -- Por enquanto sempre permitido
end

function stopBuilding()
  Webview.Build.currentItem = nil
  Webview.Build.allowedPosition = false
  
  -- Remove o hover do mapa
  local gameMap = modules.game_interface.getMapPanel()
  if gameMap then
    gameMap:setHoverColor(nil)
  end
  
  g_game.getProtocolGame():sendExtendedOpcode(10, json.encode({ 
    action = "updateBuildState", 
    data = Webview.Build 
  }))
end

function isBuilding()
  if not getWebview() then return false end
  return getWebview():isBuilding()
end

-- Função para atualizar o estado do hover baseado na posição do mouse
function updateBuildHover(position)
  if not isBuilding() then return end
  
  -- Por enquanto sempre permitido, mas aqui você pode adicionar suas regras
  Webview.Build.allowedPosition = true
  
  local gameMap = modules.game_interface.getMapPanel()
  if gameMap then
    gameMap:setHoverColor(Webview.Build.allowedPosition and "#00FF00" or "#FF0000")
  end
end 
