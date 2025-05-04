#pragma once

/// Represents the input commands for the game.
/// This struct abstracts the hardware input into logical game commands.
/// It allows easy mapping of hardware inputs to game actions, making it easier
/// to adapt to different input devices or configurations.
struct InputCommands
{
    bool forward;          ///< Move forward command (e.g., W key).
    bool back;             ///< Move backward command (e.g., S key).
    bool right;            ///< Move right command (e.g., D key).
    bool left;             ///< Move left command (e.g., A key).
    bool rotRight;         ///< Rotate right command (not used in this implementation).
    bool rotLeft;          ///< Rotate left command (not used in this implementation).
    bool moveUp;           ///< Move up command (e.g., Q key).
    bool moveDown;         ///< Move down command (e.g., E key).
    bool rightMouseDown;   ///< Right mouse button is pressed.
    bool startGame;        ///< Start game command (e.g., Tab key).
};

/// Handles input from the keyboard and mouse.
/// This class abstracts hardware input and provides a unified interface for the game to process input commands.
/// It uses DirectXTK's `Keyboard` and `Mouse` classes to manage input states.
class Input
{
public:
    /// Constructor to initialize the Input object.
    Input();

    /// Destructor to clean up resources.
    ~Input();

    /// Initializes the input system with the specified window.
    /// @param window The handle to the game window.
    void Initialise(HWND window);

    /// Updates the input state by polling the keyboard and mouse.
    void Update();

    /// Checks if the application should quit.
    /// @return True if the application should quit, false otherwise.
    bool Quit();

    /// Retrieves the current game input commands.
    /// @return The current input commands.
    InputCommands getGameInput();

    /// Retrieves the current mouse state.
    /// @return The current state of the mouse.
    DirectX::Mouse::State getMouseState() const;

    /// Retrieves the mouse movement delta since the last frame.
    /// @return A 2D vector representing the mouse movement delta.
    DirectX::SimpleMath::Vector2 getMouseDelta() const;

    /// Updates the previous mouse state for calculating deltas.
    void UpdatePreviousMouseState();

    /// Locks the mouse cursor within the game window and hides it.
    void LockMouse();

    /// Unlocks the mouse cursor and makes it visible.
    void UnlockMouse();

private:
    bool m_quitApp; ///< Indicates whether the application should quit.
    std::unique_ptr<DirectX::Keyboard> m_keyboard; ///< Manages keyboard input.
    std::unique_ptr<DirectX::Mouse> m_mouse; ///< Manages mouse input.
    DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker; ///< Tracks keyboard state changes.
    DirectX::Mouse::ButtonStateTracker m_MouseTracker; ///< Tracks mouse button state changes.
    InputCommands m_GameInput; ///< Stores the current game input commands.
    DirectX::Mouse::State m_previousMouseState; ///< Stores the previous mouse state for delta calculations.
    HWND m_window; ///< Handle to the game window.
};