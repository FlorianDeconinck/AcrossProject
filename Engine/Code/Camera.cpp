//---------------------------------------------------------------------------
//Project
#include "Camera.h"
//STD
#include <math.h>
namespace AE{
	//---------------------------------------------------------------------------
	//BASE CAMERA
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
	void BASE_CAMERA::LookAt(AT::VEC3Df Eye, AT::VEC3Df Target, AT::VEC3Df Up){
		m_Eye = Eye;
		m_Target = Target;
		m_Up = Up;
		AT::VEC3Df zaxis = Target - Eye;					// The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = Up.Cross(zaxis);				// The "right" vector.
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
	//ZELDA CAMERA
	//---------------------------------------------------------------------------
	void ZELDA_CAMERA::MoveLeft(){
		AT::VEC3Df zaxis = m_Target - m_Eye;				// The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = m_Up.Cross(zaxis);				// The "right" vector.
		xaxis.Normalize(); 
		xaxis = xaxis*-0.1f;
		LookAt(m_Eye+xaxis, m_Target+xaxis, m_Up);
	}
	//---------------------------------------------------------------------------
	void ZELDA_CAMERA::MoveRight(){
		AT::VEC3Df zaxis = m_Target - m_Eye;				// The "look-at" vector.
		zaxis.Normalize();
		AT::VEC3Df xaxis = m_Up.Cross(zaxis);				// The "right" vector.
		xaxis.Normalize(); 
		xaxis = xaxis*0.1f;
		LookAt(m_Eye+xaxis, m_Target+xaxis, m_Up);
	}
	//---------------------------------------------------------------------------
	void ZELDA_CAMERA::MoveForward(){
		AT::VEC3Df Forward(0.1f, 0.f, 0.1f);
		LookAt(m_Eye+Forward, m_Target+Forward, m_Up);
	}
	//---------------------------------------------------------------------------
	void ZELDA_CAMERA::MoveBackward(){
		AT::VEC3Df Backward(-0.1f, 0.f, -0.1f);
		LookAt(m_Eye+Backward, m_Target+Backward, m_Up);
	}
	//---------------------------------------------------------------------------
}//namespace AE
//---------------------------------------------------------------------------
