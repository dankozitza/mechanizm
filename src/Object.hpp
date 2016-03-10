// 
// Object.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_OBJECT
#define _MECH_OBJECT

#include <GL/glut.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "tools.hpp"
#include "Object.hpp"

using namespace std;


class Object {

   public:

      typedef tools::Error (*Function)(double, Object&);

      int vertices = 8;
      GLfloat cube[8][3];
      int faceIndex[6][4];

      GLfloat ax;
      GLfloat ay;
      GLfloat az;

      string   id;
      double   last_run_time; // used to determine whether or not to run func

//      vector<string> connected; // list of objects this object is connected to
      // this function describes the objects motion as a function of time.
      // It will be used by mechanizm to get the objects position.
      // the reference passed to it is a reference to this very object.
      Function func_Motion;

      Object();
      Object(
            string identity,
            GLfloat mass_d,
            Function function_Motion);

      void set_cube(GLfloat new_cube[][3]);
      void set_faceIndex(GLfloat new_faceIndex[][4]);

      // cube transformations
      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z);
      void scale_by(GLfloat x, GLfloat y, GLfloat z);

      void setQuantity(string name, GLfloat q);
      GLfloat* getQuantity(string name);// this simply returns what's in the map

      // these functions will try in multiple ways to calculate the quantity
      GLfloat* get_m();
      GLfloat* get_v();
      GLfloat* get_a();
      GLfloat* get_K();
      GLfloat* get_p();

      // these functions do the same but with respect to time.
      GLfloat* get_m_o_t(GLfloat t);
      GLfloat* get_v_o_t(GLfloat t);
      GLfloat* get_a_o_t(GLfloat t);
      GLfloat* get_K_o_t(GLfloat t);
      GLfloat* get_p_o_t(GLfloat t);

   private:

      // constant physics quantities
      // 
      // These are pointers specifically so we can check if they are NULL with
      // constant time.
      //
      // The values set in this can be any physical quantities. The ones known
      // by the get_* functions are listed here. these functions will use
      // whatever values they can find to calculate the desired quantity. They
      // will treat any values defined in this map as constant with time.
      //
      // Values can be set using the setQuantity function, the getQuantity
      // function is used to get copies of the values in the map.
      //
      // Quantities known by the get_* functions:
      //
      //    m        - the mass
      //    v        - the velocity
      //    a        - the acceleration
      //    K        - the kinetic energy
      //    p        - the momentum
      // 
      unordered_map<string, GLfloat*> c_qs;



};

#endif
