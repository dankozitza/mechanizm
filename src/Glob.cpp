// 
// Glob.cpp
//
// Created by Daniel Kozitza
//

#include "Glob.hpp"

//Vertex init_v_tetra[4] = {
//   {0.0, 0.0, 0.0},
//   {1.0, 0.0, 0.0},
//   {0.5, 0.0, 0.5},
//   {0.5, 0.5, 0.5}};
//
//int init_faceIndex[4][3] = {
//   0, 1, 2,
//   0, 1, 3,
//   0, 2, 3,
//   1, 2, 3};
#include "tools.hpp"
using namespace tools;

Glob::Glob() {
   Object init_obj("init_g_object_0");
   initialize(init_obj);
}

Glob::Glob(Object& obj) {
   initialize(obj);
}

//Glob::Glob(const Glob& other) {
//   *this = other;
//}

void Glob::initialize(Object& obj) {
   objs.clear();
   objs[obj.id] = obj;
   center_offset = obj.tetra.points[0] - obj.tetra.center();
}

Vertex Glob::center() {
   Vertex ret;

   // loop through objects, get average point of all
   // object centers
   // divide by number of objects

   for (auto it = objs.begin(); it != objs.end(); it++) {
      ret += it->second.tetra.center();
   }
   ret.x = ret.x / (GLfloat) objs.size();
   ret.y = ret.y / (GLfloat) objs.size();
   ret.z = ret.z / (GLfloat) objs.size();

   return ret;
}

tools::Error Glob::attach(
      string source_id, int source_face, Object& new_obj) {

   //remove interior faces from vis_faces index
   for (auto it = objs[source_id].tetra.vis_faces.begin();
            it != objs[source_id].tetra.vis_faces.end(); it++) {

      if (*it == source_face) { //objs[source_id].tetra.vis_faces[i] == source_face) {
         objs[source_id].tetra.vis_fbools[*it] = false;
         objs[source_id].tetra.vis_faces.erase(it);
         break;
      }
   }

   int new_obj_face;
   if (source_face == 0) new_obj_face = 0;
   else if (source_face == 1) new_obj_face = 1;
   else if (source_face == 2) new_obj_face = 3;
   else if (source_face == 3) new_obj_face = 2;

   // remove interior faces from vis_faces index
   for (auto it2 = new_obj.tetra.vis_faces.begin();
            it2 != new_obj.tetra.vis_faces.end(); it2++) {

      if (*it2 == new_obj_face) { //new_obj.tetra.vis_faces[i] == new_obj_face) {
         new_obj.tetra.vis_fbools[*it2] = false;
         new_obj.tetra.vis_faces.erase(it2);
         break;
      }
   }

   objs[new_obj.id] = new_obj;

   return attach_interior_sides(new_obj.id);
}

tools::Error Glob::attach_interior_sides(string new_obj_id) {

   // remove vis_faces
   for (auto objit = objs.begin(); objit != objs.end(); objit++) {

      if (objit->first == new_obj_id) continue;

      //cout << "other vis faces:\n";
      //cout << objs[objit->first].tetra.vis_faces.size() << "\n";
      for (auto ovfit = objs[objit->first].tetra.vis_faces.begin();
            ovfit != objs[objit->first].tetra.vis_faces.end(); ovfit++) {

      //for (auto vfit = objs[new_obj_id].tetra.vis_faces.begin();
      //      vfit != objs[new_obj_id].tetra.vis_faces.end(); vfit++) {

         int ofi = objs[objit->first].tetra.vis_faces[*ovfit];
         int nfi;

         if (ofi == 0) nfi = 0;
         else if (ofi == 1) nfi = 1;
         else if (ofi == 2) nfi = 3;
         else if (ofi == 3) nfi = 2;

         string oid = objit->first;

         if (objs[new_obj_id].tetra.vis_fbools[nfi] == true) {
            cout << "new object has side!\n";
         }

         int npi = objs[new_obj_id].tetra.faceIndex[nfi][0];
         int opi = objs[oid].tetra.faceIndex[ofi][0];

         cout << "other object id: " << oid << "\n";
         cout << "other face index:"
              << ofi << ", new face index: " << nfi << "\n";
         cout << "opi: " << opi << ", npi: " << npi << "\n";

         //for (auto vfit = objs[new_obj_id].tetra.vis_faces.begin();
         //   vfit != objs[new_obj_id].tetra.vis_faces.end(); vfit++) {

         //}
      }
   }

   return NULL;
}

//void Glob::rotate_abt_vert(
//      Vertex& vert, GLfloat AX, GLfloat AY, GLfloat AZ) {
//   AXsum += AX;
//   AYsum += AY;
//   AZsum += AZ;
//
//   for (int pi = 0; pi < vertices; pi++) {
//      points[pi].rotate_about(vert, AX, AY, AZ);
//   }
//}

//void Glob::rotate_abt_zero(GLfloat AX, GLfloat AY, GLfloat AZ) {
//   AXsum += AX;
//   AYsum += AY;
//   AZsum += AZ;
//   rotate_abt_zero_ns(AX, AY, AZ);
//}

//void Glob::rotate_abt_zero_ns(GLfloat AX, GLfloat AY, GLfloat AZ) {
//   for (int pi = 1; pi < vertices; pi++) {
//      points[pi].rotate_about(points[0], AX, AY, AZ);
//   }
//}

void Glob::translate_by(GLfloat x, GLfloat y, GLfloat z) {
   for (auto it = objs.begin(); it != objs.end(); it++) {
      it->second.tetra.translate_by(x, y, z);
   }
}

void Glob::rotate_abt_center(GLfloat ax, GLfloat ay, GLfloat az) {
   Vertex v = center();
   for (auto it = objs.begin(); it != objs.end(); it++) {
      it->second.tetra.rotate_abt_vert(v, ax, ay, az);
   }  
}
