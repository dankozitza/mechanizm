/* DEISS Olivier                                                                           */
/* Class : Camera                                                                          */
/* Last Update : 12/28/2014                                                                */

/* This class is a Camera. It provides all the information needed by gluLookAt() function. */

#ifndef _MECH_CAMERA
#define _MECH_CAMERA

#include <cmath>

class Camera {

	public :
	
		Camera(float, float, float, float, float, float, float);
		float getSightX()					{ return _X + sin(_AX)*sin(_AY); }
		float getSightY()					{ return _Y + cos(_AY); }
		float getSightZ()					{ return _Z + cos(_AX)*sin(_AY); }
		float getX()						{ return _X; }
		float getY()						{ return _Y; }
		float getZ()						{ return _Z; }
      float getAX()                 { return _AX; }
      float getAY()                 { return _AY; }
		void rotation(int, int);
		void setKeyboard(int i, bool etat);
		void setX(float X)					{ _X = X; }
		void setY(float Y)					{ _Y = Y; }
		void setZ(float Z)					{ _Z = Z; }
		void translation();	
      bool pressKey(unsigned char c);
      void pos_in_los(float distance, float& x, float& y, float& z);
      int last_mouse_pos_x;
      int last_mouse_pos_y;
      bool agm_enabled;
      bool  _keyboard[255]; // to know if one specific key is up or down

	private :
	
		//bool  _keyboard[255]; // to know if one specific key is up or down
		float _rotationSpeed;
		int   _time;
		float _translationSpeed;
		float _X;
		float _Y;
		float _Z;
		float _AX;
		float _AY;
      float _shift; // when shift is pressed this increases speed
};

#endif
