//---------------------------------------------------------------------------
//Tools
#include <CodeTools.h>
//Project
#include "Renderer.h"
#include "Window.h"
#include "Controller.h"
#include "GlobalDebug.h"
//STD
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <atlbase.h>
#include <atlwin.h>
#endif
//---------------------------------------------------------------------------
#pragma warning(disable : 4996)
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	WINDOW::WINDOW():hDC(NULL){
		bQuit = false;
	}
	//---------------------------------------------------------------------------
	void WINDOW::SetNames(const char* Name, const char* Title){
		strcpy((char*)szWdwClassName, (char*)Name);
		strcpy((char*)szWdwTitle, (char*)Title);
	}
	//---------------------------------------------------------------------------
	void WINDOW::AttachEngines(RENDERER& _R, CONTROLLER& _C){
		pRenderer = &_R;
		pController = &_C;
	}
	//---------------------------------------------------------------------------
	#ifdef _WIN32
	//---------------------------------------------------------------------------
	LRESULT CALLBACK WINDOW::sWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		WINDOW* pWindow=NULL;
		if (message == WM_NCCREATE){
			pWindow = (WINDOW*) ((LPCREATESTRUCT) lParam)->lpCreateParams ;
			SetWindowLong (hWnd, GWL_USERDATA, (long)pWindow) ;
			pWindow->pRenderer->SetMainWindowSettings(GetDC(hWnd), hWnd);
			return DefWindowProc(hWnd,message,wParam,lParam);
		}else
			pWindow = (WINDOW*)GetWindowLong(hWnd, GWL_USERDATA);
		if(pWindow)
			return pWindow->WndProc(hWnd,message,wParam,lParam);
		else
			return DefWindowProc(hWnd,message,wParam,lParam);
	}
	//---------------------------------------------------------------------------
	LRESULT CALLBACK WINDOW::WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		switch (message){
			case WM_DESTROY:
				PostQuitMessage(0);
				break;
			case WM_SIZE:
				RECT Rect;
				GetClientRect(hWnd, &Rect);
				glViewport(0, 0, (GLsizei)Rect.right, Rect.bottom);
				break;
			case WM_KEYDOWN:
				pController->OnKeyboardCallback(pController->ConvertKeyCodeFromWin32ToAcrossKey(wParam), true);
				break;
			case WM_KEYUP:
				pController->OnKeyboardCallback(pController->ConvertKeyCodeFromWin32ToAcrossKey(wParam), false);
				break;
			case WM_MOUSEHWHEEL:
				pController->OnMouseScrollCallback(GET_WHEEL_DELTA_WPARAM(wParam));
				break;
			case WM_LBUTTONDOWN:
				pController->OnClickDownCallback(CONTROLLER::LEFT);
				break;
			case WM_RBUTTONDOWN:
				pController->OnClickDownCallback(CONTROLLER::RIGHT);
				break;
			case WM_MBUTTONDOWN:
				pController->OnClickDownCallback(CONTROLLER::MIDDLE);
				break;
			case WM_LBUTTONUP:
				pController->OnClickUpCallback(CONTROLLER::LEFT);
				break;
			case WM_RBUTTONUP:
				pController->OnClickUpCallback(CONTROLLER::RIGHT);
				break;
			case WM_MBUTTONUP:
				pController->OnClickUpCallback(CONTROLLER::MIDDLE);
				break;
			case WM_MOUSEMOVE:
				pController->OnMouseMoveCallback(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			case WM_APPCOMMAND:{
				int cmd = GET_APPCOMMAND_LPARAM(lParam);
				pController->OnKeyboardCallback(pController->ConvertCmdCodeFromWin32ToAcrossKey(cmd), true);
				break;
			}
			case WM_QUIT:
				bQuit = true;
				break;
			default:
				return DefWindowProc(_hWnd, message, wParam, lParam);
		}
		return 0;
	}
	//---------------------------------------------------------------------------
	AT::I8 WINDOW::Init(PLT_HINSTANCE hInstance){
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc	= WINDOW::sWndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= LoadIcon(hInstance, IDI_APPLICATION);
		wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= (LPCSTR)szWdwClassName;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, IDI_APPLICATION);

		Instance = hInstance; // Store instance handle in our global variable

		ATOM ClassAtom = RegisterClassEx(&wcex);
		if(!ClassAtom){
			DWORD err = GetLastError();
			Break();
			return FALSE;
		}

		hWnd = CreateWindowEx(NULL, MAKEINTATOM(ClassAtom), (LPCSTR)szWdwTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, (LPVOID)this);
		if(!hWnd){
			DWORD err = GetLastError();
			Break();
			return FALSE;
		}
	
		ShowWindow(hWnd, 1);
		SetWindowPos(hWnd, NULL, 0, 0, RENDERER::WIDTH, RENDERER::HEIGHT, SWP_NOMOVE);
		UpdateWindow(hWnd);
		hDC = GetDC(hWnd);
		return TRUE;
	}
	//---------------------------------------------------------------------------
	#elif _OSX
	
	#endif
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
