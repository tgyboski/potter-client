local OPCODE_CODE_BUILDING_ITEMS = 10
Webview = {
  webview = nil
}

function init()
  ProtocolGame.registerExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, onExtendedOpcode)
  
  -- Adiciona o evento de teste da animação
  testResourceAnimationEvent()
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
    getWebview():setWaitingBuildResponse(false)
  elseif action == "itemBuilded" then
    print("itemBuilded", data)
    if data.shift ~= true then
      getWebview():setBuildingState(false, data.itemId)
    else
      getWebview():setBuildingState(true, data.itemId)
    end
    getWebview():setWaitingBuildResponse(false)
  end
end

function open()
  getBuildableItems()
  local url = string.format('BuildWindow')
  
  Webview.webview = openWebView(url, { success = true, loading = true })
  Webview.webview:show()
end

function isBuilding()
  if not getWebview() then return false end
  return getWebview():isBuilding()
end

-- Função de teste para a animação de recursos
function testResourceAnimation()
  local player = g_game.getLocalPlayer()
  if not player then return end
  
  local playerPos = player:getPosition()
  if not playerPos then return end

  print("playerPos", playerPos.x + 1)
  print("playerPos", playerPos.y)
  print("playerPos", playerPos.z)
  -- Pega a posição do SQM ao lado do jogador (direita)
  local fromPos = { x = playerPos.x + 1, y = playerPos.y, z = playerPos.z }
  
  -- Anima o item da posição ao lado até o jogador
  animateResourceToPlayer(fromPos, playerPos, 5901)
end

function testResourceAnimationEvent()
  print("testResourceAnimation")

  scheduleEvent(function()
    testResourceAnimationEvent()
    return true -- retorna true para continuar o evento
  end, 2000) -- 1000ms = 1 segundo

  testResourceAnimation()
end