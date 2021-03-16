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
#include "ObjectMap.hpp"
#include "Tetrahedron.hpp"

//using namespace std;

struct phys_event {
   string obj_id;
   double init_t;
   GLfloat cx, cy, cz;
   GLfloat ax, ay, az;
   GLfloat dx, dy, dz;
};

class Object {

   public:

      typedef tools::Error (*Function)(double, Object&);

      vector<phys_event> events;

      string shape = "tetrahedron";

      int vertices = 8;
      GLfloat cube[8][3];
      int faceIndex[6][4];
		GLfloat faceColors[6][3];

      Tetrahedron tetra;

      GLfloat ax;
      GLfloat ay;
      GLfloat az;

      string id;
      string gid;
      void   *group;
      double last_t; // used to integrate cube transformations using different
                     // motion functions

      bool om_tracking = false;
      string last_om_key = "none";

      //vector<string> connected; // list of objects this object is connected to
      // this function describes the objects motion as a function of time.
      // It will be used by mechanizm to get the objects position.
      // the reference passed to it is a reference to this very object.
      Function function;

      Object();
      Object(string identity);
      Object(string identity, GLfloat mass);
      Object(string identity, GLfloat mass, Function func);
      void initialize(string identity, Function func);

      void set_cube(GLfloat new_cube[][3]);
      void set_faceIndex(GLfloat new_faceIndex[][4]);

      string getJSON();

      // cube transformations
      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void translate_by(GLfloat add_point[3]);
      void translate_part(size_t tg_i, unsigned long work,
                                  GLfloat x, GLfloat y, GLfloat z);
      void translate_part(size_t tg_i, unsigned long work, GLfloat add_vert);
      void multiply_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z);
      void scale_by(GLfloat x, GLfloat y, GLfloat z);

      // getters and setters for the constant quantities vector, c_qs
      //
      // Given a scalar quantity this will place the GLfloat q in the first
      // element of the vector c_qs[name].
      //
      void setConstQ(string name, GLfloat q);
      void setCQ(string name, GLfloat q);

      void setConstQ(string name, vector<GLfloat> q);
      void setCQ(string name, vector<GLfloat> q);

      vector<GLfloat> getCQ(string name);
      vector<GLfloat> getConstQ(string name);

      // these functions will try in multiple ways to calculate the quantity
      // they return false if the quantity cannot be calculated
      bool get_m(GLfloat &m);           // mass
      bool get_v(vector<GLfloat> &v);   // velocity
      bool get_vi(vector<GLfloat> &vi); // initial velocity
      bool get_a(vector<GLfloat> &a);   // acceleration
      bool get_K(GLfloat &K);           // kinetic energy
      bool get_U(GLfloat &U);           // potential energy
      bool get_k(GLfloat &k);           // force constant
      bool get_p(vector<GLfloat> &p);   // momentum

      GLfloat magnitude(vector<GLfloat> q);

      tools::Error enable_physics();
      tools::Error disable_physics();
      //tools::Error addevent(string oid, double init_t,
                            //GLfloat cx, cy, cz,
                            //GLfloat ax, ay, az,
                            //GLfloat dx, dy, dz);

      //tools::Error tryhard_motion(double t, Object &self);

      //// these functions do the same but with respect to time.
      //GLfloat* get_v_o_t(GLfloat t);
      //GLfloat* get_a_o_t(GLfloat t);
      //GLfloat* get_K_o_t(GLfloat t);
      //GLfloat* get_p_o_t(GLfloat t);

      // constant physics quantities
      // 
      // The values set in this can be any physical quantities. The ones known
      // by the get_* functions are listed here. these functions will use
      // whatever values they can find to calculate the desired quantity. They
      // will treat any values defined in this map as constant with time.
      //
      // Values can be set using the setQuantity function, the getQuantity
      // function is used to get copies of the values in the map.
      //
      // scalar quantities will be placed in the 0 element of the vector,
      // vector quantities will fill whatever space they need
      //
      // Quantities known by the get_* functions:
      //
      //    m        - mass
      //    v        - velocity
      //    a        - acceleration
      //    K        - kinetic energy
      //    U        - potential energy
      //    p        - momentum
      //    k        - force constant
      //    us       - coefficient of static friction
      //    uk       - coefficient of kinetic friction
      //    ipos     - initial position
      // 
      unordered_map<string, vector<GLfloat>> c_qs;
};

#endif
