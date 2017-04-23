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
#include <iomanip>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
//#include "commands.hpp"
#include "mechanizm.hpp"
#include "options.hpp"
#include "tools.hpp"

#include <GL/glu.h>
#include <GL/glut.h>
#include <glm/core/type_vec.hpp>

#include "Camera.hpp"
#include "Object.hpp"
#include "Side.hpp"
#include "Block.hpp"
#include "MapSection.hpp"
#include "Map.hpp"


using namespace tools;

void help(string prog_name);

void
   drawScene(void),
   setMatrix(int w, int h),
   map_generation(void),
   animation(void),
   hot_pause(void),
   setup_glut_menu(),
   menu(int choice),
   resize(int w, int h),
   mouse(int button, int state, int x, int y),
   mouse_passive(int x, int y),
   keyboard(unsigned char c, int x, int y),
   keyboard_up(unsigned char c, int x, int y),
   motion(int x, int y),
   drawWireframe(int objs_index, int face),
   drawFilled(int objs_index, int face);

tools::Error random_motion(double t, Object &self);
tools::Error random_motion_2(double t, Object &self);
tools::Error spawned_motion(double t, Object &self);
tools::Error tryhard_motion(double t, Object &self);

static vector<Object> objs;
static int selected_object = 0;
static int menu_depth = 0;
static string menu_input;
static vector<GLfloat*> terrain;
int block_count = 1;

// X Y Z theta (ax) psi (ay) rotationSpeed translationSpeed
static Camera cam(2.0, 2.0, 3.5, 0.0, 0.0, 0.6, 0.0022);

static mechanizm mech;
int count = 1; // cycles through 1/60th of a second

Map MAP;
//Q<Side> visible_sides;
Q<MapSection> CMSQ;
Q<Map::PidSid> PIDSIDQ;

// menu options
bool MAP_GEN = false;
bool ANIMATION = false;

// commands for the in-game cli
void cmd_select(vector<string>& argv);

int main(int argc, char *argv[]) {

   string prog_name = string(argv[0]);
   options opt;
   bool quiet      = false;
   bool show_help  = false;
   bool file_given = false;
   bool realtime   = false;
   bool time       = false;
   string file_path;
   string time_int_str;
   string m[5];
   vector<string> args;
   Error e = NULL;

   mech.set_objects(objs);
   mech.current_time = 1000.0;

   signal(SIGINT, signals_callback_handler);

   opt.handle('q', quiet);
   opt.handle('h', show_help);
   opt.handle('f', file_given, file_path);
   opt.handle('r', realtime);
   opt.handle('t', time, time_int_str);

   for (int i = 1; i < argc; ++i)
      args.push_back(argv[i]);

   opt.evaluate(args);

   if (pmatches(m, prog_name, "^.*/([^/]+)$"))
      prog_name = m[1];

   if (show_help) {
      help(prog_name);
      return 0;
   }

   // set up the in game command line
   vector<string> cmd_argv = {"help"};
   //commands cmds;

   //cmds.set_max_line_width(80);
   //cmds.set_cmds_help("\nThis is the in-game command line interface.\n\n");

   //cmds.handle(
   //      "select",
   //      cmd_select,
   //      "Select an object by index.",
   //      "select [index]");

   Object test_object_1("test_object_1", 1, NULL);
   objs.push_back(test_object_1);

   // build the map around origin
   MAP.load_section_list();
   //MAP.update(0.0, 0.0, 0.0, CMSQ);

   cout << "MAP.ms.size(): " << MAP.ms.size() << "\n";
   MapSection* ms = MAP.ms.get_item_ptr();

   //for (int z = 0; z < MAP.ms.size(); ++z) { // loop through map sections
   //   cout << ms->sid[0] << " " << ms->sid[1] << " " << ms->sid[2] << "\n";
   //   ms->populate_visible_sides(visible_sides, cam);
   //   MAP.ms.increment_item_ptr();
   //   ms = MAP.ms.get_item_ptr();
   //}

   int fake = 0;
   char** fake2;
   glutInit(&fake, fake2);

   glutInitWindowSize(1080, 800);
   glutInitDisplayMode(GLUT_RGB | GLUT_STENCIL | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Mechanizm");

   glutIdleFunc(hot_pause);
   glutDisplayFunc(drawScene);
   //glutMouseFunc(mouse);
   glutPassiveMotionFunc(mouse_passive);
   glutReshapeFunc(resize);
   glutCreateMenu(menu);
   setup_glut_menu();
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboard_up);
   glutMainLoop();

   return 0;
}

