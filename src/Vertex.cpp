// 
// Vertex.cpp
//

//#include <stdio.h>
#include "Vertex.hpp"
//#include "tools.hpp"

GLfloat init_vertex[3] = {0.0, 0.0, 0.0};

Vertex::Vertex() {
   Vertex::x = init_vertex[0];
   Vertex::y = init_vertex[1];
   Vertex::z = init_vertex[2];
}

Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z) {
   Vertex::x = x;
   Vertex::y = y;
   Vertex::z = z;
}

Vertex::Vertex(GLfloat x) {
   Vertex::x = x;
   Vertex::y = x;
   Vertex::z = x;
}

Vertex::~Vertex() {
}

GLfloat Vertex::dot(const Vertex & b) {
   return Vertex::x * b.x + Vertex::y * b.y + Vertex::z * b.z;
}

Vertex Vertex::cross(const Vertex & b) {
   return Vertex(
      Vertex::y * b.z - Vertex::z * b.y,
      Vertex::z * b.x - Vertex::x * b.z,
      Vertex::x * b.y - Vertex::y * b.x
   );
}

Vertex Vertex::normalize() {
   const GLfloat s = 1.0f / sqrtf(Vertex::x * Vertex::x + Vertex::y * Vertex::y + Vertex::z * Vertex::z);
   return Vertex(Vertex::x * s, Vertex::y * s, Vertex::z * s);
}

void Vertex::rotate_about(
      Vertex origin_point,
      GLfloat AX, GLfloat AY, GLfloat AZ) {
   Vertex tmp;

   if (AX != 0.0) {
      tmp.y = y - origin_point.y;
      tmp.z = z - origin_point.z;

      y = (tmp.y * cos(AX)) - (tmp.z * sin(AX)) + origin_point.y;
      z = (tmp.y * sin(AX)) + (tmp.z * cos(AX)) + origin_point.z;
   }

   if (AY != 0.0) {
      tmp.z = z - origin_point.z;
      tmp.x = x - origin_point.x;

      z = (tmp.z * cos(AY)) - (tmp.x * sin(AY)) + origin_point.z;
      x = (tmp.z * sin(AY)) + (tmp.x * cos(AY)) + origin_point.x;
   }

   if (AZ != 0.0) {
      tmp.x = x - origin_point.x;
      tmp.y = y - origin_point.y;

      x = (tmp.x * cos(AZ)) - (tmp.y * sin(AZ)) + origin_point.x;
      y = (tmp.y * cos(AZ)) + (tmp.x * sin(AZ)) + origin_point.y;
   }
}

GLfloat& Vertex::operator[](int index) {
   if (index == 0) return x;
   if (index == 1) return y;
   if (index == 2) return z;

   //cout << "Vertex::operator[]: index '" << index << "' is out of bounds\n";
   //exit(1);
}

Vertex Vertex::operator+(const Vertex & b) {
   return Vertex(
      Vertex::x + b.x,
      Vertex::y + b.y,
      Vertex::z + b.z
   );
}
Vertex Vertex::operator+=(const Vertex & b) {
   *this = Vertex::operator+(b);
   return *this;
}

Vertex Vertex::operator-(const Vertex & b) {
   return Vertex(
      Vertex::x - b.x,
      Vertex::y - b.y,
      Vertex::z - b.z
   );
}
Vertex Vertex::operator-=(const Vertex & b) {
   *this = Vertex::operator-(b);
   return *this;
}

Vertex Vertex::operator*(const Vertex & b) {
   return Vertex(
      Vertex::x * b.x,
      Vertex::y * b.y,
      Vertex::z * b.z
   );
}
Vertex Vertex::operator*=(const Vertex & b) {
   *this = Vertex::operator*(b);
   return *this;
}
Vertex Vertex::operator*(GLfloat b) {
   return Vertex(
      Vertex::x * b,
      Vertex::y * b,
      Vertex::z * b
   );
}
Vertex Vertex::operator*=(GLfloat b) {
   *this = Vertex::operator*(b);
   return *this;
}

Vertex Vertex::operator/(const Vertex & b) {
   return Vertex(
      Vertex::x / b.x,
      Vertex::y / b.y,
      Vertex::z / b.z
   );
}
Vertex Vertex::operator/=(const Vertex & b) {
   *this = Vertex::operator/(b);
   return *this;
}
Vertex Vertex::operator/(GLfloat b) {
   return Vertex(
      Vertex::x * b,
      Vertex::y * b,
      Vertex::z * b
   );
}
Vertex Vertex::operator/=(GLfloat b) {
   *this = Vertex::operator/(b);
   return *this;
}

//void Vertex::multiply_vert_by(int vertex_index, GLGLfloat x, GLGLfloat y, GLGLfloat z) {
//   points[vertex_index][0] *= x;
//   points[vertex_index][1] *= y;
//   points[vertex_index][2] *= z;
//}

