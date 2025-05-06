-- Internal binary implementation of GUI conforms to this type mapping
export type KeyboardAction = "press" | "release"
export type KeyboardCallback = (key: string, action: KeyboardAction) -> ()

export type GUI = {
    -- Register a callback for keyboard events
    registerKeyboardCallback: (callback: KeyboardCallback) -> boolean,
}

return {} :: GUI 