// 
// Group.hpp
//
// A group of objects that are attached physically to eachother.
// Group contains a map of Objects, 'objs',  and metadata on those objects.
//
// Created by Daniel Kozitza
//

#ifndef _MECH_GROUP
#define _MECH_GROUP

#include <GL/glut.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "tools.hpp"
#include "Vertex.hpp"
#include "Triangle.hpp"
#include "Object.hpp"

class Group {

   public:

      Object *phys_obj = NULL;

      string id;
      unordered_map<string, Object> objs;
      vector<string> vis_objs;
      vector<string>::iterator voit;// = vis_objs.begin();

      Group();
      Group(string nid, Object& obj);

      void initialize(Object& init_obj);
      void set_vis_objs(vector<string>& new_vis_objs);
      void remove_vis_obj(string obj_id);
      void add_vis_obj(string obj_id);
      string getJSON();

      Vertex center();
      tools::Error attach(
            string source_id, int source_face,
            Object& new_obj);
      tools::Error detach(string id);

      // TODO: void move_to(GLfloat x, GLfloat y, GLfloat z);
      void translate(GLfloat x, GLfloat y, GLfloat z);
      void rotate_abt_center(GLfloat ax, GLfloat ay, GLfloat az);

      // loop through visible triangles and attach all that are within
      // some distance of eachother
      tools::Error attach_interior_sides(string new_obj_id);
};

#endif
