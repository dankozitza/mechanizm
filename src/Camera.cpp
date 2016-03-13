/* DEISS Olivier             */
/* Class : Camera            */
/* Last Update : 12/28/2014  */

#ifndef CAMERAHPP
#define CAMERAHPP

#include "Camera.hpp" 

#ifdef __linux__
    #include <GL/glut.h>
#else
    #include <GLUT/glut.h>
#endif

#include <iostream>

Camera::Camera(float X, float Y, float Z, float AX, float AY, float rotationSpeed, float translationSpeed, int width, int height) : 
	_mouseX(width/2),
	_mouseY(height/2),
	_rotationSpeed(rotationSpeed),
	_translationSpeed(translationSpeed),
	_X(X),
	_Y(Y),
	_Z(Z),
   _AX(AX),
   _AY(AY),
   last_mouse_pos_x(0),
   last_mouse_pos_y(0) {
        
    for(int i=0 ; i<255 ; i++) { _keyboard[i] = false; }
        
}

void Camera::rotation(int x, int y) {

//   glRotatf(_X, 1.0, 0.0, 0.0); // rotate our camera on the x axis
//   glRotatef(_y, 0.0, 1.0, 0.0);// rotate our camera on the y axis
//   GLTranslated(-_X, -_Y, -_Z); // translate the screen

   GLfloat diffx = x - last_mouse_pos_x;
   GLfloat diffy = y - last_mouse_pos_y;

   if (diffx > 4.0 || diffx < -4.0) {
      diffx = 0.0;
   }
   if (diffy > 4.0 || diffy < -4.0) {
      diffy = 0.0;
   }

   last_mouse_pos_x = x;
   last_mouse_pos_y = y;

   // keep AX and AY between -2pi and 2pi
   if (_AX + 3.141592654 * diffx / 180 > 2 * M_PI)
      _AX -= 2 * M_PI;
   if (_AX + 3.141592654 * diffx / 180 < -2 * M_PI)
      _AX += 2 * M_PI;

   if (_AY + 3.141592654 * diffy / 180 > 2 * M_PI)
      _AY -= 2 * M_PI;
   if (_AY + 3.141592654 * diffy / 180 < -2 * M_PI)
      _AY += 2 * M_PI;

   _AX += (3.141592654 * diffx / 180) * _rotationSpeed;
   _AY += (3.141592654 * diffy / 180) * _rotationSpeed;

	//_AX -= (float)(x - _mouseX) * _rotationSpeed;
	//_AY   += (float)(y - _mouseY) * _rotationSpeed;
	
	//if(_AY <= 0.1)			   _AY = 0.1;
	//else if(_AY >= 0.95*M_PI) _AY = 0.95*M_PI;
	
	//_mouseX = x;
	//_mouseY = y;
}

void Camera::pressKey(unsigned char c) {
	
	if(c == 'd') {
		
		//_X -= sin(_AX + M_PI/2)*sin(_AY) * _translationSpeed * t;
		//_Z -= cos(_AX + M_PI/2)*sin(_AY) * _translationSpeed * t;

      _X += (cos(_AX) * cos(-_AY)) * _translationSpeed;
      _Z += (sin(_AX) * cos(-_AY)) * _translationSpeed;
		
	}
	
	if(c == 'a') {

		//_X -= sin(_AX - M_PI/2)*sin(_AY) * _translationSpeed * t;
		//_Z -= cos(_AX - M_PI/2)*sin(_AY) * _translationSpeed * t;

      _X -= (cos(_AX) * cos(-_AY)) * _translationSpeed;
      _Z -= (sin(_AX) * cos(-_AY)) * _translationSpeed;

		
	}

	if(c == 'w') {
	 
		//_X += sin(_AX)*sin(_AY) * _translationSpeed * t;
		//_Y += cos(_AY)			    * _translationSpeed * t;
		//_Z += cos(_AX)*sin(_AY) * _translationSpeed * t;

      _X += (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed;
	   _Y += sin(-_AY) * _translationSpeed;
      _Z += (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed;
	}

 
	if(c == 's') {
	 
//		_X -= sin(_AX)*sin(_AY) * _translationSpeed * t;
//		_Y -= cos(_AY)			    * _translationSpeed * t;
//		_Z -= cos(_AX)*sin(_AY) * _translationSpeed * t;

      //_X -= cos(_AX) * _translationSpeed * t;
	   //_Y += sin(_AX) * _translationSpeed * t;
      //_Z += cos(-_AY) * _translationSpeed * t;

      _X -= (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed;
	   _Y -= sin(-_AY) * _translationSpeed;
      _Z -= (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed;
	}
   
   if (c == ' ') {
      
      _X -= (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed;
      _Y += cos(-_AY) * _translationSpeed;
      _Z -= (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed;

   }

   if (c == 'c') {
      
      _X += (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed;
      _Y -= cos(-_AY) * _translationSpeed;
      _Z += (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed;

   }

}

// computes the right speeds to give to the given directions using trigonometric formulas
void Camera::translation() {
	
	float t = (float)(glutGet(GLUT_ELAPSED_TIME) - _time);
	_time   = glutGet(GLUT_ELAPSED_TIME);
	
	if(_keyboard[100]) { // d
		
		//_X -= sin(_AX + M_PI/2)*sin(_AY) * _translationSpeed * t;
		//_Z -= cos(_AX + M_PI/2)*sin(_AY) * _translationSpeed * t;

      _X += (cos(_AX) * cos(-_AY)) * _translationSpeed * t;
      _Z += (sin(_AX) * cos(-_AY)) * _translationSpeed * t;
		
	}
	
	if(_keyboard[97]) { // a

		//_X -= sin(_AX - M_PI/2)*sin(_AY) * _translationSpeed * t;
		//_Z -= cos(_AX - M_PI/2)*sin(_AY) * _translationSpeed * t;

      _X -= (cos(_AX) * cos(-_AY)) * _translationSpeed * t;
      _Z -= (sin(_AX) * cos(-_AY)) * _translationSpeed * t;

		
	}

	if(_keyboard[119]) { // w
	 
		//_X += sin(_AX)*sin(_AY) * _translationSpeed * t;
		//_Y += cos(_AY)			    * _translationSpeed * t;
		//_Z += cos(_AX)*sin(_AY) * _translationSpeed * t;

      _X += (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t;
	   _Y += sin(-_AY) * _translationSpeed * t;
      _Z += (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t;
	}

 
	if(_keyboard[115]) { // s
	 
//		_X -= sin(_AX)*sin(_AY) * _translationSpeed * t;
//		_Y -= cos(_AY)			    * _translationSpeed * t;
//		_Z -= cos(_AX)*sin(_AY) * _translationSpeed * t;

      //_X -= cos(_AX) * _translationSpeed * t;
	   //_Y += sin(_AX) * _translationSpeed * t;
      //_Z += cos(-_AY) * _translationSpeed * t;

      _X -= (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t;
	   _Y -= sin(-_AY) * _translationSpeed * t;
      _Z -= (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t;
	}
   
   if (_keyboard[' ']) {
      
      _X -= (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t;
      _Y += cos(-_AY) * _translationSpeed * t;
      _Z -= (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t;

   }

   if (_keyboard['c']) {
      
      _X += (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t;
      _Y -= cos(-_AY) * _translationSpeed * t;
      _Z += (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t;

   }

}
																						  
#endif
