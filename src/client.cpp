//
// client.cpp
//
// Frontend for the mechanizm object.
//
// Created by Daniel Kozitza
//

#include <csignal>
#include <cstdlib>
#include <dirent.h>
#include <fstream>
#include <glm/vec3.hpp>
#include <iomanip>
#include <iostream>
#include "mechanizm.hpp"
#include "options.hpp"
#include "tools.hpp"
#include "Object.hpp"

//#include <stdlib.h>
//#include <string.h>
//#include <math.h>

#include <GL/glu.h>
#include <GL/glut.h>


#include "Camera.hpp"

using namespace tools;

void help(string prog_name);

void
   drawScene(void), setMatrix(int w, int h), animation(void),
   resize(int w, int h),
   mouse(int button, int state, int x, int y),
   mouse_passive(int x, int y),
   keyboard(unsigned char c, int x, int y), menu(int choice), 
   keyboard_up(unsigned char c, int x, int y),
   motion(int x, int y),
   drawWireframe(int objs_index, int face),
   drawFilled(int objs_index, int face);

tools::Error to1_motion(double t, Object &self);
tools::Error spawned_motion(double t, Object &self);

static vector<Object> objs;
static int selected_object = 0;

// X Y Z theta (ax) psi (ay) rotationSpeed translationSpeed width height
static Camera cam(2.8, 2.3, 2.5, 0.0, 0.0, 0.8, 0.2, 0.1, 0.1);


static mechanizm mech;

bool animation_on = false;

int main(int argc, char *argv[]) {

   string prog_name = string(argv[0]);
   options opt;
   bool quiet      = false;
   bool show_help  = false;
   bool file_given = false;
   bool realtime   = false;
   string file_path;
   string m[5];
   vector<string> args;
   Error e = NULL;

   mech.set_objects(objs);

   signal(SIGINT, signals_callback_handler);

   opt.handle('q', quiet);
   opt.handle('h', show_help);
   opt.handle('f', file_given, file_path);
   opt.handle('r', realtime);

   for (int i = 1; i < argc; ++i)
      args.push_back(argv[i]);

   opt.evaluate(args);

   if (pmatches(m, prog_name, "^.*/([^/]+)$"))
      prog_name = m[1];

   if (show_help) {
      help(prog_name);
      return 0;
   }

   Object test_object_1("test_object_1", 1, to1_motion);
   objs.push_back(test_object_1);

   int fake = 0;
   char** fake2;
   glutInit(&fake, fake2);

   glutInitWindowSize(800, 600);
   glutInitDisplayMode(GLUT_RGB | GLUT_STENCIL | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Mechanizm");

   glutDisplayFunc(drawScene);
   //glutMouseFunc(mouse);
   glutPassiveMotionFunc(mouse_passive);
   glutReshapeFunc(resize);
   glutCreateMenu(menu);
   glutAddMenuEntry("Motion", 3);
   glutAddMenuEntry("Stencil on", 1);
   glutAddMenuEntry("Stencil off", 2);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboard_up);
   glutMainLoop();


   return 0;
}

// a pointer to this function placed in the test_object_1 object. It sets the
// current position as a function of time.
tools::Error to1_motion(double t, Object &self) {


   Object tmp;
   //float gt = (float)(glutGet(GLUT_ELAPSED_TIME));
   //cout << "gluttime: " << gt << endl;
   self.translate_by(
         0.0,
         sin(t/2) / 5,
         0.0);

   //self.translate_by(
   //      0.1 * sin(t * 3),
   //      ,
   //      0.1 * cos(t * 3));

   //self.set_cube(tmp.cube);

   cout << "t: " << t << endl;
   cout << self.id << ": cube 0 0: " << self.cube[0][0] << endl;
   cout << self.id << ": cube 0 1: " << self.cube[0][1] << endl;
   cout << self.id << ": cube 0 2: " << self.cube[0][2] << endl;
   return NULL;
}

tools::Error spawned_motion(double t, Object &self) {

   //if (self.cube[0][1] <= 0.0000001) {
   //   glutIdleFunc(NULL);
   //}

   Object tmp;

   if (self.cube[0][1] <= -0.0000001) {
      tmp.translate_by(
            -25 / t,
            -31.3 / t + 4.9 / t / t,
            0.0);
   }
   else {

   //float gt = (float)(glutGet(GLUT_ELAPSED_TIME));
   //cout << "gluttime: " << gt << endl;
      tmp.translate_by(
            25 * t,
            31.3 * t - 4.9 * t * t,
            0.0);
   }

   //self.translate_by(
   //      0.1 * sin(t * 3),
   //      ,
   //      0.1 * cos(t * 3));

   self.set_cube(tmp.cube);

   cout << "t: " << t << endl;
   cout << self.id << ": cube 0 0: " << self.cube[0][0] << endl;
   cout << self.id << ": cube 0 1: " << self.cube[0][1] << endl;
   cout << self.id << ": cube 0 2: " << self.cube[0][2] << endl;
   return NULL;
}

