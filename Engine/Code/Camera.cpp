//---------------------------------------------------------------------------
//Project
#include "Camera.h"
#include "World.h"
#include "Rendering/Renderer.h"
#include "./GUI/GUI.h"
//STD
#include <math.h>
//Tool
#include <MathMacros.h>
namespace AE{
	//---------------------------------------------------------------------------
	//BASE CAMERA
	//---------------------------------------------------------------------------
	BASE_CAMERA::BASE_CAMERA(){
		m_View.Identity();
		m_Proj.Identity();
		m_Eye.Zero();
		m_Target.Zero();
		m_Up.Zero();
	}
	//---------------------------------------------------------------------------
	void MatrixMultNxN(AT::I32F R[4][4], AT::I32F A[4][4], AT::I32F B[4][4]){
		static const int NMatrixMult = 4;
		for(int r = 0 ; r < NMatrixMult ; ++r){
			for(int c = 0 ; c < NMatrixMult ; ++c){
				R[c][r] = 0;
				for (int k = 0; k < NMatrixMult; k++)
					R[c][r] += A[c][k] * B[k][r];
			}
		}
	}
	//---------------------------------------------------------------------------
	void BASE_CAMERA::LookAt(){
		LookAt(m_Eye, m_Target, m_Up);
	}
	//---------------------------------------------------------------------------
	void BASE_CAMERA::LookAt(AT::VEC3Df Eye, AT::VEC3Df Target, AT::VEC3Df Up){
		m_Eye = Eye;
		m_Target = Target;
		m_Up = Up;
		AT::VEC3Df zaxis = Target - Eye;		  // The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = Up.Cross(zaxis);		  // The "right" vector.
		xaxis.Normalize();
		AT::VEC3Df yaxis = zaxis.Cross(xaxis);    // The "up" vector.

		// Create a 4x4 orientation matrix from the right, up, and at vectors
	#if 0
		View.T.x = -Eye.x;
		View.T.y = -Eye.y;
		View.T.z = -Eye.z;
	
		View.Q.w = sqrt(1+xaxis.x-yaxis.y-zaxis.z) / 2;
		View.Q.i = (yaxis.z - zaxis.y) / (4*View.Q.w);
		View.Q.j = (zaxis.x - xaxis.z) / (4*View.Q.w);
		View.Q.k = (xaxis.y - yaxis.y) / (4*View.Q.w);
	#else
		//--
		AT::I32F orientation[4][4] = {
			xaxis.x, yaxis.x, zaxis.x, 0,
			xaxis.y, yaxis.y, zaxis.y, 0,
			xaxis.z, yaxis.z, zaxis.z, 0,
			0,       0,       0,     1
		};
		AT::I32F translation[4][4] = {
			1,      0,      0,     0,
			0,      1,      0,     0, 
			0,      0,      1,     0,
			-Eye.x, -Eye.y, -Eye.z,  1
		};
		MatrixMultNxN(m_View.m_GLMat, translation, orientation);
		m_View.m_bDirty=false;
	#endif
		
	// 	Matrix4 orientation = {
	// 		xaxis.x, yaxis.x, zaxis.x, 0,
	// 		xaxis.y, yaxis.y, zaxis.y, 0,
	// 		xaxis.z, yaxis.z, zaxis.z, 0,
	// 		0,       0,       0,     1
	// 	};
	// 	// Create a 4x4 translation matrix by negating the eye position.
	// 	Matrix4 translation = {
	// 		1,      0,      0,     0,
	// 		0,      1,      0,     0, 
	// 		0,      0,      1,     0,
	// 		-eye.x, -eye.y, -eye.z,  1
	// 	};
	// 
	// 	// Combine the orientation and translation to compute the view matrix
	// 	return ( translation * orientation );
	}
	//---------------------------------------------------------------------------
	AT::I32F ToRadians(AT::I32F Degrees){
		return Degrees * 3.159265359f / 180.f;
	}
	//---------------------------------------------------------------------------
	void BASE_CAMERA::BuildProjMatrix(AT::I32F fovyInDegrees/*=60.f*/, AT::I32F aspect/*=1.0*/, AT::I32F zNear/*=0.1f*/, AT::I32F zFar/*=1000.f*/){
			AT::I32F range = tan(ToRadians(fovyInDegrees / 2)) * zNear;	
			AT::I32F left = -range * aspect;
			AT::I32F right = range * aspect;
			AT::I32F bottom = -range;
			AT::I32F top = range;

	// 		Result[0][0] = (valType(2) * zNear) / (right - left);
	// 		Result[1][1] = (valType(2) * zNear) / (top - bottom);
	// 		Result[2][2] = - (zFar + zNear) / (zFar - zNear);
	// 		Result[2][3] = - valType(1);
	// 		Result[3][2] = - (valType(2) * zFar * zNear) / (zFar - zNear);
	#if 0
			Proj.GLMat[0][0]  = 2/(right - left);
			Proj.GLMat[1][1]  = 2/(top - bottom);
			Proj.GLMat[2][2] = -2/(zFar - zNear);
			Proj.GLMat[3][3] = 1; 
			Proj.GLMat[0][3] = -(right + left)/(right-left); 
			Proj.GLMat[1][3] = -(top+bottom)/(top - bottom) ;
			Proj.GLMat[2][3] = (zFar+zNear)/(zFar - zNear) ;
	#else
			m_Proj.m_GLMat[0][0]  = zNear / right;
			m_Proj.m_GLMat[1][1]  = zNear / top;
			m_Proj.m_GLMat[2][2] = -(zFar + zNear)/(zFar - zNear);
			m_Proj.m_GLMat[2][3] = -1; 
			m_Proj.m_GLMat[3][2] = -(2*zFar*zNear)/(zFar - zNear) ;
	#endif
			m_Proj.m_bDirty=false;
	}
	//---------------------------------------------------------------------------
	//CLASSIC CAMERA
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	inline void MoveLeft(BASE_CAMERA& Cam){
		AT::VEC3Df zaxis = Cam.m_Target - Cam.m_Eye;				// The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = Cam.m_Up.Cross(zaxis);				// The "right" vector.
		xaxis.Normalize(); 
		xaxis = xaxis*-0.1f;
		Cam.LookAt(Cam.m_Eye+xaxis, Cam.m_Target+xaxis, Cam.m_Up);
	}
	//---------------------------------------------------------------------------
	inline void MoveRight(BASE_CAMERA& Cam){
		AT::VEC3Df zaxis = Cam.m_Target - Cam.m_Eye;				// The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = Cam.m_Up.Cross(zaxis);				// The "right" vector.
		xaxis.Normalize(); 
		xaxis = xaxis*0.1f;
		Cam.LookAt(Cam.m_Eye+xaxis, Cam.m_Target+xaxis, Cam.m_Up);
	}
	//---------------------------------------------------------------------------
	inline void MoveForward(BASE_CAMERA& Cam){
		AT::VEC3Df Forward(0.1f, 0.f, 0.1f);
		Cam.LookAt(Cam.m_Eye+Forward, Cam.m_Target+Forward, Cam.m_Up);
	}
	//---------------------------------------------------------------------------
	inline void MoveBackward(BASE_CAMERA& Cam){
		AT::VEC3Df Backward(-0.1f, 0.f, -0.1f);
		Cam.LookAt(Cam.m_Eye+Backward, Cam.m_Target+Backward, Cam.m_Up);
	}
	//---------------------------------------------------------------------------
	void CLASSIC_CAMERA::KeyboardCB(CONTROLLER::ACROSS_KEY_CODE KC, AT::I8 bDown){
		switch(KC){
			case CONTROLLER::KC_LEFT:
				MoveLeft(*this);
				break;
			case CONTROLLER::KC_RIGHT:
				MoveRight(*this);
				break;
			case CONTROLLER::KC_UP:
				MoveForward(*this);
				break;
			case CONTROLLER::KC_DOWN:
				MoveBackward(*this);
				break;
		}
	}
	//---------------------------------------------------------------------------
	// BALL CAMERA 
	//---------------------------------------------------------------------------
	ARCBALL_CAMERA::ARCBALL_CAMERA(){
		m_LastMousePos.Set(-1, -1);
		m_R = 5.f;
		m_Theta = AT_PI/2.f;
		m_Phi = -AT_PI;
		m_bDirty = true;
		//--
		m_Up.Set(0, 1.f, 0);
		m_Eye.Zero();
		m_Target.Zero();
	}
	//---------------------------------------------------------------------------
	void ARCBALL_CAMERA::Update(const WORLD& W){
		if(!m_bDirty)
			return;
		//--
		m_Eye = m_Target;
		m_Eye.z += m_R * sin(m_Theta) * cos(m_Phi);
		m_Eye.x += m_R * sin(m_Theta) * sin(m_Phi);
		m_Eye.y += m_R * cos(m_Theta);
		//--
		LookAt(m_Eye, m_Target, m_Up);
		m_bDirty = false;
	}
	//---------------------------------------------------------------------------
	void ARCBALL_CAMERA::MouseMoveCB(const CONTROLLER& C, AT::I32 X, AT::I32 Y){
		AT::VEC2Di DeltaMove(X - m_LastMousePos.x, Y - m_LastMousePos.y);
		DeltaMove.y *= -1;
		const static float speed = 10.f;
		if(C.GetClickedMouseButton() == CONTROLLER::LEFT){
			m_Theta += AT_DEG_TO_RAD(((AT::I32F)DeltaMove.y / (AT::I32F)RENDERER::HEIGHT)*2*AT_PI*speed);
			m_Phi	+= AT_DEG_TO_RAD(((AT::I32F)DeltaMove.x / (AT::I32F)RENDERER::WIDTH)*2*AT_PI*speed);
			m_bDirty = true;
		}
		if(C.GetClickedMouseButton() == CONTROLLER::MIDDLE){
			AT::I32F dx = m_Eye.x-m_Target.x;
			AT::I32F dy = m_Eye.z-m_Target.z;
			AT::VEC2Df ZXNormalToTargetEye(-dy, dx);
			AT::I32F Move = ((AT::I32F)DeltaMove.x / (AT::I32F)RENDERER::WIDTH);
			m_Target.x += ZXNormalToTargetEye.x * Move;
			m_Target.z += ZXNormalToTargetEye.y * Move;
			m_Target.y -= ((AT::I32F)DeltaMove.y / (AT::I32F)RENDERER::HEIGHT)*speed;
			m_bDirty = true;
		}
		m_LastMousePos.Set(X, Y);
	}
	//---------------------------------------------------------------------------
	void ARCBALL_CAMERA::MouseScrollCB(AT::I32 DeltaWheel){
		m_R += DeltaWheel;
		if(m_R <= 0)
			m_R = 1.f;
		m_bDirty = true;
	}
	//---------------------------------------------------------------------------
	// ZELDA CAMERA
	//---------------------------------------------------------------------------
	ZELDA_CAMERA::ZELDA_CAMERA():m_DistanceToPlayer(5){
		//--
		m_Up.Set(0.f, 1.f, 0.f);
		m_Target.Zero();
		m_Eye.Zero();
		//--
		m_TargetToEye.z = -m_DistanceToPlayer;
		static const AT::I32F alpha_in_degree = 30;
		static const AT::I32F theta_in_degree = 0;
		m_TargetToEye = AT::QUAT<AT::I32F>::RotateAngleAxis(m_TargetToEye, AT_DEG_TO_RAD(alpha_in_degree), 1.f, 0, 0); //up
		m_TargetToEye = AT::QUAT<AT::I32F>::RotateAngleAxis(m_TargetToEye, AT_DEG_TO_RAD(theta_in_degree), 0, 1.f, 0); //behind
	}
	//---------------------------------------------------------------------------
	void ZELDA_CAMERA::Update(const WORLD& W){
		if(W.GetPlayerCount()<=0)
			return;
		//--
		AT::VEC2Df Player0WorldPos = W.GetPlayerWorldPos(0);
		//--
		m_Target = AT::VEC3Df(Player0WorldPos.x, 0, Player0WorldPos.y);
		//--
		m_Eye = m_Target + m_TargetToEye;
		LookAt(m_Eye, m_Target, m_Up);
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
