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

   vector<int> newvf(0);
   for (int vfi = 0; vfi < objs[source_id].tetra.vis_faces.size(); vfi++) {
      if (objs[source_id].tetra.vis_faces[vfi] != source_face) {
         newvf.push_back(objs[source_id].tetra.vis_faces[vfi]);
      }
   }
   objs[source_id].tetra.set_vis_faces(newvf);

   int new_obj_face;
   if (source_face == 0) new_obj_face = 0;
   else if (source_face == 1) new_obj_face = 1;
   else if (source_face == 2) new_obj_face = 3;
   else if (source_face == 3) new_obj_face = 2;

   vector<int> novfs;
   for (int nvfi = 0;
            nvfi < new_obj.tetra.vis_faces.size();
            nvfi++) {
      if (new_obj.tetra.vis_faces[nvfi] != new_obj_face) {
         novfs.push_back(new_obj.tetra.vis_faces[nvfi]);
      }
   }
   new_obj.tetra.set_vis_faces(novfs);

   objs[new_obj.id] = new_obj;

   return attach_interior_sides(new_obj.id);
}

tools::Error Glob::attach_interior_sides(string new_obj_id) {

   for (auto objit = objs.begin(); objit != objs.end(); objit++) {

      if (objit->first == new_obj_id) continue;
      string objid = objit->first;
      vector<int> remove_vfs;

      for (int ovfi = 0; ovfi < objs[objid].tetra.vis_faces.size(); ovfi++) {

         int ofi = objs[objid].tetra.vis_faces[ovfi];
         int nfi;

         if (ofi == 0) nfi = 0;
         else if (ofi == 1) nfi = 1;
         else if (ofi == 2) nfi = 3;
         else if (ofi == 3) nfi = 2;

         // get center points of both faces
         Vertex ocenter;
         objs[objid].tetra.face[ofi].center(ocenter);

         Vertex ncenter;
         objs[new_obj_id].tetra.face[nfi].center(ncenter);

         // check distance between centers of faces
         if (verts_within_eps(ocenter, ncenter, (GLfloat)0.05)) {

            // remove faces that are attached and still visible
            if (objs[new_obj_id].tetra.vis_fbools[nfi] == true) {

               objs[new_obj_id].tetra.vis_fbools[nfi] = false;

               vector<int> nnvfs;
               for (int nvfi = 0;
                        nvfi < objs[new_obj_id].tetra.vis_faces.size();
                        nvfi++) {
                  if (objs[new_obj_id].tetra.vis_faces[nvfi] != nfi) {
                     nnvfs.push_back(objs[new_obj_id].tetra.vis_faces[nvfi]);
                  }
               }
               objs[new_obj_id].tetra.set_vis_faces(nnvfs);
            }

            if (objs[objid].tetra.vis_fbools[ofi] == true) {
               remove_vfs.push_back(ovfi);
            }
         }

      } // end ovfi loop

      // rebuild vis_face vectors
      for (int rvfi = 0; rvfi < remove_vfs.size(); rvfi++) {

         int remove_vfi = remove_vfs[rvfi];
         vector<int> novfs;

         for (int tvfi = 0;
            tvfi < objs[objid].tetra.vis_faces.size(); tvfi++) {

            if (tvfi != remove_vfi) {
               novfs.push_back(objs[objid].tetra.vis_faces[tvfi]);
            }
            else {
               objs[objid].tetra.vis_fbools[
                  objs[objid].tetra.vis_faces[tvfi]
               ] = false;
            }
         }
         objs[objid].tetra.set_vis_faces(novfs);
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