void help(string p_name) {
   cout << "\n";
   cout << fold(0, 80, p_name +
      " is a frontend for the mechanizm object.");
   cout << "\n\nUsage:\n\n   " << p_name;
   cout << " [-option]\n\n";
   cout << "Options:\n\n";
   cout << "   r             - Run the simulation in real time.\n";
   cout << "   q             - Quiet mode. Nothing will be printed to stdout.";
   cout << "\n   f <file_name> - Load the objects from the given file name.\n";
   cout << "   h             - Print this information.\n\n";
}

void
drawWireframe(int objs_index, int face)
{
   int i, j = objs_index;
   glBegin(GL_LINE_LOOP);
   for (i = 0; i < 4; i++)
      glVertex3fv((GLfloat *) objs[j].cube[(objs[j].faceIndex[face][i])]);
   glEnd();
}

// This fills the space betwen the lines defined in faceIndex
void
drawFilled(int objs_index, int face)
{
   int i, j = objs_index;
   glBegin(GL_POLYGON);
   for (i = 0; i < 4; i++)
      glVertex3fv((GLfloat *) objs[j].cube[objs[j].faceIndex[face][i]]);
   glEnd();
}

//void mouse(int button, int state, int x, int y) {
//   //cam.rotation(x, y);
//   //cout << "here! ";
////   SDL_SetRelativeMouseMode((SDL_bool) 1);
//   //glutPostRedisplay();
//}

void mouse_passive(int x, int y) {
   cam.rotation(x, y);

//   SDL_SetRelativeMouseMode((SDL_bool) 1);
   glutPostRedisplay();
}


void drawScene(void) {
   int i;

   glClearColor(0.0, 0.0, 0.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();

   //// set up camera
   //glViewport(0, 0, 500, 500);

   //cout << "here!\n";
   
   glLoadIdentity();
   //cam.translation();
   glRotatef(180 * cam.getAY() / 3.141592653, 1.0, 0.0, 0.0);
   glRotatef(180 * cam.getAX() / 3.141592653, 0.0, 1.0, 0.0);
   glTranslated(-cam.getX(), -cam.getY(), -cam.getZ());

   //glRotatef(cam.getTheta(), 1.0, 0.0, 0.0);
   //glRotatef(cam.getPsi(), 0.0, 1.0, 0.0);
   //glTranslated(-cam.getX(), -cam.getY(), -cam.getZ());

   //gluLookAt(cam.getX(), cam.getY(), cam.getZ(), cam.getSightX(),
         //cam.getSightY(), cam.getSightZ(), 0, 1, 0);
   //gluLookAt(cam.getX(), cam.getY(), cam.getZ(), 0.0, 0.0, 0.0, 0, 1, 0);
   //cout << " SX: " << cam.getSightX() << " SY: " << cam.getSightY();
   //cout << " SZ: " << cam.getSightZ() << endl;
   //
  //cout << "X: " << cam.getX() << " Y: " << cam.getY() << " Z: " << cam.getZ();
   //cout << " _AX: " << cam.getAX() << " _AY: " << -cam.getAY() << endl;

   // draw stuff
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);


//   for (int j = 0; j < objs.size(); ++j) {
//      glRotatef(objs[j].ax, 1.0, 0.0, 0.0);
//      glRotatef(-objs[j].ay, 0.0, 1.0, 0.0);
//   }

   /* all the good stuff follows */

   glEnable(GL_STENCIL_TEST);
   glClear(GL_STENCIL_BUFFER_BIT);
   glStencilMask(1);
   glStencilFunc(GL_ALWAYS, 0, 1);
   glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
   glColor3f(1.0, 1.0, 0.0);

   for (int j = 0; j < objs.size(); ++j) {
      for (i = 0; i < 6; i++) {
         drawWireframe(j, i);
         glStencilFunc(GL_EQUAL, 0, 1);
         glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
         glColor3f(0.0, 0.0, 0.0);
         drawFilled(j, i);

         glColor3f(1.0, 1.0, 0.0);
         glStencilFunc(GL_ALWAYS, 0, 1);
         glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
         glColor3f(1.0, 1.0, 0.0);
         drawWireframe(j, i);
      }
   }
   glPopMatrix();

   glDisable(GL_STENCIL_TEST);

   /* end of good stuff */

   glutSwapBuffers();
}