void cmd_select(vector<string>& argv) {
   cout << "you ran select!\n";
}

// a pointer to this function placed in the test_object_1 object. It sets the
// current position as a function of time.
tools::Error random_motion(double t, Object &self) {

   GLfloat x = -0.007;
   if (rand() % 2) {
      x = 0.007;
   }
   GLfloat y = -0.007;
   if (rand() % 2) {
      y = 0.007;
   }
   GLfloat z = -0.007;
   if (rand() % 2) {
      z = 0.007;
   }

   self.translate_by(
         x,
         y,
         z);

   return NULL;
}

tools::Error random_motion_2(double t, Object &self) {

   if (self.c_qs["a"].size() != 3)
      self.setConstQ("a", {
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 220,
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 220,
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 220});


   Object tmp;
   if (self.c_qs.count("vi"))
      tmp.setConstQ("vi", self.c_qs["vi"]);
   else
      tmp.setConstQ("vi", 0.0);
   tmp.setConstQ("a", {
      self.c_qs["a"][0] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190,
      self.c_qs["a"][1] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190,
      self.c_qs["a"][2] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190});

//            (GLfloat) (rand() % 3 - 1) / (GLfloat) 10,
//            (GLfloat) (rand() % 3 - 1) / (GLfloat) 10,
//            (GLfloat) (rand() % 3 - 1) / (GLfloat) 10});

   Object tmp_2 = tmp;
   tryhard_motion(t, tmp);
   tryhard_motion(self.last_t, tmp_2);

   tmp_2.multiply_by(-1, -1, -1);
   tmp.translate_by(tmp_2.cube[0][0], tmp_2.cube[0][1], tmp_2.cube[0][2]);

   self.translate_by(tmp.cube[0][0], tmp.cube[0][1], tmp.cube[0][2]);

   self.last_t = t;
}

//tools::Error random_motion_3(double t, Object &self) {
//   if (self.c_qs["v"].size() != 3)
//      self.setConstQ("v", {
//            (GLfloat) (rand() % 17 - 8) / (GLfloat) 180,
//            (GLfloat) (rand() % 17 - 8) / (GLfloat) 180,
//            (GLfloat) (rand() % 17 - 8) / (GLfloat) 180});
//
//
//   Object tmp;
//   tmp.setConstQ("v", {
//            self.c_qs["v"][0] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 150,
//            self.c_qs["v"][1] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 150,
//            self.c_qs["v"][2] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 150});
//
//   Object tmp_2 = tmp;
//   tryhard_motion(t, tmp);
//   tryhard_motion(self.last_t, tmp_2);
//
//   tmp_2.multiply_by(-1, -1, -1);
//   tmp.translate_by(tmp_2.cube[0][0], tmp_2.cube[0][1], tmp_2.cube[0][2]);
//
//   self.translate_by(tmp.cube[0][0], tmp.cube[0][1], tmp.cube[0][2]);
//
////   cout << "t: " << t << "\n   ";
////   cout << self.id << ": x: " << self.cube[0][0];
////   cout << " y: " << self.cube[0][1];
////   cout << " z: " << self.cube[0][2] << endl;
//
//   self.last_t = t;
//   return NULL;
//}

