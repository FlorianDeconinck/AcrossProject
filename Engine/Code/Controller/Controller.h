//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//AT
#include <CommonTypes.h>
//Windows
#include <Windows.h>
//Project
#include "XboxController.h"
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	class RENDERER;
	class WINDOW;
	class CONTROLLER{
	protected :
		RENDERER *			m_pRenderer;
		//--
	public :
		enum ACROSS_KEY_CODE{
			UNKNOWN = 0,
			KC_LEFT,
			KC_RIGHT,
			KC_UP,
			KC_DOWN,
			KC_A,
			KC_B,
			KC_N,
			KC_P,
			KC_V,
			KC_D,
			KC_ESCAPE,
			CMD_VOLUME_UP,
			CMD_VOLUME_DOWN,
			CMD_NEXT_SONG,
			CMD_PREV_SONG,
		};
		WINDOW*				m_pMainWindow;
		XBOX_CONTROLLER		m_Xbox;
		AT::I32				m_MouseX;
		AT::I32				m_MouseY;
		AT::I32				m_Scroll;
		enum MOUSE_BUTTON{
			NO_BUTTON=0,
			LEFT,
			RIGHT,
			MIDDLE
		};
		MOUSE_BUTTON		m_MouseButton;
		AT::I8				m_bQuit;
		AT::I8				m_Padding[3];
		//--
						CONTROLLER():m_pRenderer(NULL),m_bQuit(false),m_MouseButton(NO_BUTTON){}
		ACROSS_KEY_CODE ConvertCmdCodeFromWin32ToAcrossKey(AT::U32 Win32CmdCode);
		ACROSS_KEY_CODE ConvertKeyCodeFromWin32ToAcrossKey(AT::U32 Win32KeyCode);
		void			OnKeyboardCallback(ACROSS_KEY_CODE KC, AT::I8 bDown);
		void			OnGamepadCallback();
		void			OnClickDownCallback(MOUSE_BUTTON MB);
		void			OnClickUpCallback(MOUSE_BUTTON MB);
		void			OnMouseMoveCallback(AT::I32 X, AT::I32 Y);
		void			OnMouseScrollCallback(AT::I32 DeltaWheel);
		void			OnMouseLeaveWdwCallback();
		void			Loop();
		void			Update();
		//--
		inline void			SetDependancies(RENDERER* _R){m_pRenderer = _R;}
		inline MOUSE_BUTTON	GetClickedMouseButton() const { return m_MouseButton; }
		inline AT::I32		GetWheelScroll()		const { return m_Scroll; }
	};
	//---------------------------------------------------------------------------
	static DWORD __stdcall ControllerThread_CB(LPVOID Param){
		CONTROLLER* pController= (CONTROLLER*)Param;
		//--
		pController->Loop();
		return 0;
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------