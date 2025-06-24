local OPCODE_CODE_BUILDING_ITEMS = 10
Webview = {
  webview = nil
}

function init()
  ProtocolGame.registerExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, onExtendedOpcode)
  Collect.init()
end

function terminate()
  ProtocolGame.unregisterExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, onExtendedOpcode)
  Collect.terminate()
end

local function getBuildableItems()
  local player = g_game.getLocalPlayer()
  if not player then
      return false
  end
  
  g_game.getProtocolGame():sendExtendedOpcode(OPCODE_CODE_BUILDING_ITEMS, json.encode({ action = "getBuildingItems" }))
end

function getWebview()
  if not Webview.webview then
    g_logger.error("Webview not found")
  end
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