tools::Error spawned_motion(double t, Object &self) {

   //if (self.cube[0][1] <= 0.0000001) {
   //   glutIdleFunc(hot_pause);
   //}

   Object tmp;

   //float gt = (float)(glutGet(GLUT_ELAPSED_TIME));
   //cout << "gluttime: " << gt << endl;
   tmp.translate_by(
         25 * t,
         31.3 * t - 4.9 * t * t,
         0.0);

   //self.translate_by(
   //      0.1 * sin(t * 3),
   //      ,
   //      0.1 * cos(t * 3));

   self.set_cube(tmp.cube);

   cout << self.id << ": cube 0 0: " << self.cube[0][0] << endl;
   cout << self.id << ": cube 0 1: " << self.cube[0][1] << endl;
   cout << self.id << ": cube 0 2: " << self.cube[0][2] << endl;
   cout << "t: " << t << endl;

   return NULL;
}

//tools::Error tryhard_motion(double t, Object &self) {
//   vector<GLfloat> posi, v, vi, a; 
//   Object tmp;
//
//   posi = self.c_qs["posi"];
//   vi   = self.c_qs["vi"];
//
//   if (self.get_v(v)) {
//      self.set_cube(tmp.cube);
//      self.translate_by(
//         posi[0] + v[0] * t,
//         posi[1] + v[1] * t,
//         posi[2] + v[2] * t);
//   }
//   else if (self.get_a(a)) {
//      self.set_cube(tmp.cube);
//      self.translate_by(
//         posi[0] + vi[0] * t + a[0] * t * t,
//         posi[1] + vi[1] * t + a[1] * t * t,
//         posi[2] + vi[2] * t + a[2] * t * t);
//   }
//   return NULL;
//}  

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
      glVertex3fv((GLfloat *) objs[j].cube[ objs[j].faceIndex[face][i] ]);
   glEnd();
}

void drawSides() {
   int sidecnt = 0;
   for (int msi = 0; msi < MAP.ms.size(); msi++) {
      for (int vsi = 0; vsi < MAP.ms[msi].visible_sides.size(); vsi++) {

         glBegin(GL_POLYGON);
         glColor3fv(MAP.ms[msi].visible_sides[vsi].color);
         for (int point = 0; point < 4; ++point) {
            glVertex3fv(MAP.ms[msi].visible_sides[vsi].points[point]);
         }
         glEnd();

         sidecnt++;
      }
   }
   if (count == 60) {
      cout << "client::drawSides: Total visible sides rendered: `"
           << sidecnt << "`.\n";
   }
}

