-- Main game script
local engine = require("engine")

-- Set up the game
function init()
    -- Set clear color to dark blue
    engine.setClearColor(0.1, 0.1, 0.3, 1.0)
    print("Game initialized!")
end

-- Update game state
function update(deltaTime)
    -- Add game logic here
end

-- Render the game
function render()
    -- Add rendering code here
end

-- Main game loop
function main()
    init()
    
    while engine.isRunning() do
        update(engine.getDeltaTime())
        render()
        engine.present()
    end
end

-- Start the game
main() 