//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//AT
#include <CommonTypes.h>
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
	class RENDERER;
	class CONTROLLER;
	//---------------------------------------------------------------------------
	class WINDOW{
	private :
		AT::I8			szWdwClassName[256];
		AT::I8			szWdwTitle[256];
		AT::I8			bQuit;
		RENDERER*	pRenderer;
		HDC				hDC;
	public :
		HWND			hWnd;
		PLT_HINSTANCE	Instance;
		CONTROLLER*		pController;
		//---
		WINDOW();
		~WINDOW(){}
		void SetNames(const char* Name, const char* Title);
		void AttachEngines(RENDERER& _R, CONTROLLER& _C);
		AT::I8 Init(PLT_HINSTANCE hInstance);
		static LRESULT CALLBACK sWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
	//--------------------------------------------------------------------------
}//namespace AE
//--------------------------------------------------------------------------
