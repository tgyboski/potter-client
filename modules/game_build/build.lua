local OPCODE_CODE_BUILDING_ITEMS = 10
local currentBuildState = {
  isBuilding = false,
  currentItem = nil,
  allowedPosition = false
}

function init()
  modules.game_build = {
    open = open,
    getBuildState = function() return currentBuildState end
  }
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
    webview:onMessage("build", function(parameters)
      local params = json.decode(parameters)
      local itemId = params.itemId
      print("WEBVIEW BUILD!!")
    end)
  elseif action == "buildItem" then
    startBuilding(data.item)
  end
end

function open()
  getBuildableItems()
  local url = string.format('BuildWindow')
  
  webview = openWebView(url, { success = true, loading = true })
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

function startBuilding(item)
  currentBuildState.isBuilding = true
  currentBuildState.currentItem = item
  currentBuildState.allowedPosition = true -- Por enquanto sempre permitido
  
  -- Atualiza o estado do hover no mapa
  local gameMap = modules.game_interface.getMapPanel()
  if gameMap then
    gameMap:setHoverColor(currentBuildState.allowedPosition and "#00FF00" or "#FF0000")
  end
  
  -- g_game.getProtocolGame():sendExtendedOpcode(10, json.encode({ 
  --   action = "updateBuildState", 
  --   data = currentBuildState 
  -- }))
end

function stopBuilding()
  currentBuildState.isBuilding = false
  currentBuildState.currentItem = nil
  currentBuildState.allowedPosition = false
  
  -- Remove o hover do mapa
  local gameMap = modules.game_interface.getMapPanel()
  if gameMap then
    gameMap:setHoverColor(nil)
  end
  
  g_game.getProtocolGame():sendExtendedOpcode(10, json.encode({ 
    action = "updateBuildState", 
    data = currentBuildState 
  }))
end

-- Função para atualizar o estado do hover baseado na posição do mouse
function updateBuildHover(position)
  if not currentBuildState.isBuilding then return end
  
  -- Por enquanto sempre permitido, mas aqui você pode adicionar suas regras
  currentBuildState.allowedPosition = true
  
  local gameMap = modules.game_interface.getMapPanel()
  if gameMap then
    gameMap:setHoverColor(currentBuildState.allowedPosition and "#00FF00" or "#FF0000")
  end
end 
