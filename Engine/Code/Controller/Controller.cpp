//---------------------------------------------------------------------------
//Project
#include "Controller.h"
#include "../Rendering/Renderer.h"
#include "../Rendering/RObject.h"
#include "../Window.h"
//WIN32
#include <Windows.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	CONTROLLER::ACROSS_KEY_CODE CONTROLLER::ConvertCmdCodeFromWin32ToAcrossKey(AT::U32 Win32CmdCode){
		switch(Win32CmdCode){
			case APPCOMMAND_VOLUME_UP :
				return CMD_VOLUME_UP;			
				break;
			case APPCOMMAND_VOLUME_DOWN :
				return CMD_VOLUME_DOWN;			
				break;
			case APPCOMMAND_MEDIA_NEXTTRACK:
				return CMD_NEXT_SONG;
				break;
			case APPCOMMAND_MEDIA_PREVIOUSTRACK:
				return CMD_PREV_SONG;
				break;
			default :
				return CONTROLLER::UNKNOWN;
				break;
		}
	}
	//---------------------------------------------------------------------------
	CONTROLLER::ACROSS_KEY_CODE CONTROLLER::ConvertKeyCodeFromWin32ToAcrossKey(AT::U32 Win32KeyCode){
		switch(Win32KeyCode){
			case VK_LEFT:	
				return KC_LEFT;
			case VK_RIGHT:
				return KC_RIGHT;
			case VK_UP:
				return KC_UP;
			case VK_DOWN:
				return KC_DOWN;
			case 'A':
				return KC_A;
			case 'B':
				return KC_B;
			case 'D' :
				return KC_D;
			case 'N':
				return KC_N;
			case 'P':
				return KC_P;
			case 'V':
				return KC_V;
			case VK_ESCAPE:
				return KC_ESCAPE;
			default :
				return CONTROLLER::UNKNOWN;
				break;
		}
	}
	//---------------------------------------------------------------------------
	void CONTROLLER::OnKeyboardCallback(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown){
		if(bDown){
			switch (KC){
				case CONTROLLER::UNKNOWN:
					break;
				case CONTROLLER::KC_LEFT:
				{
					m_pRenderer->m_MainCamera.MoveLeft();
					break;
				}
				case CONTROLLER::KC_RIGHT:
				{
					m_pRenderer->m_MainCamera.MoveRight();
					break;
				}
				case CONTROLLER::KC_UP:
				{
					m_pRenderer->m_MainCamera.MoveForward();
					break;
				}
				case CONTROLLER::KC_DOWN:
				{
					m_pRenderer->m_MainCamera.MoveBackward();
					break;
				}
				case CONTROLLER::KC_ESCAPE:
					m_bQuit = true;
					break;
				case KC_B:
					m_pRenderer->m_iPostProcess = 1; //blur
					break;
				case KC_A:
					m_pRenderer->m_iPostProcess = 2; //fxaa
					break;
				case KC_D:
					m_pRenderer->m_bDrawDebug = !m_pRenderer->m_bDrawDebug;
					break;
				case KC_N:
					m_pRenderer->m_iPostProcess = 0; //no post process
				case KC_V:
					m_pRenderer->ToggleVSync();
					break;
				case CMD_VOLUME_UP:
					m_pRenderer->m_BlurShader.IncreaseBlurWdw();
					break;
				case CMD_VOLUME_DOWN:
					m_pRenderer->m_BlurShader.DecreaseBlurWdw();
					break;
				case CMD_NEXT_SONG:
					m_pRenderer->m_BlurShader.IncreaseBlurRatio();
					break;
				case CMD_PREV_SONG:
					m_pRenderer->m_BlurShader.DecreaseBlurRatio();
					break;
				default:
					break;
			}
		}
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::OnClickDownCallback(MOUSE_BUTTON MB){
		if(m_MouseButton!=NO_BUTTON)
			m_MouseButton = MB;
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::OnClickUpCallback(MOUSE_BUTTON MB){
		if(m_MouseButton == MB)
			m_MouseButton = NO_BUTTON;
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::OnMouseMoveCallback(AT::I32 X, AT::I32 Y){
		m_MouseX = X;
		m_MouseY = Y;
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::OnMouseScrollCallback(AT::I32 DeltaWheel){
		m_Scroll = DeltaWheel;
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::Loop(){
		MSG msg;
		HACCEL hAccelTable;
		hAccelTable = LoadAccelerators(m_pMainWindow->Instance, MAKEINTRESOURCE(107));
		while(!m_bQuit){
			AT::I32 bRet = PeekMessage(&msg, m_pMainWindow->hWnd, 0, 0, 0);
			if(bRet>=0){
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			m_Xbox.UpdateState();
		}
	}
	//--------------------------------------------------------------------------
	void CONTROLLER::Update(){
		MSG msg;
		AT::I32 bRet = PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		if(bRet>=0){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if(msg.message == WM_QUIT)
			m_bQuit = true;
		m_Xbox.UpdateState();
	}
	//--------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