void drawFilledTStrip() {
   glBegin(GL_TRIANGLE_STRIP);
//   for (GLFloat i = -10; i <= 10; ++i) {
   glColor3f(1, 0, 0); glVertex3f(0, 2, 0);
   glColor3f(0, 1, 0); glVertex3f(-1, 0, 1);
   glColor3f(0, 0, 1); glVertex3f(1, 0, 1);
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

void renderBitmapString(float x, float y, float z, void *font) {
   glRasterPos3f(x, y, z);
   for (int i = 0; i < menu_input.size(); ++i) {
      glColor3f(1.0, 1.0, 1.0);
      glutBitmapCharacter(font, menu_input[i]);
   }
}

void drawScene(void) {
   int i;

   glClearColor(0.2, 0.4, 0.5, 0.0); // background color
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();

   //// set up camera
   //glViewport(0, 0, 500, 500);

   //cout << "here!\n";
   
   glLoadIdentity();
   cam.translation();
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

   // testing terrain draw
   if (terrain.size() > 2) {
      //glFrontFace(GL_CW);
      
      //glBegin(GL_LINES);
      //glBegin(GL_POINTS);
      //glBegin(GL_POLYGON);
      glBegin(GL_TRIANGLE_STRIP);
      for (int i = 0; i < terrain.size(); ++i) {
         glColor3f(1.0, 1.0, 1.0); glVertex3fv(terrain[i]);
      }
      glEnd();
   }

   drawSides();

   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1, 0, 0); glVertex3f(0, 2, 0);
   glColor3f(0, 1, 0); glVertex3f(-1, 0, 1);
   glColor3f(0, 0, 1); glVertex3f(1, 0, 1);
   glEnd();

//   // draw a simple grid
//   glColor3f(1.0, 1.0, 1.0);
//   glBegin(GL_LINES);
//   for (GLfloat i = -30; i <= 30; i += 0.25) {
//      if (mech.current_time > i + 40.0) {
//      glVertex3f(i, 0, 30); glVertex3f(i, 0, -30);
//      glVertex3f(30, 0, i); glVertex3f(-30, 0, i);
//      }
//   }
//   glEnd();

//   for (int j = 0; j < objs.size(); ++j) {
//      glRotatef(objs[j].ax, 1.0, 0.0, 0.0);
//      glRotatef(-objs[j].ay, 0.0, 1.0, 0.0);
//   }

   /* all the good stuff follows */

   //glEnable(GL_STENCIL_TEST);
   //glClear(GL_STENCIL_BUFFER_BIT);
   //glStencilMask(1);
   //glStencilFunc(GL_ALWAYS, 0, 1);
   //glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
   //glColor3f(1.0, 1.0, 0.0);

   for (int j = 0; j < objs.size(); ++j) {
      for (i = 0; i < 6; i++) {
         //drawWireframe(j, i);
         //glStencilFunc(GL_EQUAL, 0, 1);
         //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

         // set the default color
         //objs[j].faceColors[i][0] = rand() % 100 / 100.0;//0.7;
         //objs[j].faceColors[i][1] = rand() % 100 / 100.0;//0.7;
         //objs[j].faceColors[i][2] = rand() % 100 / 100.0;//0.7;
         glColor3fv(objs[j].faceColors[i]); // box color
         drawFilled(j, i);

//         glStencilFunc(GL_ALWAYS, 0, 1);
//         glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
         glColor3f(0.0, 0.0, 0.0);
         drawWireframe(j, i);
      }
   }

   if (menu_depth != 0) {
      // write character bitmaps into the menu_input raster
      renderBitmapString(
         // these calculate the location in front of the face
         cam.getX() + (cos(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*1.3,
         //cam.getY() + sin(-cam.getAY()) - 0.5,
         (GLfloat) (cam.getY() + sin(-cam.getAY())*1.3 - 0.33),
         cam.getZ() + (sin(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*1.3,
         GLUT_BITMAP_9_BY_15);

      // render the output next
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

   // set the perspective (angle of sight, width/height ratio, clip near, depth)
   gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 100.0);

//   gluLookAt(cam.getX(), cam.getY(), cam.getZ(), 0, 0, 0, 0, 1, 0);


   //glRotatef(cam.getTheta(), 1.0, 0.0, 0.0);
   //glRotatef(cam.getPsi(), 0.0, 1.0, 0.0);
   //glTranslated(-cam.getX(), -cam.getY(), -cam.getZ());

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void map_generation(void) {

   if (count % 7 == 0) {
      MAP.update(cam.getX(), cam.getY(), cam.getZ(), CMSQ);
      //MAP.update_nearby_ms_pointers();
      for (int z = 0; z < MAP.ms.size(); ++z) {
         if (!MAP.ms[z].has_sides)
            MAP.ms[z].generate_visible_sides(cam);

       //  MAP.ms[z].generate_visible_edge_sides(cam);
      }
   }

   if (count == 60) {
      cout << "client::map_generation: cached map section queue has "
           << CMSQ.size() << " sections.\n";
   }

   int fcount = 0;
   // fork and save the map sections with no PIDSID entry
   if (CMSQ.size() >= 128 && PIDSIDQ.size() < 4) {
      for (int u = 0; u < CMSQ.size(); ++u) {

         if (fcount == 16) {
            break;
         }

         int idir;
         for (idir = 0; idir >= 0; ++idir) {
            if (CMSQ[u].fid < idir * 100 + 100) {
               break;
            }
         }

         bool has_pid = false;
         for (int o = PIDSIDQ.size()-1; o >= 0; --o) {
            if (  PIDSIDQ[o].sid[0] == CMSQ[u].sid[0] &&
                  PIDSIDQ[o].sid[1] == CMSQ[u].sid[1] &&
                  PIDSIDQ[o].sid[2] == CMSQ[u].sid[2]    ) {
               has_pid = true;
               break;
            }
         }

         // if no process is saving this map section: fork
         if (!has_pid) {

            MapSection* msp = &CMSQ[u];

            pid_t pID = fork();
            if (pID < 0) {
               cerr << "Map::update: failed to fork!\n";
               exit(1);
            }
            else if (pID == 0) { // child
               msp->save(MAP.map_name);
               _exit(0);
            }
            else { // parent

               // store the process id of the child and the section id
               Map::PidSid tps;
               tps.pid = pID;
               tps.sid[0] = CMSQ[u].sid[0];
               tps.sid[1] = CMSQ[u].sid[1];
               tps.sid[2] = CMSQ[u].sid[2];
               PIDSIDQ.enq(tps);
               fcount++;
            }
         }
      }
   }

   if (PIDSIDQ.size() > 40) {
      cout << "client::map_generation: PIDSIDQ size has reached `"
           << PIDSIDQ.size() << "`! There should not be that many child "
           << "processes!\n";
   }

   // check if any child process is done and remove from cache
   // waiting for more than 4 of these at a time seems to break some and cause
   // the proc/pid dir to remain after waitpid gets an exit status 0 from the
   // child process.
   int ret_cnt = 0;
   if (PIDSIDQ.size() > 0) {
      for (int z = 0; z < PIDSIDQ.size() && ret_cnt < 4;) {
         int exit_status;

         pid_t ws = waitpid(PIDSIDQ[z].pid, &exit_status, WNOHANG);

         if( WIFEXITED(exit_status) ) {
            //cout << "waitpid() exited with signal: Status= "
            //     << WEXITSTATUS(exit_status)
            //     << endl;

            // a process exited, delete the map section and pidsid
            ret_cnt++;

            bool found_data = false;
            for (int x = 0; x < CMSQ.size();) {

               if (  PIDSIDQ[z].sid[0] == CMSQ[x].sid[0] &&
                     PIDSIDQ[z].sid[1] == CMSQ[x].sid[1] &&
                     PIDSIDQ[z].sid[2] == CMSQ[x].sid[2]) {

                  found_data = true;

                  // add this map section to the esm
                  MAP.add_esm_entry(CMSQ[x]);

                  CMSQ.delete_item(x);
                  if (tools::pid_exists(PIDSIDQ[z].pid)) {
                     cout << "client::map_generation: child process `"
                          << PIDSIDQ[z].pid << "` was retrieved but it still "
                          << "exists! Returned exit status: `" << exit_status
                          << "`.\n";
                  }
                  else {
                     PIDSIDQ.delete_item(z);
                  }
                  break;
               }
               else {
                  x++;
               }
            }
            if (!found_data) {
               cout << "client::map_generation: did not find map "
                    << "section (" << PIDSIDQ[z].sid[0] << ", "
                    << PIDSIDQ[z].sid[1] << ", " << PIDSIDQ[z].sid[2]
                    << ") in cached map section queue. process "
                    << PIDSIDQ[z].pid << " was supposed to be using it.\n";
               PIDSIDQ.delete_item(z);
            }
         }
         else {
            z++;
         }
      }
   }
}

void animation(void) {

   tools::Error e = NULL;
   // 1/64th of a second
   e = mech.run(0.015625, 0, 0.015625);
   if (e != NULL) {
      cout << e;
      return;
   }
   //cout << "count: " << count << " t: " << mech.current_time << "\n";
   //glutPostRedisplay();

   if (count == 60) { // make up for the time lost
      e = mech.run(0.046875, 0, 0.046875);
      cout << "last t: " << mech.current_time << "\n";

   }
   if (e != NULL) {
      cout << e;
      return;
   }



}

void hot_pause(void) {
   if (MAP_GEN) {
      map_generation();
   }
   if (ANIMATION) {
      animation();
   }

   glutPostRedisplay();

   count++;
   if (count > 60) {
      count = 0;
   }
}

void setup_glut_menu() {
   while (glutGet(GLUT_MENU_NUM_ITEMS) > 0) {
      glutRemoveMenuItem(1);
   }
   if (MAP_GEN) {
      glutAddMenuEntry("1 - Map Generation", 4);
   }
   else {
      glutAddMenuEntry("0 - Map Generation", 4);
   }
   if (ANIMATION) {
      glutAddMenuEntry("1 - Animation", 3);
   }
   else {
      glutAddMenuEntry("0 - Animation", 3);
   }
//   glutAddMenuEntry("Stencil", 1);
   glutAddMenuEntry("Reset", 2);
}

// mouse right click menu
void menu(int choice) {

   switch (choice) {
   case 4: // Map Generation
      if (MAP_GEN) {
         MAP_GEN = false;
      }
      else {
         MAP_GEN = true;
      }
      //setup_glut_menu();
      break;
   case 3: // Animation
      count = 0;
      if (ANIMATION) {
         ANIMATION = false;
      }
      else {
         ANIMATION = true;
      }
      break;
   case 2: // Reset
      mech.current_time = 0.0;
      for (int i = 0; i < objs.size(); ++i) {
         objs[i].set_cube(Object().cube);
         if (objs[i].c_qs.count("posi")
               && objs[i].c_qs["posi"].size() == 3) {
            objs[i].translate_by(
                  objs[i].c_qs["posi"][0],
                  objs[i].c_qs["posi"][1],
                  objs[i].c_qs["posi"][2]);
         }
         objs[i].last_t = 0.0;
      }

      glutPostRedisplay();
      break;
   case 1:
      glutSetWindowTitle("Stencil Enabled");
      glutPostRedisplay();
      break;
   }

   setup_glut_menu();
}

void menu_1_help() {
   cout << "\n*** Menu level: 1";
   cout << "  -  selected object: " << selected_object;
   cout << "  -  time: " << mech.current_time << "\n";
   cout << "   p -   Print the object information\n";
   cout << "   r -   Set motion function to null.\n";
   cout << "   c -   Randomize the colors of selected object.\n";
   cout << "   e -   Enter command.\n";
   cout << "   h -   Print this help message.\n";
   cout << "   q -   exit menu\n";
   cout << "\n";
}

/* ARGSUSED1 */
void
keyboard(unsigned char c, int x, int y)
{

   string prefix = "-> ";
   for (int i = 0; i < menu_depth; ++i) {
      prefix += "   ";
   }

   if (menu_depth != 0) { // go into menu mode

      if (menu_depth == 37) { // this means we're taking input
         if (c != 13 && c != 8) { // 13 is Enter
            // use a buffer to hold the input while writing the characters
            // to stdout.
            menu_input += c;
            cout << (int)c << "\n";
            //glColor3f(1.0, 1.0, 1.0);
            //glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
         }
         else if (c == 8) { // backspace
            if (menu_input.size() > 0) {
               menu_input.resize(menu_input.size()-1);
            }
         }
         else  {
            cout << selected_object << "# " << menu_input << "\n";
            menu_depth = 1;
            //vector<string> cmd_argv;
            //cmds.run(menu_input, cmd_argv);
         }
         return;
      }

      cout << "\n*** Menu level: " << menu_depth;
        cout << "  -  selected object: " << selected_object;
      cout << "  -  time: " << mech.current_time << "\n";

      // run menu 1 level buttons
      if (c == 'p') {
         for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 3; ++j) {
               cout << objs[selected_object].cube[i][j] << " ";
            }
            cout << "\n";
         }
      }
      if (c == 'r') {
         cout << prefix;
         cout << "reset motion function for object " << selected_object;
         cout << "\n";
         objs[selected_object].func_motion = NULL;
      }
      if (c == 'c') {
         cout << prefix;
         cout << "reset colors for object " << selected_object << "\n";
         for (int i = 0; i < 6; i++) {
            // set the default color
            objs[selected_object].faceColors[i][0] = rand() % 100 / 100.0;
            objs[selected_object].faceColors[i][1] = rand() % 100 / 100.0;
            objs[selected_object].faceColors[i][2] = rand() % 100 / 100.0;
         }
      }
      if (c == 'e') {
         menu_depth = 37;
         menu_input = "";
         cout << prefix << "entering command mode:\n";
      }
      if (c == 'h') {
         menu_1_help();
      }
      if (c == 'q') {
         cout << prefix << "exiting menu\n";
         menu_depth = 0;
      }
      return;
   }

   // gameplay level controls
   Object spawned_test_object;
   if (c == 'h' | c == '?') {
      cout << "\n   Controls:\n";
      cout << "      b -             spawn a cube\n";
      cout << "      v -             spawn a vertice\n";
      cout << "      i j k l y n -   move selected cube\n";
      cout << "      u o -           stretch selected cube\n";
      cout << "      0-9 -           select cube\n";
      cout << "      m -             open menu\n";
   }
   if (c == 'b') {
      string id = "spawned_object_";
      cout << prefix;
      cout << "spawning random object: " << id << objs.size();
        cout << "\n";
      if (objs.size() < 9)
         id.push_back((char)(objs.size() + 48));
      else
         id += "n";
      spawned_test_object.id = id;
      spawned_test_object.setConstQ("m", 1);
      switch (rand() % 3) {
      case 0:
         cout << "1!\n";
         spawned_test_object.func_motion = random_motion;
         break;
      case 1:
         cout << "2!\n";
         spawned_test_object.func_motion = random_motion_2;
         break;
      case 2:
         cout << "2!\n";
         spawned_test_object.func_motion = random_motion_2;
         break;
      }
      for (int i = 0; i < 6; i++) {
         // set the default color
         spawned_test_object.faceColors[i][0] = 0.8;//rand() % 100 / 100.0;
         spawned_test_object.faceColors[i][1] = 0.8;//rand() % 100 / 100.0;
         spawned_test_object.faceColors[i][2] = 0.8;//rand() % 100 / 100.0;
      }
      spawned_test_object.translate_by(
         (GLfloat) \
         (cam.getX() + (cos(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*7.0),
         (GLfloat) \
         (cam.getY() + sin(-cam.getAY())*7.0 - 1.0),
         (GLfloat) \
      (cam.getZ() + (sin(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*7.0));

      spawned_test_object.c_qs["posi"][0] = spawned_test_object.cube[0][0];
      spawned_test_object.c_qs["posi"][1] = spawned_test_object.cube[0][1];
      spawned_test_object.c_qs["posi"][2] = spawned_test_object.cube[0][2];
   }
   if (c == 'v') { // add a vector to the terrain
      GLfloat tmp[3] = {
         (GLfloat) \
         (cam.getX() + (cos(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*2.0),
         (GLfloat) \
         (cam.getY() + sin(-cam.getAY())*2.0),
         (GLfloat) \
         (cam.getZ() + (sin(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*2.0)
      };
      terrain.push_back(tmp);
      cout << prefix << "set vertice at (" << tmp[0] << ", " << tmp[1];
      cout << ", " << tmp[2] << ")\n";
      return;
   }


   switch (c) {
   case 27: // Esc
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
   case 'b':
      objs.push_back(spawned_test_object);
      selected_object = objs.size() - 1;
      break;
   case 'm':
   case '`':
      menu_depth = 1;
      menu_1_help();
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
         cam.pressKey(c);

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
      
