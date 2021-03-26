//
// Group.cpp
//
// Created by Daniel Kozitza
//

#include "Group.hpp"
#include "tools.hpp"
using namespace tools;

Group::Group() {
   Object init_obj("init_g_object_0");
   id = "init_gid";
   initialize(init_obj);
}

Group::Group(string nid, Object& obj) {
   id = nid;
   initialize(obj);
}

//Group::Group(const Group& other) {
//   *this = other;
//}

void Group::initialize(Object& obj) {
   objs.clear();
   vis_objs.clear();
   // make sure glob objects are tracked in ObjectMap
   obj.om_tracking = true;
   obj.gid = id;
   objs[obj.id] = obj;
   vis_objs.push_back(obj.id);
   voit = vis_objs.begin();
}

void Group::set_vis_objs(vector<string>& new_vis_objs) {
   vis_objs.resize(new_vis_objs.size());
   for (int voi = 0; voi < new_vis_objs.size(); voi++) {
      vis_objs[voi] = new_vis_objs[voi];
   }
}

void Group::remove_vis_obj(string obj_id) {
   vector<string> nvos;
   for (int voi = 0; voi < vis_objs.size(); voi++) {
      if (vis_objs[voi] != obj_id) {
         nvos.push_back(vis_objs[voi]);
      }
   }
   set_vis_objs(nvos);
   return;
}
void Group::add_vis_obj(string obj_id) {
   vis_objs.push_back(obj_id);
   return;
}

string Group::getJSON() {
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

Vertex Group::center() {
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

tools::Error Group::attach(
      string source_id, int source_face, Object& new_obj) {

   if (verts_within_eps(objs[source_id].tetra.center(), new_obj.tetra.center(),
       (GLfloat)0.05)) {
      cout << "ERROR!!! new object intersects source object\n";
      return error("ERROR!!! new object intersects source object\n");
   }

   objs[new_obj.id] = new_obj;
   vis_objs.push_back(new_obj.id);

   tools::Error n = attach_interior_sides(new_obj.id);
   if (n != NULL) {
      return tools::error("Group::attach::" + string(n));
   }

   new_obj.om_tracking = true;
   new_obj.gid = id;
   new_obj.last_om_key = om_get_section_key(new_obj.tetra.center());
   om_add_obj(new_obj.gid, new_obj.id, new_obj.last_om_key);

   return n;
}

tools::Error Group::attach_interior_sides(string new_obj_id) {

   for (int voit = 0; voit < vis_objs.size(); voit++) {

      if (vis_objs[voit] == new_obj_id) continue;
      string objid = vis_objs[voit];
      vector<int> remove_vfs;

      if (verts_within_eps(objs[objid].tetra.center(),
               objs[new_obj_id].tetra.center(), (GLfloat)0.08)) {
         //cout << "ERROR!!! new object intersects existing object\n";

         objs[new_obj_id].tetra.vis_faces.resize(0);
         objs[new_obj_id].tetra.set_vis_faces(objs[new_obj_id].tetra.vis_faces);
         auto it = objs.find(new_obj_id);
         objs.erase(it);
         remove_vis_obj(new_obj_id);

         // TODO: fix unwanted remaining visible sides
         // mark 'existing object' green
         objs[objid].tetra.set_faceColors(Tetrahedron().fc_green);

         return errorf("attach_interior_sides: removed new object %s: intersects existing object (green) %s", new_obj_id.c_str(), objid.c_str());
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

            //if (objs[objid].tetra.vis_fbools[ofi] == true) {
               remove_vfs.push_back(ovfi);
            //}
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

tools::Error Group::detach(string oid) {
   // loop through 'oid' faces
   for (int fi = 0; fi < 4; fi++) {

      int objid2_fi = fi;
      if (fi == 2) { objid2_fi = 3; }
      if (fi == 3) { objid2_fi = 2; }

      if (objs[oid].tetra.vis_fbools[fi] == true) { continue; }

      for (auto it = objs.begin(); it != objs.end(); it++) {
         //it->second.tetra.rotate_abt_vert(v, ax, ay, az);
         string objid2 = it->first;

         if (oid == objid2) { continue; }
         if (objs[objid2].tetra.vis_fbools[objid2_fi] == true) { continue; }
         
         Vertex oid_c, objid2_c;
         objs[oid].tetra.face[fi].center(oid_c);
         objs[objid2].tetra.face[objid2_fi].center(objid2_c);
         if (verts_within_eps(
                  oid_c,
                  objid2_c,
                  0.08)) {

            // make objs[objid2].face[objid2_fi] visible
            if (objs[objid2].tetra.vis_faces.size() == 0) {
               add_vis_obj(objid2);
            }
            objs[objid2].tetra.add_vis_face(objid2_fi);
            break;
         }
      }
   }
   // remove all traces of oid
   remove_vis_obj(oid);
   om_remove_obj(id, oid, objs[oid].last_om_key);
   auto it = objs.find(oid);
   objs.erase(it);

   return NULL;
}

//void Group::rotate_abt_vert(
//      Vertex& vert, GLfloat AX, GLfloat AY, GLfloat AZ) {
//   AXsum += AX;
//   AYsum += AY;
//   AZsum += AZ;
//
//   for (int pi = 0; pi < vertices; pi++) {
//      points[pi].rotate_about(vert, AX, AY, AZ);
//   }
//}

//void Group::rotate_abt_zero(GLfloat AX, GLfloat AY, GLfloat AZ) {
//   AXsum += AX;
//   AYsum += AY;
//   AZsum += AZ;
//   rotate_abt_zero_ns(AX, AY, AZ);
//}

//void Group::rotate_abt_zero_ns(GLfloat AX, GLfloat AY, GLfloat AZ) {
//   for (int pi = 1; pi < vertices; pi++) {
//      points[pi].rotate_about(points[0], AX, AY, AZ);
//   }
//}

void Group::translate(GLfloat x, GLfloat y, GLfloat z) {
   for (auto it = objs.begin(); it != objs.end(); it++) {
      it->second.translate(x, y, z);
   }
}

void Group::rotate_abt_center(GLfloat ax, GLfloat ay, GLfloat az) {
   Vertex v = center();
   for (auto it = objs.begin(); it != objs.end(); it++) {
      it->second.rotate_abt_vert(v, ax, ay, az);
   }
   return;
}
