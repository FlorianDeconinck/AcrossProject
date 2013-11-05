//---------------------------------------------------------------------------
//Tools
#include <CodeTools.h>
//Project
#include "./Rendering/Renderer_Interface.h"
#include "Window.h"
#include "./Controller/Controller.h"
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
	WINDOW::WINDOW():m_hDC(NULL),m_bTrackMouse(false){
		m_bQuit = false;
	}
	//---------------------------------------------------------------------------
	void WINDOW::SetNames(const char* Name, const char* Title){
		strcpy((char*)m_szWdwClassName, (char*)Name);
		strcpy((char*)m_szWdwTitle, (char*)Title);
	}
	//---------------------------------------------------------------------------
	void WINDOW::AttachEngines(RENDERER_ABC* _R, CONTROLLER& _C){
		m_pRenderer = _R;
		m_pController = &_C;
	}
	//---------------------------------------------------------------------------
	#ifdef _WIN32
	//---------------------------------------------------------------------------
	LRESULT CALLBACK WINDOW::sWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		WINDOW* pWindow=NULL;
		if (message == WM_NCCREATE){
			pWindow = (WINDOW*) ((LPCREATESTRUCT) lParam)->lpCreateParams ;
			SetWindowLong (hWnd, GWL_USERDATA, (long)pWindow) ;
			pWindow->m_pRenderer->SetMainWindowSettings(GetDC(hWnd), hWnd);
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
				GetClientRect(m_hWnd, &Rect);
				glViewport(0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT);
				break;
			case WM_KEYDOWN:
				m_pController->OnKeyboardCallback(m_pController->ConvertKeyCodeFromWin32ToAcrossKey(wParam), true);
				break;
			case WM_KEYUP:
				m_pController->OnKeyboardCallback(m_pController->ConvertKeyCodeFromWin32ToAcrossKey(wParam), false);
				break;
			case WM_MOUSEWHEEL:
				m_pController->OnMouseScrollCallback(GET_WHEEL_DELTA_WPARAM(wParam));
				break;
			case WM_MOUSELEAVE :
				m_pController->OnMouseLeaveWdwCallback();
				m_bTrackMouse = false;
				break;
			case WM_LBUTTONDOWN:
				m_pController->OnClickDownCallback(CONTROLLER::LEFT);
				break;
			case WM_RBUTTONDOWN:
				m_pController->OnClickDownCallback(CONTROLLER::RIGHT);
				break;
			case WM_MBUTTONDOWN:
				m_pController->OnClickDownCallback(CONTROLLER::MIDDLE);
				break;
			case WM_LBUTTONUP:
				m_pController->OnClickUpCallback(CONTROLLER::LEFT);
				break;
			case WM_RBUTTONUP:
				m_pController->OnClickUpCallback(CONTROLLER::RIGHT);
				break;
			case WM_MBUTTONUP:
				m_pController->OnClickUpCallback(CONTROLLER::MIDDLE);
				break;
			case WM_MOUSEMOVE:{
				if(!m_bTrackMouse){
					TRACKMOUSEEVENT tm;
					tm.cbSize = sizeof(TRACKMOUSEEVENT);
					tm.dwFlags = TME_LEAVE;
					tm.hwndTrack = m_hWnd;
					if(TrackMouseEvent(&tm))
						m_bTrackMouse = true;
				}
				m_pController->OnMouseMoveCallback(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				break;
			}case WM_APPCOMMAND:{
				int cmd = GET_APPCOMMAND_LPARAM(lParam);
				m_pController->OnKeyboardCallback(m_pController->ConvertCmdCodeFromWin32ToAcrossKey(cmd), true);
				break;
			}
			case WM_QUIT:
				m_bQuit = true;
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
		wcex.lpszClassName	= (LPCSTR)m_szWdwClassName;
		wcex.hIconSm		= LoadIcon(wcex.hInstance, IDI_APPLICATION);

		m_Instance = hInstance; // Store instance handle in our global variable

		ATOM ClassAtom = RegisterClassEx(&wcex);
		if(!ClassAtom){
			DWORD err = GetLastError();
			Break();
			return FALSE;
		}

		m_hWnd = CreateWindowEx(NULL, MAKEINTATOM(ClassAtom), (LPCSTR)m_szWdwTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, NULL, NULL, hInstance, (LPVOID)this);
		if(!m_hWnd){
			DWORD err = GetLastError();
			Break();
			return FALSE;
		}
	
		ShowWindow(m_hWnd, 1);
		SetWindowPos(m_hWnd, NULL, 0, 0, RENDERER_ABC::WIDTH, RENDERER_ABC::HEIGHT, SWP_NOMOVE);
		UpdateWindow(m_hWnd);
		m_hDC = GetDC(m_hWnd);
		return TRUE;
	}
	//---------------------------------------------------------------------------
	#elif _OSX
	
	#endif
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
