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
		void		LookAt();
		void		BuildProjMatrix(AT::I32F fovy=60.f, AT::I32F aspect=1.0, AT::I32F znear=0.1f, AT::I32F zfar=1000.f);
		//----
		virtual	void Update(const WORLD& W)=0;										//at end of each Renderer.Update()
		virtual void KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown){}		//Call @ end of controller keyboard cb
		virtual void MouseMoveCB(const CONTROLLER& C, AT::I32 X, AT::I32 Y){}		//Call @ end of mouse move cb
		virtual void MouseScrollCB(AT::I32 DelatWheel){}							
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
	class ARCBALL_CAMERA:public BASE_CAMERA{
	public:
						 ARCBALL_CAMERA();
		/*virtual*/	void Update(const WORLD& W);
		/*virtual*/ void MouseMoveCB(const CONTROLLER& C, AT::I32 X, AT::I32 Y);
		/*virtual*/ void MouseScrollCB(AT::I32 DeltaWheel);
	protected:
		AT::VEC2Di	m_LastMousePos;
		AT::I32F	m_R;
		AT::I32F	m_Theta;
		AT::I32F	m_Phi;
		AT::I8		m_bDirty;
		//--
		AT::I8		m_Padding[3];
	};
	//---
	class ZELDA_CAMERA:public BASE_CAMERA{
	public:
						ZELDA_CAMERA();
		//--
		/*virtual*/	void Update(const WORLD& W);
		//--
	protected:
		AT::I32F	m_DistanceToPlayer;
		AT::VEC3Df	m_TargetToEye;
		AT::VEC3Df	m_EyeToUp;
	};
	//---
}//namespace AE
//---------------------------------------------------------------------------
