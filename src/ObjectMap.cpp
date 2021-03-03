//
// ObjectMap.cpp
//
// Created by Daniel Kozitza
//

#include "ObjectMap.hpp"
#include "tools.hpp"
using namespace tools;

unordered_map<string, vector<oinfo>> OM;
int om_section_size = 5;

unordered_map<string, vector<oinfo>>& M() {
   return OM;
}

void om_add_obj(string glb_id, string obj_id, string om_key) {
   OM[om_key].push_back(oinfo{glb_id, obj_id});
   return;
}

void om_remove_obj(string glb_id, string obj_id, string om_key) {

   if (om_key == "none") {return;}

   for (int i = 0; i < OM[om_key].size(); i++) {
      if (OM[om_key][i].gid == glb_id && OM[om_key][i].oid == obj_id) {
         OM[om_key].erase(OM[om_key].begin() + i);
         break;
      }
   }

   if (OM[om_key].size() == 0) {
      auto it = OM.find(om_key);
      OM.erase(it);
   }
   return;
}

string om_get_section_key(Vertex center) {
   long int loc[3];

   loc[0] = roundf(center.x / (GLfloat) om_section_size) * om_section_size;
   loc[1] = roundf(center.y / (GLfloat) om_section_size) * om_section_size;
   loc[2] = roundf(center.z / (GLfloat) om_section_size) * om_section_size;

   char buf[200];
   sprintf(buf, "%li_%li_%li", loc[0], loc[1], loc[2]);
   return string(buf);
}

void om_get_sections(string center, GLfloat radius,
         vector<string>& sections) {

   return;
}

void om_get_nearby_sections(string center, vector<string>& sections) {

   int delim_1 = -1;
   int delim_2 = -1;

   for (int i = 0; i < center.size(); i++) {
      if (center[i] == '_') {
         if (delim_1 == -1) {
            delim_1 = i;
         }
         else if (delim_2 == -1) {
            delim_2 = i;
            break;
         }
      }
   }

   if (delim_1 == -1 || delim_2 == -1) {
      cout << "om_get_nearby_sections: invalid section key: '"
           << center << "'." << endl;
      return;
   }

   long int x = atol(center.substr(0, delim_1).c_str());
   long int y = atol(center.substr(delim_1+1, delim_2 - delim_1+1).c_str());
   long int z = atol(
                center.substr(delim_2+1, center.size() - delim_2+1).c_str());

   for (long int tx = x - om_section_size;
        tx <= x + om_section_size;
        tx += om_section_size) {
      for (long int ty = y - om_section_size;
           ty <= y + om_section_size;
           ty += om_section_size) {
         for (long int tz = z - om_section_size;
              tz <= z + om_section_size;
              tz += om_section_size) {
            char buf[200];
            sprintf(buf, "%li_%li_%li", tx, ty, tz);

            sections.push_back(string(buf));
         }
      }
   }

   //cout << "om_get_nearby_sections: center: " << x << ", " << y << ", " << z << endl;

   return;
}

string om_getJSON() {
   string r;
   r = "{\n";
   int keycnt = 0;
   for (auto it = OM.begin(); it != OM.end(); it++) {
      r += "\"" + it->first + "\": [\n";

      for (int i = 0; i < it->second.size(); i++) {
         r += "[" + it->second[i].gid + ", " + it->second[i].oid + "]";
         
         if (i != it->second.size() - 1) {r += ",\n";}
         else {r += "\n]";}
      }

      if (keycnt != OM.size() - 1) {
         r += ",\n";
      }
      else {r+= "\n";}

      keycnt++;
   }
   r += "}";
   return r;
}
