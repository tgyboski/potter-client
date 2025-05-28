buildWindow = nil
buildButton = nil

-- Dados estáticos para teste
local categories = {
  { 
    name = "Buildings", 
    id = 1, 
    isActive = true, 
    button = nil,
    items = {
      { name = "Parede de madeira horizontal", displayId = 1282, category = 1, description = "Parede de madeira", ingredients = "2x Wood", id = 1, url = "file://" .. g_resources.getWorkDir() .. "/modules/game_build/dist/index.html#/", size = 1024 },
      { name = "Parede de madeira vertical", displayId = 1286, category = 1, description = "Parede de madeira", ingredients = "2x Wood", isNew = true, id = 2, url = "file://" .. g_resources.getWorkDir() .. "/modules/game_build/dist/index.html#/teste2", size = 800 },
      { name = "Parede de madeira superior esquerdo", displayId = 1283, category = 1, description = "Parede de madeira", ingredients = "2x Wood", isDisabled = false, id = 3, url = "file://" .. g_resources.getWorkDir() .. "/modules/game_build/dist/index.html#/BuildPage", size = 1500 },
      { name = "Parede de madeira inferior direito", displayId = 1285, category = 1, description = "Parede de madeira", ingredients = "2x Wood", isDisabled = true, id = 4, url = "file://" .. g_resources.getWorkDir() .. "/modules/game_build/dist/index.html#/teste4", size = 1200 }
    }
  },
  { 
    name = "Production", 
    id = 2, 
    button = nil,
    items = {
      { name = "Steel Helmet", displayId = 251, category = 2, description = "A sturdy steel helmet", ingredients = "3x Steel Ingot", id = 5 },
    }
  },
  { 
    name = "Potions", 
    id = 3, 
    isOn = true, 
    button = nil,
    items = {
      { name = "Health Potion", displayId = 252, category = 3, description = "Restores 100 health points", ingredients = "1x Red Mushroom, 1x Water", id = 6 },
      { name = "Bread", displayId = 253, category = 4, description = "Restores 50 hunger points", ingredients = "2x Wheat, 1x Water", id = 7 }
    }
  },
  { 
    name = "Food", 
    id = 4, 
    isDisabled = true, 
    button = nil 
  }
}
local currentCategory = nil -- Category selected
local currentItem = nil -- Item selected

-- Variável global para rastrear a WebView atual
local currentWebView = nil

-- Registra as funções no módulo
-- modules.game_build.open = open
-- modules.game_build.close = close
-- modules.game_build.refresh = refresh

function init()
  -- print("game_build: init() called")

  -- Load styles
  g_ui.importStyle('build')
  g_ui.importStyle('ui/general/buildtabs')
  g_ui.importStyle('ui/general/item')
  
  -- Registra o módulo
  modules.game_build = {}
  open()
  onCategoryClick(1)
  
  -- connect(g_game, { onGameStart = refresh })
  -- connect(g_game, { onGameEnd = close })
end

function terminate()
  disconnect(g_game, { onGameStart = refresh })
  disconnect(g_game, { onGameEnd = close })

  if buildWindow then
    buildWindow:destroy()
  end
end

function refresh()
  if buildWindow then
    buildWindow:destroy()
  end
end

function close()
  if buildWindow then
    buildWindow:destroy()
  end
end

function open()
  if buildWindow then
    buildWindow:destroy()
  end

  buildWindow = g_ui.createWidget('BuildWindow', rootWidget)
  buildWindow:show()
  buildWindow:raise()
  buildWindow:focus()

  setupCategories()
end

function setupCategories()
  local categoriesList = buildWindow:getChildById('buildTabBar')
  categoriesList:destroyChildren()

  for i, category in ipairs(categories) do
    local categoryButton = g_ui.createWidget('BuildTabBarButton', categoriesList)
    categoryButton:setText(category.name)
    categories[i].button = categoryButton
    categoryButton.onClick = function() onCategoryClick(i) end
    if category.isActive then
      categoryButton:setChecked(true)
    end
    if category.isDisabled then
      categoryButton:setEnabled(false)
    end
    if category.isOn then
      categoryButton:setOn(true)
    end
  end
