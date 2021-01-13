// 
// Tetrahedron.cpp
//
// Created by Daniel Kozitza
//


#include "Tetrahedron.hpp"

Vertex init_v_tetra[4] = {
   {0.0, 0.0, 0.0},
   {1.0, 0.0, 0.0},
   {0.5, 0.0, 0.5},
   {0.5, 0.5, 0.5}};

int init_faceIndex[4][3] = {
   0, 1, 2,
   0, 1, 3,
   0, 2, 3,
   1, 2, 3};

Tetrahedron::Tetrahedron() {
   initialize();
}

Tetrahedron::Tetrahedron(const Tetrahedron& other) {
   *this = other;
}

Tetrahedron& Tetrahedron::operator=(const Tetrahedron& other) {
   AXsum = other.AXsum;
   AYsum = other.AYsum;
   AZsum = other.AZsum;
   vertices = other.vertices;
   set_points(other.points);
   set_faceIndex(other.faceIndex);
   set_face(other.face);
   set_vis_faces(other.vis_faces);
   init_triangles(); // pointers not copied by default constructor
   set_faceColors(other.faceColors);
   return *this;
}

//Tetrahedron& Tetrahedron::operator=(Tetrahedron& other) {
//   vertices = other.vertices;
//   set_points(other.points);
//   set_faceIndex(other.faceIndex);
//   set_face(other.face);
//   init_triangles(); // pointers not copied by default constructor
//   set_faceColors(other.faceColors);
//   total_types = other.total_types;
//   type = other.type;
//   return *this;
//}

void Tetrahedron::initialize() {
   set_faceIndex(init_faceIndex);
   set_points(init_v_tetra);
   init_triangles();
   set_vis_faces_all();
   GLfloat grayness = rand() % 15 + 20;
   for (int i = 0; i < vertices; i++) {
      // mostly blue
      //faceColors[i][0] = rand() % 70 / 100.0;
      //faceColors[i][1] = rand() % 70 / 100.0;
      //faceColors[i][2] = 1.0;
      //random dark
      faceColors[i][0] = grayness / 100.0;
      faceColors[i][1] = grayness / 100.0;
      faceColors[i][2] = grayness / 100.0;
   }
}

// assigns 3 pointers from points to each triangle using the face index
void Tetrahedron::init_triangles() {
   for (int fi = 0; fi < vertices; ++fi) {
      face[fi].set_points(points, faceIndex, fi);
   }
}

//void Tetrahedron::set_points(const GLfloat new_points[][3]) {
//   for (int tpi = 0; tpi < vertices; ++tpi) {
//      for (int ai = 0; ai < 3; ai++) {
//         points[tpi][ai] = new_points[tpi][ai];
//      }
//   }
//}

void Tetrahedron::set_points(Vertex new_points[4]) {
   for (int tpi = 0; tpi < vertices; ++tpi) {
      for (int ai = 0; ai < 3; ai++) {
         points[tpi][ai] = new_points[tpi][ai];
      }
   }
}

void Tetrahedron::set_points(const Vertex new_points[4]) {
   for (int tpi = 0; tpi < vertices; ++tpi) {
      Vertex tmp(new_points[tpi]);
      for (int ai = 0; ai < 3; ai++) {
         points[tpi][ai] = tmp[ai];
      }
   }
}

void Tetrahedron::rotate_abt_vert(
      Vertex& vert, GLfloat AX, GLfloat AY, GLfloat AZ) {
   AXsum += AX;
   AYsum += AY;
   AZsum += AZ;

   for (int pi = 0; pi < vertices; pi++) {
      points[pi].rotate_about(vert, AX, AY, AZ);
   }
}

void Tetrahedron::rotate_abt_zero(GLfloat AX, GLfloat AY, GLfloat AZ) {
   AXsum += AX;
   AYsum += AY;
   AZsum += AZ;
   rotate_abt_zero_ns(AX, AY, AZ);
}

void Tetrahedron::rotate_abt_zero_ns(GLfloat AX, GLfloat AY, GLfloat AZ) {
   for (int pi = 1; pi < vertices; pi++) {
      points[pi].rotate_about(points[0], AX, AY, AZ);
   }
}

