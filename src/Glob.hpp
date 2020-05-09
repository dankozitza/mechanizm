// 
// Glob.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_GLOB
#define _MECH_GLOB

#include <GL/glut.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "tools.hpp"
#include "Object.hpp"
#include "Vertex.hpp"
#include "Triangle.hpp"

class Glob {

   public:

      Object::Function function = NULL;

      string id;
      unordered_map<string, Object> objs;
      Vertex center_offset; // calculated on attach/detach
      string init_obj_id;

      Glob();
      Glob(Object& obj);

      void initialize(Object& init_obj);

      Vertex center();
      tools::Error attach(
            string source_id, int source_face,
            Object& new_obj);
      tools::Error detach(string id);

      void translate_by(GLfloat x, GLfloat y, GLfloat z);
      void rotate_abt_center(GLfloat ax, GLfloat ay, GLfloat az);

      // loop through visible triangles and attach all that are within
      // some distance of eachother
      tools::Error attach_interior_sides(string new_obj_id);
};

#endif
