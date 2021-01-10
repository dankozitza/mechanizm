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
   vis_objs.clear();
   objs[obj.id] = obj;
   vis_objs.push_back(obj.id);
}

void Glob::set_vis_objs(vector<string>& new_vis_objs) {
   vis_objs.resize(new_vis_objs.size());
   for (int voi = 0; voi < new_vis_objs.size(); voi++) {
      vis_objs[voi] = new_vis_objs[voi];
   }
}

void Glob::remove_vis_obj(string obj_id) {
   vector<string> nvos;
   for (int voi = 0; voi < vis_objs.size(); voi++) {
      if (vis_objs[voi] != obj_id) {
         nvos.push_back(vis_objs[voi]);
      }
   }
   set_vis_objs(nvos);
}

string Glob::getJSON() {
   string r;
   r = "{\n\"g_id\": \"" + id + "\",\n";

   r += "\"g_vis_objs\": [";
   for (int voi = 0; voi < vis_objs.size(); voi++) {
      r += "\"" + vis_objs[voi] + "\"";

      if (voi != vis_objs.size() - 1) {r += ", ";}
   }
   r += "],\n";

   r += "\"g_objs\": {\n";

   unsigned long int i = 0;
   for (auto it = objs.begin(); it != objs.end(); it++) {
      r += "\"" + it->first + "\": " + it->second.getJSON();

      if (i != objs.size() - 1) {r += ",\n";}
      i++;
   }
   r += "}\n}";

   return r;
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

   if (verts_within_eps(objs[source_id].tetra.center(), new_obj.tetra.center(),
       (GLfloat)0.05)) {
      cout << "ERROR!!! new object intersects source object\n";
      return error("ERROR!!! new object intersects source object\n");
   }

   int new_obj_face = source_face;

   objs[new_obj.id] = new_obj;
   vis_objs.push_back(new_obj.id);

   return attach_interior_sides(new_obj.id);
}

tools::Error Glob::attach_interior_sides(string new_obj_id) {

   for (int voit = 0; voit < vis_objs.size(); voit++) {

      if (vis_objs[voit] == new_obj_id) continue;
      string objid = vis_objs[voit];
      vector<int> remove_vfs;

      if (verts_within_eps(objs[objid].tetra.center(),
               objs[new_obj_id].tetra.center(), (GLfloat)0.08)) {
         //cout << "ERROR!!! new object intersects existing object\n";

         GLfloat red[4][3] = {{0.7, 0.2, 0.2},
            {0.7, 0.2, 0.2},
            {0.7, 0.2, 0.2},
            {0.7, 0.2, 0.2}};
         
         objs[new_obj_id].tetra.set_faceColors(red);
         objs[new_obj_id].tetra.vis_faces.resize(0);
         objs[new_obj_id].tetra.set_vis_faces(objs[new_obj_id].tetra.vis_faces);
         remove_vis_obj(new_obj_id);

         //remove_vis_obj(objid);

         return errorf("attach_interior_sides: new object %s intersects existing object %s", new_obj_id.c_str(), objid.c_str());
      }

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
         if (verts_within_eps(ocenter, ncenter, (GLfloat)0.08)) {

            // remove faces that are attached and still visible
            if (objs[new_obj_id].tetra.vis_fbools[nfi] == true) {

               objs[new_obj_id].tetra.remove_vis_face(nfi);

               if (objs[new_obj_id].tetra.vis_faces.size() == 0) {
                  remove_vis_obj(new_obj_id);
               }
            }

            if (objs[objid].tetra.vis_fbools[ofi] == true) {
               remove_vfs.push_back(ovfi);
            }
         }
      } // end ovfi loop

      // get list of face indices to remove
      vector<int> remove_fis;
      for (int rvfi = 0; rvfi < remove_vfs.size(); rvfi++) {
         remove_fis.push_back(objs[objid].tetra.vis_faces[remove_vfs[rvfi]]);
      }
      // remove visible faces from objid
      for (int rfi = 0; rfi < remove_fis.size(); rfi++) {
         objs[objid].tetra.remove_vis_face(remove_fis[rfi]);
         if (objs[objid].tetra.vis_faces.size() == 0) {
            remove_vis_obj(objid);
         }
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
