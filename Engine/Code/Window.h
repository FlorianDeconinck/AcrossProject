//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//AT
#include <AcrossTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	//---------------------------------------------------------------------------
	#ifdef _WIN32
		#include <wtypes.h>
		typedef HANDLE PLT_HANDLE;
		typedef HINSTANCE PLT_HINSTANCE;
		typedef HWND PLT_HWND;
	#endif
	//---------------------------------------------------------------------------
	class RENDERER_ABC;
	class CONTROLLER;
	//---------------------------------------------------------------------------
	class WINDOW{
	private :
		AT::I8			m_szWdwClassName[256];
		AT::I8			m_szWdwTitle[256];
		AT::I8			m_bQuit;
		AT::I8			m_bTrackMouse;
		RENDERER_ABC*	m_pRenderer;
		HDC				m_hDC;
	public :
		HWND			m_hWnd;
		PLT_HINSTANCE	m_Instance;
		CONTROLLER*		m_pController;
		//---
		WINDOW();
		~WINDOW(){}
		void SetNames(const char* Name, const char* Title);
		void AttachEngines(RENDERER_ABC* _R, CONTROLLER& _C);
		AT::I8 Init(PLT_HINSTANCE hInstance);
		static LRESULT CALLBACK sWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
	//--------------------------------------------------------------------------
}//namespace AE
//--------------------------------------------------------------------------
