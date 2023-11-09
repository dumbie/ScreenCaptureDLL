#pragma once
#include "CaptureIncludes.h"
#include "CaptureVariables.h"

namespace
{
	LRESULT WindowMessageProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_DISPLAYCHANGE)
		{
			Sleep(500);
			vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(true);
			vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(false);
		}
		else if (uMsg == WM_SYSCOMMAND)
		{
			Sleep(500);
			vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(true);
			vWgcInstance.vGraphicsCaptureSession.IsBorderRequired(false);
		}
		return DefWindowProcA(hWnd, uMsg, wParam, lParam);
	}

	BOOL WindowCreateHiddenMessageCapture()
	{
		//Set window information
		auto windowTitle = "ScreenCaptureMessageWindow";
		auto windowClassName = "ScreenCaptureMessageWindow";

		//Set window class
		WNDCLASSEX windowClass{};
		windowClass.cbSize = sizeof(windowClass);
		windowClass.lpfnWndProc = WindowMessageProcedure;
		windowClass.lpszClassName = windowClassName;

		//Register window class
		if (!RegisterClassExA(&windowClass))
		{
			std::cout << "Failed to register window class." << std::endl;
			return false;
		}

		//Create window
		HWND hWnd = CreateWindowExA(0, windowClassName, windowTitle, WS_OVERLAPPED, 0, 0, 0, 0, HWND_DESKTOP, NULL, NULL, NULL);
		if (hWnd)
		{
			std::cout << "Message window created: " << hWnd << std::endl;
			return true;
		}
		else
		{
			std::cout << "Message window creation failed." << std::endl;
			return false;
		}

		//Update window
		UpdateWindow(hWnd);
	}
}