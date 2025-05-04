#include "pch.h"
#include "Input.h"
#include <Windows.h>

/// Constructor to initialize the Input object.
Input::Input()
{
}

/// Destructor to clean up resources.
Input::~Input()
{
}

/// Initializes the input system with the specified window.
/// @param window The handle to the game window.
void Input::Initialise(HWND window)
{
    m_keyboard = std::make_unique<DirectX::Keyboard>(); // Initialize the keyboard.
    m_mouse = std::make_unique<DirectX::Mouse>();       // Initialize the mouse.
    m_mouse->SetWindow(window);                         // Associate the mouse with the game window.
    m_window = window;                                  // Store the window handle.
    m_quitApp = false;                                  // Initialize the quit flag.

    // Initialize all game input commands to false.
    m_GameInput.forward = false;
    m_GameInput.back = false;
    m_GameInput.right = false;
    m_GameInput.left = false;
    m_GameInput.rotRight = false;
    m_GameInput.rotLeft = false;
    m_GameInput.moveUp = false;
    m_GameInput.moveDown = false;
    m_GameInput.rightMouseDown = false;
    m_GameInput.startGame = false;
}

/// Updates the input state by polling the keyboard and mouse.
void Input::Update()
{
    // Update the keyboard and mouse states.
    auto kb = m_keyboard->GetState(); // Get the current keyboard state.
    m_KeyboardTracker.Update(kb);     // Update the keyboard state tracker.
    auto mouse = m_mouse->GetState(); // Get the current mouse state.
    m_MouseTracker.Update(mouse);     // Update the mouse state tracker.

    // Check if the Escape key is pressed to quit the application.
    if (kb.Escape)
    {
        m_quitApp = true;
    }

    // Update game input commands based on keyboard state.
    m_GameInput.left = kb.A;
    m_GameInput.right = kb.D;
    m_GameInput.forward = kb.W;
    m_GameInput.back = kb.S;
    m_GameInput.moveUp = kb.Q;
    m_GameInput.moveDown = kb.E;
    m_GameInput.startGame = kb.Tab;

    // Update the right mouse button state.
    if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED)
    {
        m_GameInput.rightMouseDown = true;
        LockMouse(); // Lock the mouse when the right button is pressed.
    }
    else if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED)
    {
        m_GameInput.rightMouseDown = false;
        UnlockMouse(); // Unlock the mouse when the right button is released.
    }
}

/// Checks if the application should quit.
/// @return True if the application should quit, false otherwise.
bool Input::Quit()
{
    return m_quitApp;
}

/// Retrieves the current game input commands.
/// @return The current input commands.
InputCommands Input::getGameInput()
{
    return m_GameInput;
}

/// Retrieves the current mouse state.
/// @return The current state of the mouse.
DirectX::Mouse::State Input::getMouseState() const
{
    return m_mouse->GetState();
}

/// Retrieves the mouse movement delta since the last frame.
/// @return A 2D vector representing the mouse movement delta.
DirectX::SimpleMath::Vector2 Input::getMouseDelta() const
{
    auto current = m_mouse->GetState(); // Get the current mouse state.
    DirectX::SimpleMath::Vector2 delta;

    // Calculate the difference between the current and previous mouse positions.
    delta.x = static_cast<float>(current.x - m_previousMouseState.x);
    delta.y = static_cast<float>(current.y - m_previousMouseState.y);

    return delta;
}

/// Updates the previous mouse state for calculating deltas.
void Input::UpdatePreviousMouseState()
{
    m_previousMouseState = m_mouse->GetState();
}

/// Locks the mouse cursor within the game window and hides it.
void Input::LockMouse()
{
    RECT rect;
    GetClientRect(m_window, &rect); // Get the client area of the window.
    POINT ul = { rect.left, rect.top };
    POINT lr = { rect.right, rect.bottom };

    ClientToScreen(m_window, &ul); // Convert the upper-left corner to screen coordinates.
    ClientToScreen(m_window, &lr); // Convert the lower-right corner to screen coordinates.

    RECT clipRect;
    clipRect.left = ul.x;
    clipRect.top = ul.y;
    clipRect.right = lr.x;
    clipRect.bottom = lr.y;

    ClipCursor(&clipRect); // Lock the cursor inside the window.
    ShowCursor(false);     // Hide the cursor.
}

/// Unlocks the mouse cursor and makes it visible.
void Input::UnlockMouse()
{
    ClipCursor(nullptr); // Unlock the cursor.
    ShowCursor(true);    // Show the cursor.
}