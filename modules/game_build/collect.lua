Collect = {
  opCode = 11,
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
  return g_things.isCollectableItem(itemId)
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

  if action == "try" then
    Collect.onTry(data)
  elseif action == "start" then
    Collect.onStart(data)
  elseif action == "update" then
    Collect.onUpdate(data)
  elseif action == "stop" then
    Collect.onStop()
  end
end

function Collect.onStart(data)
  local tile = g_map.getTile(data.position)
  if not tile then return end
  local item = tile:getTopUseThing()
  if not item or not item:isItem() or item:getId() ~= data.itemId or not g_things.isCollectableItem(item:getId()) then return end

  g_game.collect(item)
end

-- Função de teste para a animação de recursos
function Collect.onTry(data)
  if not data.success then
    return
  end

  local fromPos = data.pos1
  local toPos = data.pos2
  local count = data.count
  local type = data.type

  local tile = g_map.getTile(data.pos1)
  if not tile then return end
  local item = tile:getTopUseThing()
  if not item or not item:isItem() or item:getId() ~= data.itemId or not g_things.isCollectableItem(item:getId()) then return end
  
  -- Anima o item da posição ao lado até o jogador
  for i = 1, count do
    scheduleEvent(function()
      animateResourceToPlayer(fromPos, toPos, type, 1000)
    end, 150 * i)
  end
end

function Collect.onUpdate(data)
  local collecting = data.collecting
  if collecting == 0 then
    Collect.onStop()
  end
end

function Collect.onStop()
  g_game.collect(nil)
end