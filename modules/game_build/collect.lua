Collect = {
  opCode = 11,
}

-- IDs dos itens que podem ser coletados
Collect.CollectableItems = {
    3617, 3619, 3614, 3621, 3616, 3634, 3630, 3629, 1852, 6189, 1859, 1860, 1874, 1857, 1873, 1872, 3632
}

function init()
    connect(g_game, { onGameStart = Collect.onGameStart })
    connect(g_game, { onGameEnd = Collect.onGameEnd })
end

function terminate()
    disconnect(g_game, { onGameStart = Collect.onGameStart })
    disconnect(g_game, { onGameEnd = Collect.onGameEnd })
end

function Collect.onGameStart()
  ProtocolGame.registerExtendedOpcode(Collect.opCode, Collect._onExtendedOpcode)
end

function Collect.onGameEnd()
  ProtocolGame.unregisterExtendedOpcode(Collect.opCode, Collect._onExtendedOpcode)
end

function Collect.isCollectableItem(itemId)
  for _, id in ipairs(Collect.CollectableItems) do
    if itemId == id then
      return true
    end
  end
  return false
end 

function Collect._onExtendedOpcode(protocol, opcode, buffer)
  if tonumber(opcode) ~= tonumber(Collect.opCode) then return end
  local json_status, json_data =
  pcall(
    function()
      return json.decode(buffer)
    end
  )
  if not json_status then
    g_logger.error("Collect json error: " .. json_data)
    return false
  end
  local action = json_data["action"]
  local data = json_data["data"]

  if action == "tryCollectResource" then
    Collect.onTryCollectResource(data)
  elseif action == "startCollectResource" then
    Collect.onStartCollectResource(data)
  elseif action == "collectingUpdate" then
    Collect.onCollectingUpdate(data)
  end
end

function Collect.onStartCollectResource(data)
  local tile = g_map.getTile(data.position)
  if not tile then return end
  local item = tile:getTopUseThing()
  if not item or not item:isItem()  or item:getId() ~= data.itemId then return end

  g_game.collect(item)
end

-- Função de teste para a animação de recursos
function Collect.onTryCollectResource(data)
  if not data.success then
    return
  end

  local fromPos = data.pos1
  local toPos = data.pos2
  local count = data.count
  local type = data.type
  
  -- Anima o item da posição ao lado até o jogador
  for i = 1, count do
    scheduleEvent(function()
      animateResourceToPlayer(fromPos, toPos, type, 1000)
    end, 150 * i)
  end
end

function Collect.onCollectingUpdate(data)
  local collecting = data.collecting
  print("onCollectingUpdate", collecting)
  if collecting == 0 then
    print("onStopCollecting")
    Collect.onStopCollecting()
  end
end

function Collect.onStopCollecting()
  g_game.collect(nil)
end