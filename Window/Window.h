#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include "../PointBuffer.h"
#include "../Timer/Timer.h"
#include "pnglib.h"

enum window_state
{
	STATE_CURSOR_INSIDE, STATE_CURSOR_OUTSIDE, STATE_CURSOR_SNAPSHOT
};

class Window
{
public:
	Window(const char* path);
	~Window();

	void Update();

	GLFWwindow* GetGLFWwindow();
	unsigned int GetWidth();
	unsigned int GetHeight();
	PointBuffer* GetPointBuffer();

	//void KeyInput();
	void WindowResize();
	void CursorMovement();

	void TakeSnapshot();

	virtual void key_callback(int key, int scancode, int action, int mods)
	{
		KeyCallback(key, scancode, action, mods);
	}

	void KeyCallback(int key, int scancode, int action, int mods);

private:
	GLFWwindow* m_GLFWwindow;
	unsigned int m_Width;
	unsigned int m_Height;

	FILE* m_Image; // file pointer to the image file currently being edited

	Brush* m_Brush;
	PointBuffer* m_PointBuffer;

	Timer m_Timer;
	float m_fps;
	uint8_t m_State;

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window* win = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		if (win)
			win->key_callback(key, scancode, action, mods);
	}
};