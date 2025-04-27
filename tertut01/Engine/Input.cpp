#include "pch.h"
#include "Input.h"


Input::Input()
{
}

Input::~Input()
{
}

void Input::Initialise(HWND window)
{
	m_keyboard = std::make_unique<DirectX::Keyboard>();
	m_mouse = std::make_unique<DirectX::Mouse>();
	m_mouse->SetWindow(window);
	m_quitApp = false;

	m_GameInput.forward		= false;
	m_GameInput.back		= false;
	m_GameInput.right		= false;
	m_GameInput.left		= false;
	m_GameInput.rotRight	= false;
	m_GameInput.rotLeft		= false;
	m_GameInput.moveUp		= false;
	m_GameInput.moveDown	= false;
	m_GameInput.startGame	= false;
}

void Input::Update()
{
	auto kb = m_keyboard->GetState();	//updates the basic keyboard state
	m_KeyboardTracker.Update(kb);		//updates the more feature filled state. Press / release etc. 
	auto mouse = m_mouse->GetState();   //updates the basic mouse state
	m_MouseTracker.Update(mouse);		//updates the more advanced mouse state. 

	if (kb.Escape)// check has escape been pressed.  if so, quit out. 
	{
		m_quitApp = true;
	}

	//A key
	if (kb.A)	m_GameInput.left = true;
	else		m_GameInput.left = false;
	
	//D key
	if (kb.D)	m_GameInput.right = true;
	else		m_GameInput.right = false;

	//W key
	if (kb.W)	m_GameInput.forward	 = true;
	else		m_GameInput.forward = false;

	//S key
	if (kb.S)	m_GameInput.back = true;
	else		m_GameInput.back = false;

	// Q key
	if (kb.Q)	m_GameInput.moveUp = true;
	else		m_GameInput.moveUp = false;

	// E key
	if (kb.E)	m_GameInput.moveDown = true;
	else		m_GameInput.moveDown = false;

	//start game
	if (kb.Tab) m_GameInput.startGame = true;
	else		m_GameInput.startGame = false;

	//mouse button
	if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::PRESSED)
	{
		m_GameInput.rightMouseDown = true;
	}
	else if (m_MouseTracker.rightButton == DirectX::Mouse::ButtonStateTracker::RELEASED)
	{
		m_GameInput.rightMouseDown = false;
	}
}

bool Input::Quit()
{
	return m_quitApp;
}

InputCommands Input::getGameInput()
{
	return m_GameInput;
}
