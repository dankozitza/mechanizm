// 
// Tetrahedron.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_TETRAHEDRON
#define _MECH_TETRAHEDRON

#include <GL/glut.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Vertex.hpp"
#include "Triangle.hpp"

class Tetrahedron {

   public:

      int vertices = 4;

      // point 0 and direction sum define orientation
      Vertex points[4];

      GLfloat AXsum = 0.0;
      GLfloat AYsum = 0.0;
      GLfloat AZsum = 0.0;

      int faceIndex[4][3];
      Triangle face[4];
      // vector of faceIndex indices
      vector<int> vis_faces;
      vector<bool> vis_fbools;

      unsigned int highlight_frames = 0;
      string highlight_color = "green";
      unordered_map<string, GLfloat[4][3]> fc;

		GLfloat faceColors[4][3];
      GLfloat fc_red[4][3] = {
         0.4, 0.2, 0.2,
         0.5, 0.2, 0.2,
         0.6, 0.2, 0.2,
         0.7, 0.2, 0.2};
      GLfloat fc_green[4][3] = {
         0.2, 0.4, 0.2,
         0.2, 0.5, 0.2,
         0.2, 0.6, 0.2,
         0.2, 0.7, 0.2};
      GLfloat fc_blue[4][3] = {
         0.2, 0.2, 0.4,
         0.2, 0.2, 0.5,
         0.2, 0.2, 0.6,
         0.2, 0.2, 0.7};

      void initialize();
      void init_triangles();

      Tetrahedron();
      Tetrahedron(const Tetrahedron& other);
      Tetrahedron& operator=(const Tetrahedron& other);

      void set_points(Vertex new_points[4]);
      void set_points(const Vertex new_points[4]);
      void set_faceIndex(const int new_faceIndex[][3]);
      void set_face(const Triangle new_face[4]);
      void set_faceColors(const GLfloat new_faceColors[][3]);
      void set_vis_faces(const vector<int> vvfi);
      void set_vis_faces_all();
      void remove_vis_face(int facei);
      void add_vis_face(int facei);
      void rotate_abt_vert(
            Vertex vert, GLfloat AX, GLfloat AY, GLfloat AZ);
      void rotate_abt_zero(GLfloat AX, GLfloat AY, GLfloat AZ);
      void rotate_abt_zero_ns(GLfloat AX, GLfloat AY, GLfloat AZ);
      Vertex center();
      void center(Vertex& c);
      void translate(GLfloat x, GLfloat y, GLfloat z);
      void translate(GLfloat add_points[3]);
      void multiply_by(GLfloat x, GLfloat y, GLfloat z);
      void multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z);
      std::string getJSON();
};

#endif