end

function getCategoryById(id)
  for _, category in ipairs(categories) do
    if category.id == id then
      return category
    end
  end
  return nil
end

function _checkCategory(categoryIndex)
  local category = categories[categoryIndex]
  
  if currentCategory then
    if currentCategory.id == category.id then
      return
    end

    currentCategory.button:setChecked(false)
  end

  local categoryButton = categories[categoryIndex].button
  if categoryButton then
    categoryButton:setChecked(true)
  end
  if category.isOn and categoryButton then
    categoryButton:setOn(false)
  end

  local oldCategory = categories[currentCategory]
  if oldCategory then
    oldCategory.button:setChecked(false)
  end

  currentCategory = category
end

function _createItem(itemsList, item)
  local itemBox = g_ui.createWidget('BuildItemBox', itemsList)
  itemBox.data = item
  print(item.id)
  itemBox:setOn(item.isNew)
  itemBox:setEnabled(not item.isDisabled)
  itemBox.onClick = function() onItemClick(itemBox) end
  itemBox.onHoverChange = function() onItemHover(itemBox) end

  local itemWidget = itemBox:getChildById('item')
  itemWidget:setItemId(item.displayId)

  local labelWidget = itemBox:getChildById('label')
  labelWidget:setText(item.name)
end

function onCategoryClick(categoryIndex)
  local category = categories[categoryIndex]
  local categoryId = category.id
  
  _checkCategory(categoryIndex)

  local itemsList = buildWindow:recursiveGetChildById('itemsPanel')
  itemsList:destroyChildren()

  for _, item in ipairs(category.items) do
    _createItem(itemsList, item)
  end
end

function _updateItemIsNew(itemId, categoryId)
  for _, category in ipairs(categories) do
    if category.id == categoryId then
      for _, item in ipairs(category.items) do
        if item.id == itemId then
          item.isNew = false
          return
        end
      end
    end
  end
end

function onItemHover(itemBox)
  itemBox:setOn(false)
  local itemId = itemBox.data.id
  local categoryId = itemBox.data.category
  _updateItemIsNew(itemId, categoryId)
end

function onItemClick(item)
  __openWebView(item.data.url, item.data.size)
  return

  --[[
    print('click')
    print(item:isChecked())
    item:setChecked(true)
    if currentItem then
      print(currentItem.data.id)
      if currentItem.data.id == item.data.id then
        print('same item')
        return
      end
      print('set false')
      currentItem:setChecked(false)
    end
    -- local itemDescription = buildWindow:getChildById('itemDescription')
    -- local craftButton = buildWindow:getChildById('craftButton')
    print(item.data.name)
    local itemName = buildWindow:recursiveGetChildById('itemName')
    itemName:setText(item.data.name)
    -- itemDescription:setText(item.description .. "\nIngredients: " .. item.ingredients)
    -- craftButton:setEnabled(true)
    -- craftButton.onClick = function() craftItem(item) end
    currentItem = item
  ]]--
end

function craftItem(item)
  -- Por enquanto apenas mostra uma mensagem
  g_ui.createWidget('MessageBox', buildWindow)
    :setText('build ' .. item.name)
    :setTextSize(14)
    :show()
end 

function __openWebView(url, size)
  g_logger.info("Abrindo WebView: " .. url)
  
  -- Se já existe uma WebView, apenas atualiza a URL
  if currentWebView then
    -- currentWebView:setSize(size, size)
    currentWebView:loadUrl(url)
    return
  end
  
  -- Cria nova WebView se não existir
  currentWebView = WebView2Panel.create(url)
  currentWebView:onMessage("close", function(parameters)
    -- parameters é uma string JSON com os parâmetros
    g_logger.info("WEBVIEW CLOSE")
    currentWebView:destroy()
    -- Aqui você pode fazer o que quiser com os parâmetros
  end)
  rootWidget:addChild(currentWebView)
end