#include "GUI.h"
#import <Cocoa/Cocoa.h>
#include <iostream>

// Initialize global instance pointer
static GUI* g_gui = nullptr;

// Objective-C++ delegate for window events
@interface MacWindowDelegate : NSObject <NSWindowDelegate>
@property (nonatomic, assign) void* userData;
@property (nonatomic, assign) void (*keyCallback)(const char* key, const char* action, void* userData);
@property (nonatomic, assign) void (*closeCallback)(void* userData);
@end

@implementation MacWindowDelegate
- (void)windowWillClose:(NSNotification *)notification {
    if (self.closeCallback) {
        self.closeCallback(self.userData);
    }
}
- (void)keyDown:(NSEvent *)event {
    if (self.keyCallback) {
        NSString* chars = [event charactersIgnoringModifiers];
        if ([chars length] > 0) {
            self.keyCallback([chars UTF8String], "press", self.userData);
        }
    }
}
- (void)keyUp:(NSEvent *)event {
    if (self.keyCallback) {
        NSString* chars = [event charactersIgnoringModifiers];
        if ([chars length] > 0) {
            self.keyCallback([chars UTF8String], "release", self.userData);
        }
    }
}
@end

// C callbacks for delegate
static void MacKeyCallback(const char* key, const char* action, void* userData) {
    if (userData) {
        static_cast<GUI*>(userData)->handleKeyEvent(key, action);
    }
}
static void MacCloseCallback(void* userData) {
    if (userData) {
        static_cast<GUI*>(userData)->onWindowClosed();
    }
}

GUI::GUI(LuauBinding* luauBinding)
    : luauBinding(luauBinding), width(800), height(600), windowOpen(false), window(nullptr), delegate(nullptr) {
    g_gui = this;
    std::cout << "[Mac] GUI constructed" << std::endl;
}

GUI::~GUI() {
    g_gui = nullptr;
    if (window) {
        NSWindow* win = (NSWindow*)window;
        [win orderOut:nil];
        win.delegate = nil;
        [win close];
        [win release];
        window = nullptr;
    }
    if (delegate) {
        [(NSObject*)delegate release];
        delegate = nullptr;
    }
    std::cout << "[Mac] GUI destroyed" << std::endl;
}

bool GUI::initialize(const std::string& windowTitle, int width, int height) {
    this->width = width;
    this->height = height;
    // Ensure NSApp is initialized
    if ([NSApp class] == nil) {
        [NSApplication sharedApplication];
    }
    
    // Make sure the application is properly activated to receive events
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps:YES];
    
    NSRect rect = NSMakeRect(0, 0, width, height);
    NSUInteger style = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable;
    NSWindow* win = [[NSWindow alloc] initWithContentRect:rect
                                                styleMask:style
                                                  backing:NSBackingStoreBuffered
                                                    defer:NO];
    [win setTitle:[NSString stringWithUTF8String:windowTitle.c_str()]];
    [win makeKeyAndOrderFront:nil];
    [win center];
    MacWindowDelegate* del = [[MacWindowDelegate alloc] init];
    del.userData = this;
    del.keyCallback = MacKeyCallback;
    del.closeCallback = MacCloseCallback;
    win.delegate = del;
    [win setAcceptsMouseMovedEvents:YES];
    [win setReleasedWhenClosed:NO];
    window = win;
    delegate = del;
    windowOpen = true;
    std::cout << "[Mac] GUI initialized with window: " << windowTitle 
              << " (" << width << "x" << height << ")" << std::endl;
    return true;
}

bool GUI::isWindowOpen() const {
    return windowOpen && window && [(NSWindow*)window isVisible];
}

void GUI::pumpMessages() {
    @autoreleasepool {
        // Process pending events in the queue
        NSEvent* event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES])) {
            if (event.type == NSEventTypeKeyDown) {
                [(MacWindowDelegate*)delegate keyDown:event];
            } else if (event.type == NSEventTypeKeyUp) {
                [(MacWindowDelegate*)delegate keyUp:event];
            } else {
                [NSApp sendEvent:event];
            }
        }
        
        // Allow NSApp to update the window state and run other app logic
        [NSApp updateWindows];
    }
}

void GUI::onWindowClosed() {
    windowOpen = false;
    std::cout << "[Mac] Window closed" << std::endl;
}

GUI* GUI::getInstance(lua_State* L) {
    if (!g_gui) {
        luaL_error(L, "GUI instance not initialized");
        return nullptr;
    }
    return g_gui;
}

// Register a keyboard callback from Lua
static int registerKeyboardCallback(lua_State* L) {
    GUI* instance = GUI::getInstance(L);
    if (!instance) return 0;

    // Check if we have a function as the first argument
    if (!lua_isfunction(L, 1)) {
        luaL_error(L, "Expected function as first argument");
        return 0;
    }

    // Store the callback function in the registry
    lua_pushvalue(L, 1);  // Push the function to the top
    int ref = lua_ref(L, -1);  // Reference the value at the top of the stack
    lua_pop(L, 1);  // Pop the function from the stack

    if (ref == LUA_NOREF) {
        luaL_error(L, "Failed to create reference to callback function");
        return 0;
    }

    // Store the reference
    instance->registerKeyboardCallback(ref);

    lua_pushboolean(L, 1);
    return 1;
}

void GUI::registerKeyboardCallback(int callbackRef) {
    keyboardCallbacks.push_back(callbackRef);
    std::cout << "[Mac] Registered keyboard callback: " << callbackRef << std::endl;
}

void GUI::handleKeyEvent(const std::string& key, const std::string& action) {
    lua_State* L = luauBinding->getLuaState();
    if (!L) return;

    // Call each registered callback
    for (int ref : keyboardCallbacks) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        lua_pushstring(L, key.c_str());
        lua_pushstring(L, action.c_str());
        
        if (lua_pcall(L, 2, 0, 0) != 0) {
            std::cerr << "Error in keyboard callback: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
        }
    }
}

// Define exports array
static LuauExport GuiExports[] = {
    {"registerKeyboardCallback", registerKeyboardCallback},
    {nullptr, nullptr}
};

const LuauExport* GUI::getExports() const {
    return GuiExports;
}

WindowInfo GUI::getWindowInfo() const {
    WindowInfo info;
    info.handle = window;
    info.context = nullptr;
    info.width = width;
    info.height = height;
    return info;
} 