void
setMatrix(int w, int h)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

//  glOrtho(-2.0, 2.0, -2.0, 2.0, -2.0, 2.0);
//

   // set the perspective (angle of sight, width, height, depth)
   gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0);

//   gluLookAt(cam.getX(), cam.getY(), cam.getZ(), 0, 0, 0, 0, 1, 0);


   //glRotatef(cam.getTheta(), 1.0, 0.0, 0.0);
   //glRotatef(cam.getPsi(), 0.0, 1.0, 0.0);
   //glTranslated(-cam.getX(), -cam.getY(), -cam.getZ());

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

int count = 1;

void
animation(void)
{
   tools::Error e = NULL;
   // 1/64th of a second
   e = mech.run(0.015625, 0, 0.015625);
   if (e != NULL) {
      cout << e;
      return;
   }

//   cout << "count: " << count << " t: " << mech.current_time << "\n";

   glutPostRedisplay();

   //if (count == 60) { // make up for the time lost
   if (count == 60) { // make up for the time lost
      e = mech.run(0.046875, 0, 0.046875);
//      cout << "last t: " << mech.current_time << "\n";
   }
   if (e != NULL) {
      cout << e;
      return;
   }

   glutPostRedisplay();

   count++;
   if (count >= 61) {
      count = 0;
//      glutIdleFunc(NULL);
   }
}

void
menu(int choice)
{
   switch (choice) {
   case 3:
      count = 0;
      if (mech.current_time >= 1.2) {
         mech.current_time = 0.0;
      }
      if (animation_on) {
         glutIdleFunc(NULL);
         animation_on = false;
      }
      else {
         glutIdleFunc(animation);
         animation_on = true;
      }
      break;
   case 2:
      glutSetWindowTitle("Stencil Disabled");
      glutPostRedisplay();
      break;
   case 1:
      glutSetWindowTitle("Stencil Enabled");
      glutPostRedisplay();
      break;
  }
}

static int selected_side;

/* ARGSUSED1 */
void
keyboard(unsigned char c, int x, int y)
{
   string id = "spawned_object_";
   for (int i = 0; i < objs.size(); ++i) {
      id += "1";
   }
   Object spawned_test_object(id, 1, spawned_motion);

   switch (c) {
   case 27:
      exit(0);
      break;
   case 93: // ]
      //grow_side(selected_side, 1.0);
      glutPostRedisplay();
      break;
   case 91: // [
      //grow_side(selected_side, -1.0);
      glutPostRedisplay();
      break;
   case 'i':
      objs[selected_object].translate_by(0, 0, -0.1);
      break;
   case 'j':
      objs[selected_object].translate_by(-0.1, 0, 0);
      break;
   case 'k':
      objs[selected_object].translate_by(0, 0, 0.1);
      break;
   case 'l':
      objs[selected_object].translate_by(0.1, 0, 0);
      break;
   case 'y':
      objs[selected_object].translate_by(0, 0.1, 0);
      break;
   case 'n':
      objs[selected_object].translate_by(0, -0.1, 0);
      break;

   case 'o':
      objs[selected_object].scale_by(1.1, 1.1, 1.1);
      break;
   case 'u':
      objs[selected_object].scale_by(0.9, 0.9, 0.9);
      break;
   case 'p':
      objs.push_back(spawned_test_object);
      //mech.spawn(objs[objs.size()-1]);
      selected_object = objs.size() - 1;
      break;

//   case 'c':
//      cam.Move(DOWN);
//      break;
//   case ' ':
//      cam.Move(UP);
//      break;
   default:
      if (c == '1' || c == '2' || c == '3' || c == '4' || c == '5' || c == '0')
         selected_object = (int)c - 48;
      else
         cam.pressKey(c, x, y);

      glutPostRedisplay();

      //std::cout << "got key:" << (int)c << std::endl;
      break;
  }
}

void keyboard_up(unsigned char c, int x, int y) {
   cam.setKeyboard(c, false);
   glutPostRedisplay();
}

void
resize(int w, int h)
{
  glViewport(0, 0, w, h);
  setMatrix(w, h);
}
