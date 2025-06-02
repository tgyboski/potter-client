local tabs = {
  {
    id = 1,
    label = "Produção",
    disabled = false,
    items = {
      {
        id = 5260,
        canBuild = true,
        ingredients = {
          { id = 5901, amount = 3, hasAmount = true },
          { id = 953, amount = 3, hasAmount = true }
        }
      },
      {
        id = 5261,
        canBuild = true,
        ingredients = {
          { id = 5901, amount = 4, hasAmount = false },
          { id = 953, amount = 2, hasAmount = true }
        }
      },
      {
        id = 5263,
        canBuild = true,
        ingredients = {
          { id = 5901, amount = 2, hasAmount = true },
          { id = 953, amount = 1, hasAmount = true }
        }
      },
      {
        id = 5264,
        canBuild = true,
        ingredients = {
          { id = 5901, amount = 4, hasAmount = false },
          { id = 953, amount = 2, hasAmount = true }
        }
      }
    }
  },
  {
    label = "Armazenamento",
    disabled = false,
    items = {}
  },
  {
    label = "Comida",
    disabled = false,
    items = {}
  },
  {
    label = "Infraestrutura",
    disabled = false,
    items = {}
  },
  {
    label = "Iluminação",
    disabled = false,
    items = {}
  },
  {
    label = "Fundamentos",
    disabled = false,
    items = {}
  },
  {
    label = "Defesa",
    disabled = false,
    items = {}
  },
  {
    label = "Móveis",
    disabled = false,
    items = {}
  }
}

return {
  tabs = tabs
}
  