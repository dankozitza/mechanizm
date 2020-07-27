// 
// Triangle.cpp
//
// Created by Daniel Kozitza
//

#include "Triangle.hpp"

Vertex init_tri_tetra[4] = {
   {0.0, 0.0, 0.0},
   {1.0, 0.0, 0.0},
   {0.5, 0.0, 0.5},
   {0.5, 0.5, 0.5}};

int init_tri_faceIndex[4][3] = {
   0, 1, 2,
   0, 1, 3,
   0, 2, 3,
   1, 2, 3};

Triangle::Triangle() {
   set_points(init_tri_tetra, init_tri_faceIndex, 0);
}

Triangle::Triangle(Vertex new_points[4]) {
   set_points(new_points, init_tri_faceIndex, 0);
}

void Triangle::set_points(
      Vertex new_points[4], int faceIndex[][3], int fi) {

   facei = fi;
   for (int pi = 0; pi < verts; pi++) {

      //pnts[pi] = new_points[faceIndex[fi][pi]];

      int npi = faceIndex[fi][pi];
      pnts[pi][0] = &new_points[npi][0];
      pnts[pi][1] = &new_points[npi][1];
      pnts[pi][2] = &new_points[npi][2];
   } 

}

void Triangle::translate_by(GLfloat x, GLfloat y, GLfloat z) {
   for (int i = 0; i < verts; ++i) {
      *pnts[i][0] += x;
      *pnts[i][1] += y;
      *pnts[i][2] += z;
   }
}

void Triangle::translate_by(GLfloat add_pnts[3]) {
   for (int i = 0; i < verts; ++i) {
      *pnts[i][0] += add_pnts[0];
      *pnts[i][1] += add_pnts[1];
      *pnts[i][2] += add_pnts[2];
   }
}

//void Triangle::multiply_by(GLfloat x, GLfloat y, GLfloat z) {
//   for (int i = 0; i < verts; ++i) {
//      points[i][0] *= x;
//      points[i][1] *= y;
//      points[i][2] *= z;
//   }
//}
//
//void Triangle::multiply_vert_by(int vertex_index, GLfloat x, GLfloat y, GLfloat z) {
//   points[vertex_index][0] *= x;
//   points[vertex_index][1] *= y;
//   points[vertex_index][2] *= z;
//}

void Triangle::center(Vertex& c) {
   c[0] = (*pnts[0][0] + *pnts[1][0] + *pnts[2][0]) / 3.0;
   c[1] = (*pnts[0][1] + *pnts[1][1] + *pnts[2][1]) / 3.0;
   c[2] = (*pnts[0][2] + *pnts[1][2] + *pnts[2][2]) / 3.0;
}
