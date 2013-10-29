//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Project
#include "Controller/Controller.h"
//Tools
#include <Trf.h>
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	//---
	class WORLD;
	//---
	class BASE_CAMERA{
	public:
		AT::TRFf	m_View;
		AT::TRFf	m_Proj;
		AT::VEC3Df	m_Eye;
		AT::VEC3Df	m_Target;
		AT::VEC3Df	m_Up;
		//----
					BASE_CAMERA();
		void		LookAt(AT::VEC3Df Eye, AT::VEC3Df Target, AT::VEC3Df Up);
		void		BuildProjMatrix(AT::I32F fovy=60.f, AT::I32F aspect=1.0, AT::I32F znear=0.1f, AT::I32F zfar=1000.f);
		//----
		virtual	void Update(const WORLD& W)=0;											//at end of each Renderer.Update()
		virtual void KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown)=0;		//Call @ end of controller keyboard cb
	};
	//---
	class CLASSIC_CAMERA:public BASE_CAMERA{
	public :
		//--
		/*virtual*/	void Update(const WORLD& W){}
		/*virtual*/ void KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown);
		//--
	};
	//---
	class ZELDA_CAMERA:public BASE_CAMERA{
	public:
						ZELDA_CAMERA();
		//--
		/*virtual*/	void Update(const WORLD& W);
		/*virtual*/ void KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown){}
		//--
	protected:
		AT::I32F	m_DistanceToPlayer;
		AT::VEC3Df	m_TargetToEye;
		AT::VEC3Df	m_EyeToUp;
	};
	//---
}//namespace AE
//---------------------------------------------------------------------------