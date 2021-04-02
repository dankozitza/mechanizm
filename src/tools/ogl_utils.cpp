//
// ogl_utils.cpp
//

#include <GL/glut.h>
#include <math.h>
#include "../tools.hpp"

#define EPSILON 0.000001f

Vertex tools::to_vert(vector<GLfloat> cmpnts) {
   Vertex r(0.0, 0.0, 0.0);
   if (cmpnts.size() != 3) {return r;}
   r.x = cmpnts[0];
   r.y = cmpnts[1];
   r.z = cmpnts[2];
   return r;
}

bool tools::line_intersects_triangle(
    Vertex line[2],
    Vertex tri[3],
    Vertex * point
) {
    Vertex e0 = tri[1] - tri[0];
    Vertex e1 = tri[2] - tri[0];

    Vertex dir = line[1] - line[0];
    Vertex dir_norm = dir.normalize();

    Vertex h = dir_norm.cross(e1);
    const float a = e0.dot(h);

    if (a > -EPSILON && a < EPSILON) {
        return false;
    }

    Vertex s = line[0] - tri[0];
    const float f = 1.0f / a;
    const float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Vertex q = s.cross(e0);
    const float v = f * dir_norm.dot(q);

    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }

    const float t = f * e1.dot(q);

    if (t > EPSILON && t < sqrtf(dir.dot(dir))) { // segment intersection
        if (point) {
            *point = line[0] + dir_norm * t;
        }

        return true;
    }

    return false;
}

bool tools::verts_within_eps(Vertex v1, Vertex v2, GLfloat eps) {
   if (abs(v1.x - v2.x) < eps &&
       abs(v1.y - v2.y) < eps &&
       abs(v1.z - v2.z) < eps) {
      return true;
   }
   return false;
}

GLfloat tools::brtns(string sbrightness, string sinput) {
   return brtns(as_double(sbrightness), as_double(sinput));
}
GLfloat tools::brtns(string sbrightness, GLfloat input) {
   return brtns(as_double(sbrightness), input);
}
GLfloat tools::brtns(GLfloat brightness, GLfloat input) {
   GLfloat nf = input;

   if (nf + brightness < 1.0) {
      return nf + brightness;
   }
   return nf;
}
