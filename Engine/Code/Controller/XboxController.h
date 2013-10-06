//-----------------------------------------------------------------------------
#pragma once
//-----------------------------------------------------------------------------
//Tool
#include "Vec2D.h"
//Windows
#include <Xinput.h>
#include <winerror.h>
//STD
#include <cmath>
//-----------------------------------------------------------------------------
namespace AE{
	//-----------------------------------------------------------------------------
	class XBOX_CONTOLLER{
		public :
			XINPUT_STATE m_state;
			AT::VEC2Df	 m_leftStick;
			void	UpdateState(){
				memset(&m_state, 0, sizeof(XINPUT_STATE));
				DWORD dwRes = XInputGetState(0, &m_state);
				if(dwRes == ERROR_SUCCESS){
					//LEFT STICK
					float LX = m_state.Gamepad.sThumbLX;
					float LY = m_state.Gamepad.sThumbLY;
					float magnitude = sqrt(LX*LX + LY*LY);//determine how far the controller is pushed
					if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ){
						m_leftStick.x = LX / magnitude;
						m_leftStick.y = LY / magnitude;
					}else
						m_leftStick.Zero();
				}
			}
	};
	//-----------------------------------------------------------------------------
}//namespace AE
//-----------------------------------------------------------------------------