void Tetrahedron::set_face(const Triangle new_face[4]) {
   for (int fi = 0; fi < vertices; fi++) {
      face[fi] = new_face[fi];
   }
}

void Tetrahedron::set_vis_faces(const vector<int> vvfi) {
   vis_faces.clear();
   vis_faces.resize(vvfi.size());
   vis_fbools.clear();
   vis_fbools.resize(4, false);
   for (int fi = 0; fi < vvfi.size(); fi++) {
      vis_fbools[vvfi[fi]] = true;
      vis_faces[fi] = vvfi[fi];
   }
}

void Tetrahedron::set_vis_faces_all() {
   vis_faces.clear();
   vis_faces.resize(4);
   vis_fbools.clear();
   vis_fbools.resize(4);
   for (int fi = 0; fi < 4; fi++) {
      vis_fbools[fi] = true;
      vis_faces[fi] = fi;
   }
}

void Tetrahedron::remove_vis_face(int facei) {
   vis_fbools[facei] = false;
   vector<int> nvfs;
   for (int vfi = 0; vfi < vis_faces.size(); vfi++) {
      if (vis_faces[vfi] != facei) {
         nvfs.push_back(vis_faces[vfi]);
      }
   }
   set_vis_faces(nvfs);
   return;
}

void Tetrahedron::add_vis_face(int facei) {
   vis_fbools[facei] = true;
   for (int vfi = 0; vfi < vis_faces.size(); vfi++) {
      // if face is visible done
      if (vis_faces[vfi] == facei) { return; }
   }

   vis_faces.push_back(facei);
   return;
}

void Tetrahedron::set_faceColors(const GLfloat new_faceColors[][3]) {
   for (int fi = 0; fi < vertices; fi++) {
      for (int pi = 0; pi < 3; pi++) {
         faceColors[fi][pi] = new_faceColors[fi][pi];
      }
   }
}

void Tetrahedron::set_faceIndex(const int new_faceIndex[][3]) {
   for (int i = 0; i < vertices; ++i) {
      for (int j = 0; j < 3; ++j) {
         faceIndex[i][j] = new_faceIndex[i][j];
      }
   }
}

Vertex Tetrahedron::center() {
   Vertex tmp;
   center(tmp);
   return tmp;
}

void Tetrahedron::center(Vertex& c) {
   c.x = (points[0].x + points[1].x + points[2].x + points[3].x) / 4.0;
   c.y = (points[0].y + points[1].y + points[2].y + points[3].y) / 4.0;
   c.z = (points[0].z + points[1].z + points[2].z + points[3].z) / 4.0;
}

void Tetrahedron::translate_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      points[i][0] += x;
      points[i][1] += y;
      points[i][2] += z;
   }
}

void Tetrahedron::translate_by(GLfloat add_points[3]) {
   for (int i = 0; i < vertices; ++i) {
      points[i][0] += add_points[0];
      points[i][1] += add_points[1];
      points[i][2] += add_points[2];
   }
}

void Tetrahedron::multiply_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < vertices; ++i) {
      points[i][0] *= x;
      points[i][1] *= y;
      points[i][2] *= z;
   }
}

void Tetrahedron::multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z) {
   points[vertex_index][0] *= x;
   points[vertex_index][1] *= y;
   points[vertex_index][2] *= z;
}

std::string Tetrahedron::getJSON() {
   std::string r;
   char buff[200];
   r = "{\n\"points\": [" + points[0].getJSON() + ", " + points[1].getJSON();
   r += ", " + points[2].getJSON() + ", " + points[3].getJSON() + "],\n";

   r += "\"vis_faces\": [";
   for (int vfi = 0; vfi < vis_faces.size(); vfi++) {
      sprintf(buff, "%i", vis_faces[vfi]);
      r += buff;

      if (vfi != vis_faces.size() - 1) {r += ", ";}
   }
   r += "],\n";

   r += "\"faceColors\": [";
   for (int i = 0; i < 4; i++) {
      sprintf(buff, "[%f, %f, %f]",
            faceColors[i][0], faceColors[i][1], faceColors[i][2]);
      r += buff;

      if (i != 3) {r += ", ";}
   }
   r += "]\n";

   r += "}";
   return r;
}
