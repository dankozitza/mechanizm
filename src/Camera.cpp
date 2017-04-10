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

Camera::Camera(
      float X, float Y, float Z, float AX, float AY,
      float rotationSpeed, float translationSpeed) : 

	   _rotationSpeed(rotationSpeed),
	   _translationSpeed(translationSpeed),
	   _X(X),
	   _Y(Y),
	   _Z(Z),
      _AX(AX),
      _AY(AY),
      last_mouse_pos_x(0),
      last_mouse_pos_y(0) {
   
   _time = glutGet(GLUT_ELAPSED_TIME);
   _shift = 1.0;
        
   for(int i=0 ; i<255 ; i++) { _keyboard[i] = false; }
        
}

void Camera::rotation(int x, int y) {

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
}

void Camera::setKeyboard(int i, bool etat) {
   if (i >= 'A' && i <= 'Z') // convert to lower case
      i += 32;

   _keyboard[i] = etat;
}

void Camera::pressKey(unsigned char c) {

   if (c >= 'A' && c <= 'Z') { // convert to lower case
      c += 32;
      _shift = 3.5;
   }
   else if (c != ' ')
      _shift = 1.0;
	
	if(c == 'd') {
      _keyboard[c] = true;
		
     // _X += (cos(_AX) * cos(-_AY)) * _translationSpeed * _shift;
     // _Z += (sin(_AX) * cos(-_AY)) * _translationSpeed * _shift;
		
	}
	
	if(c == 'a') {
      _keyboard[c] = true;

      //_X -= (cos(_AX) * cos(-_AY)) * _translationSpeed * _shift;
      //_Z -= (sin(_AX) * cos(-_AY)) * _translationSpeed * _shift;

	}

	if(c == 'w') {
	   _keyboard[c] = true;

      //_X += (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * _shift;
	   //_Y += sin(-_AY) * _translationSpeed * _shift;
      //_Z += (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * _shift;
	}
 
	if(c == 's') {
	   _keyboard[c] = true;

      //_X -= (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * _shift;
	   //_Y -= sin(-_AY) * _translationSpeed * _shift;
      //_Z -= (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * _shift;
	}
   
   if (c == ' ') {
      _keyboard[c] = true;
      //_X -= (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * _shift;
      //_Y += cos(-_AY) * _translationSpeed * _shift;
      //_Z -= (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * _shift;

   }

   if (c == 'c') {
      _keyboard[c] = true;
      //_X += (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * _shift;
      //_Y -= cos(-_AY) * _translationSpeed * _shift;
      //_Z += (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * _shift;

   }

}

// computes the right speeds to give to the given directions using trigonometric formulas
void Camera::translation() {

	float t = (float)(glutGet(GLUT_ELAPSED_TIME) - _time);
	_time   = glutGet(GLUT_ELAPSED_TIME);

	if(_keyboard[100]) { // d
		
      _X += (cos(_AX) * cos(-_AY)) * _translationSpeed * t * _shift;
      _Z += (sin(_AX) * cos(-_AY)) * _translationSpeed * t * _shift;
		
	}
	
	if(_keyboard[97]) { // a

      _X -= (cos(_AX) * cos(-_AY)) * _translationSpeed * t * _shift;
      _Z -= (sin(_AX) * cos(-_AY)) * _translationSpeed * t * _shift;

		
	}

	if(_keyboard[119]) { // w
	 
      _X += (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t * _shift * 2;
	   _Y += sin(-_AY) * _translationSpeed * t * _shift * 2;
      _Z += (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t * _shift * 2;
	}

 
	if(_keyboard[115]) { // s
	 
      _X -= (cos(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t * _shift;
	   _Y -= sin(-_AY) * _translationSpeed * t * _shift;
      _Z -= (sin(_AX - M_PI/2) * cos(-_AY)) * _translationSpeed * t * _shift;
	}
   
   if (_keyboard[' ']) {
      
      _X -= (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t * _shift;
      _Y += cos(-_AY) * _translationSpeed * t * _shift;
      _Z -= (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t * _shift;

   }

   if (_keyboard['c']) {
      
      _X += (cos(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t * _shift;
      _Y -= cos(-_AY) * _translationSpeed * t * _shift;
      _Z += (sin(_AX - M_PI/2) * sin(-_AY)) * _translationSpeed * t * _shift;

   }
}
#endif
