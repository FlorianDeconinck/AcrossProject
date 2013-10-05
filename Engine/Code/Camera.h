//---------------------------------------------------------------------------
#pragma once
//---------------------------------------------------------------------------
//Tools
#include <Trf.h>
#include <CommonTypes.h>
//---------------------------------------------------------------------------
namespace AE{
	//---
	class BASE_CAMERA{
	public:
		AT::TRFf m_View;
		AT::TRFf m_Proj;
		AT::VEC3Df m_Eye;
		AT::VEC3Df m_Target;
		AT::VEC3Df m_Up;
		void LookAt(AT::VEC3Df Eye, AT::VEC3Df Target, AT::VEC3Df Up);
		void BuildProjMatrix(AT::I32F fovy=60.f, AT::I32F aspect=1.0, AT::I32F znear=0.1f, AT::I32F zfar=1000.f);
	};
	//---
	class ZELDA_CAMERA:public BASE_CAMERA{
	public :
		void MoveLeft();
		void MoveRight();
		void MoveForward();
		void MoveBackward();
	};
	//---
}//namespace AE
//---------------------------------------------------------------------------