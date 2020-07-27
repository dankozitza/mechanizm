// 
// Triangle.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_TRIANGLE
#define _MECH_TRIANGLE

#include <GL/glut.h>
#include "Vertex.hpp"

using namespace std;

class Triangle {

   public:

      int facei = 0;
      int verts = 3;
      GLfloat* pnts[3][3];

      Triangle();
      Triangle(Vertex new_points[4]);

      void set_points(
            Vertex new_points[4], int faceIndex[][3], int fi);
      //void center(GLfloat c[3]);
      //void normal(GLfloat v[][3]);
      //
      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void translate_by(GLfloat add_points[3]);
      //multiply_by(GLfloat x, GLfloat y, GLfloat z);
      //multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat);
      
      void center(Vertex& c);
};

#endif
