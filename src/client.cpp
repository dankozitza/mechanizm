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
#include "commands.hpp"
#include "mechanizm.hpp"
#include "options.hpp"
#include "tools.hpp"

#include <GL/glu.h>
#include <GL/glut.h>
//#include <glm/detail/type_vec4.hpp>

#include "Camera.hpp"
#include "Glob.hpp"
#include "Object.hpp"
#include "Sphere.hpp"
#include "Side.hpp"
#include "Tetrahedron.hpp"
#include "Vertex.hpp"
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
   pricion(void),
   hot_pause(void),
   draw_glut_menu(),
   menu(int choice),
   resize(int w, int h),
   mouse(int button, int state, int x, int y),
   mouse_passive(int x, int y),
   keyboard(unsigned char c, int x, int y),
   keyboard_up(unsigned char c, int x, int y),
   motion(int x, int y),
   drawWireframe(int objs_index, int face),
   drawFilled(int objs_index, int face),
   drawFilledTetrahedron(int objs_index),
   drawVisibleTriangles(Tetrahedron& tetra),
   draw_circle(float cx, float cy, float r, int num_segments),
   draw_cam_spheres(),
   draw_sphere(Sphere& s),
   draw_spheres(),

   reset_sphere_distance();

tools::Error random_motion(double t, Object &self);
tools::Error random_motion_2(double t, Object &self);
tools::Error spawned_motion(double t, Object &self);
tools::Error tryhard_motion(double t, Object &self);

Tetrahedron generate_tetra_from_side(Tetrahedron& source, int source_face);

static vector<Object> objs;
static unordered_map<string, Glob> gs;
static vector<Sphere> sphrs;
static int selected_object = 0;
static string selected_glob;
static long unsigned int gen_obj_cnt = 0;
static string selector_function = "select";
static int draw_x_vertices = 18;
static int menu_depth = 0;
static int timed_menu_display = 0;
static int draw_menu_lines = 10;
static string menu_input;
static vector<string> menu_output;
int block_count = 1;
int mouse_left_state = 1;

// X Y Z theta (ax) psi (ay) rotationSpeed translationSpeed
static Camera cam(2.0, 2.0, 3.5, 0.0, 0.0, 0.6, 0.0022);

static mechanizm mech;
int count = 1; // cycles through 1/60th of a second

commands IN_GAME_CMDS;
map<string, string> CMDS_STORE; // values that can be set in-game
GLfloat SD; // current distance of the selection sphere
bool SD_DONE = true; // tells draw_cam_spheres when to move selector

Map MAP;
//Q<Side> visible_sides;
Q<MapSection> CMSQ;
Q<Map::PidSid> PIDSIDQ;

bool DRAW_GLUT_MENU = false;
// menu options
bool MAP_GEN = false;
bool ANIMATION = false;
bool PRICION = false;

// commands for the in-game cli
void cmd_help(vector<string>& argv);
void cmd_tp(vector<string>& argv);
void cmd_set(vector<string>& argv);
void cmd_select(vector<string>& argv);
void cmd_grow(vector<string>& argv);
void cmd_q(vector<string>& argv);
void cmd_exit(vector<string>& argv);

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
   mech.current_time = 0.0;

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
   vector<string> cmd_argv;

   CMDS_STORE["n_click_speed"]         = "0.075";
   CMDS_STORE["n_click_max_distance"]  = "3.8";
   CMDS_STORE["n_click_distance"]      = "1.25";
   SD = 2.0;

   Sphere selector_sphere_1(0.0, 0.0, 0.0, 0.0369, 0.4, 0.8, 0.6);
   sphrs.push_back(selector_sphere_1);

   IN_GAME_CMDS.handle(
         "help",
         cmd_help,
         "\nThis is the in-game command line interface.",
         "help [command]");

   IN_GAME_CMDS.handle(
         "mo",
         cmd_tp,
         "Teleport to the given coordinates.",
         "mo x y z");

   IN_GAME_CMDS.handle(
         "set",
         cmd_set,
         "Set a value in CMDS_STORE.",
         "set [key [value]]",
         "Set called alone will print all values in the map. Called with \nonly a key argument it will display the value for that key. Called \nwith both key and value arguments will write the value to the map.");

   IN_GAME_CMDS.handle(
         "select",
         cmd_select,
         "Select an object by index.",
         "select [index]");

   IN_GAME_CMDS.handle(
         "grow",
         cmd_grow,
         "Generate x tetrahedrons from selected object.",
         "grow [x]");

   IN_GAME_CMDS.handle(
         "q",
         cmd_q,
         "Quit to menu.",
         "q");

   IN_GAME_CMDS.handle(
         "e",
         cmd_exit,
         "Exit the command line.",
         "e");


   Object test_object_1("test_object_1", 1, NULL);
   Vertex center;
   test_object_1.tetra.center(center);
   test_object_1.tetra.translate_by(-center.x, -center.y, -center.z);
   test_object_1.tetra.center(center);
   cout << "Spawning block " << test_object_1.id << ". center: "
        << center.x << ", " << center.y << ", " << center.z << "\n";
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

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   int fake = 0;
   char** fake2;
   glutInit(&fake, fake2);

   glutInitWindowSize(700, 500);
   glutInitDisplayMode(GLUT_RGB | GLUT_STENCIL | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Mechanizm");

   glutIdleFunc(hot_pause);
   glutDisplayFunc(drawScene);
   glutMouseFunc(mouse);
   glutPassiveMotionFunc(mouse_passive);
   glutReshapeFunc(resize);
   glutCreateMenu(menu);
   draw_glut_menu();
   glutAttachMenu(GLUT_RIGHT_BUTTON);
   glutKeyboardFunc(keyboard);
   glutKeyboardUpFunc(keyboard_up);
   glutMainLoop();

   return 0;
}

void cmd_help(vector<string>& argv) {
   IN_GAME_CMDS.better_default_help(argv);
   for (int i = 0; i < argv.size(); i++) {;
      menu_output.push_back(argv[i]);
   }
}

void cmd_tp(vector<string>& argv) {
   cam.setX(as_double(argv[0]));
   cam.setY(as_double(argv[1]));
   cam.setZ(as_double(argv[2]));
}

void cmd_set(vector<string>& argv) {
   string msg;
   map<string, string>::iterator it;
   if (argv.size() == 0) {
      for (it = CMDS_STORE.begin(); it != CMDS_STORE.end(); it++) {
         msg = it->first;
         msg += ": ";
         msg += it->second;
         menu_output.push_back(msg);
      }
   }
   else if (argv.size() == 1) {
      it = CMDS_STORE.find(argv[0]);
      if (it != CMDS_STORE.end()) {
         msg = it->first;
         msg += ": ";
         msg += it->second;
         menu_output.push_back(msg);
      }
   }
   else if (argv.size() == 2) {
      CMDS_STORE[argv[0]] = argv[1];
      msg = "setting ";
      msg += argv[0];
      msg += ": ";
      msg += CMDS_STORE[argv[0]];
      menu_output.push_back(msg);
   }
}

void cmd_select(vector<string>& argv) {
   menu_output.push_back("you ran select!");
   for (int z = 0; z < argv.size(); z++) {
      menu_output.push_back(argv[z]);
   }
}

void cmd_grow(vector<string>& argv) {
   if (argv.size() != 1) {
      menu_output.push_back("invalid arguments: try 'help grow'");
      return;
   }
   int arg = as_int(argv[0]);

   while (arg-- > 0) {
      string gi = selected_glob;

     // generate_tetra_from_side(objs[selected_object].tetra, rand() % 4);
      char buffer[100];
      sprintf(buffer, "generated_object_%i", gen_obj_cnt);
      gen_obj_cnt++;
      string id = buffer;
      Object o(id);

      auto oit = gs[gi].objs.begin();
      int robji = rand() % gs[gi].objs.size() - 1;
      while (robji > 0) {robji--; oit++;}
      string robjkey = oit->first;

      string msg = "growing object: " + robjkey;
      menu_output.push_back(msg);

      int rvfacei = rand() % gs[gi].objs[robjkey].tetra.vis_faces.size() - 1;
      int rfacei = gs[gi].objs[robjkey].tetra.vis_faces[rvfacei];
      o.tetra = generate_tetra_from_side(
                     oit->second.tetra, rfacei);

      gs[gi].attach(robjkey, rfacei, o);
   }
   //timed_menu_display = 60;
}

void cmd_q(vector<string>& argv) {
   menu_depth = 1;
}

void cmd_exit(vector<string>& argv) {
   menu_depth = 0;
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
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 300,
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 300,
            (GLfloat) (rand() % 7 - 3) / (GLfloat) 300});

   Object tmp;
   if (self.c_qs.count("vi"))
      tmp.setConstQ("vi", self.c_qs["vi"]);
   else
      tmp.setConstQ("vi", 0.0);

   tmp.set_cube(self.cube);

   tmp.setConstQ("a", self.getConstQ("a"));

   //tmp.setConstQ("a", {
   //   self.c_qs["a"][0] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190,
   //   self.c_qs["a"][1] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190,
   //   self.c_qs["a"][2] + (GLfloat) (rand() % 21 - 10) / (GLfloat) 190});

   //tmp.setConstQ("a", {
   //   self.c_qs["a"][0] + (GLfloat) (rand() % 3 - 1) / (GLfloat) 10,
   //   self.c_qs["a"][0] + (GLfloat) (rand() % 3 - 1) / (GLfloat) 10,
   //   self.c_qs["a"][1] + (GLfloat) (rand() % 3 - 1) / (GLfloat) 10});
   //
   //
   //Object tmp_2 = tmp;

   tryhard_motion(t, tmp);

   //tryhard_motion(self.last_t, tmp_2);

   //tmp_2.multiply_by(-1, -1, -1);
   //tmp.translate_by(tmp_2.cube[0][0], tmp_2.cube[0][1], tmp_2.cube[0][2]);

   //self.translate_by(tmp.cube[0][0], tmp.cube[0][1], tmp.cube[0][2]);

   self.set_cube(tmp.cube);
   self.last_t = t;

   return NULL;
}

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
   int dfi, j = objs_index;
   glBegin(GL_POLYGON);
   for (dfi = 0; dfi < 4; dfi++) {
      glVertex3fv((GLfloat *) objs[j].cube[ objs[j].faceIndex[face][dfi] ]);
   }
   glEnd();
}


void drawFilledTetrahedron(int objs_index) {

   if (draw_x_vertices > 0) {
      int j = objs_index;
      int drawn = 0;

      glBegin(GL_TRIANGLES);
      for (int fi = 0; fi < 4; fi++) {
         glColor3fv(objs[j].tetra.faceColors[fi]);
         for (int pi = 0; pi < 3; pi++) {

            glVertex3f(
               *objs[j].tetra.face[fi].pnts[pi][0],
               *objs[j].tetra.face[fi].pnts[pi][1],
               *objs[j].tetra.face[fi].pnts[pi][2]);

            drawn++;
            if (drawn > draw_x_vertices) {
               glEnd();
               return;
            }
         }
      }
      glEnd();
   }
}

void drawVisibleTriangles(Tetrahedron& tetra) {

   int drawn = 0;

   if (draw_x_vertices < 0) {
      for (int pi = 0; pi < 4; pi++) {
         Sphere new_sphere(tetra.points[pi].x,
                           tetra.points[pi].y,
                           tetra.points[pi].z,
                           0.0369, 0.4, 0.8, 0.6);
         draw_sphere(new_sphere);
      }
      return;
   } 
   glBegin(GL_TRIANGLES);

   for (int vfi = 0; vfi < tetra.vis_faces.size(); vfi++) {
      int fi = tetra.vis_faces[vfi];

      glColor3fv(tetra.faceColors[fi]);

      for (int pi = 0; pi < 3; pi++) {
         Vertex tmpv;
         tmpv.x = *tetra.face[fi].pnts[pi][0];
         tmpv.y = *tetra.face[fi].pnts[pi][1];
         tmpv.z = *tetra.face[fi].pnts[pi][2];

         glVertex3f(
            tmpv.x,
            tmpv.y,
            tmpv.z);

         drawn++;
         if (drawn > draw_x_vertices) {
            glEnd();
            return;
         }
      }
   }
   glEnd();
}

void draw_circle(float cx, float cy, float r, int num_segments) {
   glBegin(GL_LINE_LOOP);
   for (int ii = 0; ii < num_segments; ii++)   {
      //get the current angle 
      float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);
      float x = r * cosf(theta);//calculate the x component 
      float y = r * sinf(theta);//calculate the y component 
      glVertex2f(x + cx, y + cy);//output vertex 
   }
   glEnd();
}

void draw_cam_spheres() {
   Error e = NULL;

   // if left click is pressed calculate the distance
   if (SD_DONE == false) {

      GLfloat n_d = as_double(CMDS_STORE["n_click_distance"]);
      GLfloat n_s = as_double(CMDS_STORE["n_click_speed"]);
      GLfloat n_m_d = as_double(CMDS_STORE["n_click_max_distance"]);

      GLfloat nsx, nsy, nsz;

      if (SD > n_m_d) {
         SD = n_d;
         if (mouse_left_state == 1) SD_DONE = true;
      }
      else {
         SD += n_s;
      }

      // get the new position in front of the camera
      cam.pos_in_los(SD, nsx, nsy, nsz);

      // set the sphere position
      sphrs[0].center[0] = nsx;
      sphrs[0].center[1] = nsy;
      sphrs[0].center[2] = nsz;

      int i = -1;
      int sidx;
      int sidy;
      int sidz;

      // check for collision with glob
      for (auto git = gs.begin(); git != gs.end(); git++) {

      string gi = git->first;

      // check each object within glob gs[gi]
      for (auto jit = gs[gi].objs.begin(); jit != gs[gi].objs.end(); jit++) {

         string j = jit->first;

         if (gs[gi].objs[j].shape == "tetrahedron") {

            // get line segment points
            Vertex line[2];
            line[0].x = nsx;
            line[0].y = nsy;
            line[0].z = nsz;

            GLfloat tx, ty, tz;
            cam.pos_in_los(SD+0.08f, tx, ty, tz);
            line[1].x = tx;
            line[1].y = ty;
            line[1].z = tz;

            // get triangle points of each face
            for (int fi = 0; fi < 4; fi++) {
               Vertex tripnts[3];

               for (int tripti = 0; tripti < 3; tripti++) {
                  tripnts[tripti].x =
                     *gs[gi].objs[j].tetra.face[fi].pnts[tripti][0];
                  tripnts[tripti].y =
                     *gs[gi].objs[j].tetra.face[fi].pnts[tripti][1];
                  tripnts[tripti].z =
                     *gs[gi].objs[j].tetra.face[fi].pnts[tripti][2];
               }

               if (tools::line_intersects_triangle(
                        line,
                        tripnts,
                        NULL)) {

                  // glob: ji object: j
                  if (selector_function == "select") {
                     selected_glob = gi;

                     string msg("selected glob: " + gi + "\nobject: " + j);
                     char buffer[100];
                     sprintf(buffer, ", face %i", fi);
                     msg += buffer;
                     menu_output.push_back(msg);
                     timed_menu_display = 150;
                     draw_menu_lines = 2;

                     reset_sphere_distance();
                     return;
                  }
                  else if (selector_function == "remove") {

                     auto it = gs[gi].objs.find(j);
                     gs[gi].objs.erase(it);

                     SD = as_double(CMDS_STORE["n_click_distance"]);
                     SD_DONE = true;

                     menu_output.push_back(
                           "glob: " + gi + ": removed object " + j);
                     timed_menu_display = 150;
                     draw_menu_lines = 1;

                     return;
                  }
                  else if (selector_function == "build") {
                     selected_glob = gi;

                     char buffer[100];
                     sprintf(buffer, "generated_object_%i", gen_obj_cnt++);

                     Object o(buffer);
                     o.tetra = generate_tetra_from_side(
                                    gs[gi].objs[j].tetra, fi);

                     gs[gi].attach(j, fi, o);

                     SD = as_double(CMDS_STORE["n_click_distance"]);
                     SD_DONE = true;
                     return;
                  }
               }
            }

         } // end of tetrahedron loop
      } // end of obect loop
      } // end of glob collision loop

      if (MAP.ms.size() > 0) {
         // select non-empty blocks from the map
         // find map section
         sidx = floorf(nsx / (float) MAP.ms[0].size) * MAP.ms[0].size;
         sidy = floorf(nsy / (float) MAP.ms[0].size) * MAP.ms[0].size;
         sidz = floorf(nsz / (float) MAP.ms[0].size) * MAP.ms[0].size;
         i = MAP.section_i_loaded(sidx, sidy, sidz);
      }

      if (i != -1) { // check the block array
         int bidx = floorf(nsx) - sidx;
         int bidy = floorf(nsy) - sidy;
         int bidz = floorf(nsz) - sidz;
         if (bidx < 0) {bidx *= -1;}
         if (bidy < 0) {bidy *= -1;}
         if (bidz < 0) {bidz *= -1;}

         if (MAP.ms[i].blocks[bidy][bidx][bidz].type == 1) {
    
            MAP.ms[i].blocks[bidy][bidx][bidz].type = 0;
            cout << "TYPE 1: block: " << sidx + bidx << " " << sidy + bidy << " " << sidz + bidz << " " << endl;

            SD = as_double(CMDS_STORE["n_click_distance"]);
            if (mouse_left_state == 1) SD_DONE = true;
         }

      }
   }
   else {
      cam.pos_in_los(SD,
            sphrs[0].center[0], sphrs[0].center[1], sphrs[0].center[2]);
   }


   // draw the first and best sphere
   glColor3f(sphrs[0].color[0], sphrs[0].color[1], sphrs[0].color[2]);
   glPushMatrix();
   glTranslatef(sphrs[0].center[0], sphrs[0].center[1], sphrs[0].center[2]);
   glutWireSphere(sphrs[0].r, 9, 9);
   glPopMatrix();

}

void draw_spheres() {
   for (int i = 1; i < sphrs.size(); i++) {
      draw_sphere(sphrs[i]);
   }
}

void draw_sphere(Sphere& s) {
      glColor3f(s.color[0], s.color[1], s.color[2]);

      glPushMatrix();
      glTranslatef(s.center[0], s.center[1], s.center[2]);
      glutWireSphere(s.r, 36, 36);
      glPopMatrix();
}

void reset_sphere_distance() {
   SD = as_double(CMDS_STORE["n_click_distance"]);
   if (mouse_left_state == 1) SD_DONE = true;
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
   //if (count == 60) {
   //   cout << "client::drawSides: Total visible sides drawed: `"
   //        << sidecnt << "`.\n";
   //}
}

void drawFilledTStrip() {
   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1, 0, 0); glVertex3f(0, 2, 0);
   glColor3f(0, 1, 0); glVertex3f(-1, 0, 1);
   glColor3f(0, 0, 1); glVertex3f(1, 0, 1);
   glEnd();
}

void mouse(int button, int state, int x, int y) {

   // left button down
   if (button == 0 && state == 0) {
      mouse_left_state = 0;

      if (SD_DONE) {
         SD_DONE = false;
      }
      else {
         SD = as_double(CMDS_STORE["n_click_distance"]);
      }
   }
   else if (button == 0 && state == 1) {
      mouse_left_state = 1;
   }

}

void mouse_passive(int x, int y) {
   cam.rotation(x, y);
//   SDL_SetRelativeMouseMode((SDL_bool) 1);
   glutPostRedisplay();
}

void draw_bitmap_string(
      float x, float y, float z, void *font, string& str) {
   glColor3f(0.1, 0.99, 0.69);
   glRasterPos3f(x, y, z);
   for (int i = 0; i < str.size(); ++i) {
      glutBitmapCharacter(font, str[i]);
   }
}

void draw_menu_output(float x, float y, float z, void *font) {
   while (menu_output.size() > 20) {
      menu_output.erase(menu_output.begin());
   }

   int dmli = menu_output.size() - draw_menu_lines;
   while (dmli < 0) { dmli++; }

   newlines_to_indices(menu_output);
   for (int i = menu_output.size() - 1; i >= dmli; i--) {
      y += 0.0369;
      glColor3f(0.1, 0.69, 0.99);
      glRasterPos3f(x, y, z);
      for (int j = 0; j < menu_output[i].size(); ++j) {
         glutBitmapCharacter(font, menu_output[i][j]);
      }
   }
}

void drawScene(void) {
   int i;

   glClearColor(0.2, 0.4, 0.5, 0.0); // background color
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();

   //// set up camera
   //glViewport(0, 0, 500, 500);

   glLoadIdentity();
   cam.translation();
   glRotatef(180 * cam.getAY() / 3.141592653, 1.0, 0.0, 0.0);
   glRotatef(180 * cam.getAX() / 3.141592653, 0.0, 1.0, 0.0);
   glTranslated(-cam.getX(), -cam.getY(), -cam.getZ());

   // draw stuff
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);

   drawSides();

   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
   glColor3f(0, 1, 0); glVertex3f(-1, 0, 1);
   glColor3f(0, 0, 1); glVertex3f(1, 0, 1);
   glEnd();

   draw_circle(3.0, 1.0, -3.0, 36);

   draw_spheres();

   draw_cam_spheres();

   if (DRAW_GLUT_MENU) {
      draw_glut_menu();
      DRAW_GLUT_MENU = false;
   }

   glEnable(GL_STENCIL_TEST);
   glClear(GL_STENCIL_BUFFER_BIT);
   glStencilMask(1);
   glStencilFunc(GL_ALWAYS, 0, 1);
   glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
   glColor3f(1.0, 1.0, 0.0);

   // draw all the objects in objs vector
   for (int j = 0; j < objs.size(); ++j) {
      if (objs[j].shape == "cube") {
         for (i = 0; i < 6; i++) {
            //drawWireframe(j, i);
            //glStencilFunc(GL_EQUAL, 0, 1);
            //glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

            // set the default color
            //objs[j].faceColors[i][0] = rand() % 100 / 100.0;//0.7;
            //objs[j].faceColors[i][1] = rand() % 100 / 100.0;//0.7;
            //objs[j].faceColors[i][2] = rand() % 100 / 100.0;//0.7;
            glColor4f(objs[j].faceColors[i][0], objs[j].faceColors[i][1], objs[j].faceColors[i][2], 0.1); // box color
            drawFilled(j, i);

//            glStencilFunc(GL_ALWAYS, 0, 1);
//            glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
            glColor3f(0.0, 0.0, 0.0);
            drawWireframe(j, i);
         }
      }
      if (objs[j].shape == "tetrahedron") {
         drawFilledTetrahedron(j);
      }
   }

   // draw visible triangles in globs map
   for (auto glit = gs.begin(); glit != gs.end(); glit++) {
      string id = glit->first;
      for (auto objit = gs[id].objs.begin(); objit != gs[id].objs.end();
                                                                objit++) {
         drawVisibleTriangles(objit->second.tetra);
      }
   }

   if (menu_depth != 0 || timed_menu_display-- > 0) {
      // write character bitmaps
      draw_bitmap_string(
         // these calculate the location in front of the face
         cam.getX() + (cos(cam.getAX() - 2*(M_PI/3)) * cos(-cam.getAY()))*1.3,
         //cam.getY() + sin(-cam.getAY())*1.3 - 0.5,
         cam.getY() - 0.5,
         cam.getZ() + (sin(cam.getAX() - 2*(M_PI/3)) * cos(-cam.getAY()))*1.3,
         GLUT_BITMAP_9_BY_15,
         menu_input);

      // draw the output next
      draw_menu_output(
         // these calculate the location in front of the face
         cam.getX() + (cos(cam.getAX() - 2*(M_PI/3)) * cos(-cam.getAY()))*1.3,
         //cam.getY() + sin(-cam.getAY())*1.3 - 0.5,
         cam.getY() - 0.5,
         cam.getZ() + (sin(cam.getAX() - 2*(M_PI/3)) * cos(-cam.getAY()))*1.3,
         GLUT_BITMAP_9_BY_15);

//         cam.getX() + (cos(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*1.3,
//         (GLfloat) (cam.getY() + sin(-cam.getAY())*1.3 - 0.33),
//         cam.getZ() + (sin(cam.getAX() - M_PI/2) * cos(-cam.getAY()))*1.3,

   }

   //if (PRICION) { // write date and time tags on objects
   //   
   //}


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
      for (int z = 0; z < MAP.ms.size(); ++z) {
         if (!MAP.ms[z].has_sides)
            MAP.ms[z].generate_visible_sides(cam);

      }
   }

   if (count % 15 == 0) {
      MAP.generate_visible_edges();
   }

   if (count == 60) {
      cout << "client::map_generation: cached map section queue has "
           << CMSQ.size() << " sections.\n";
   }

   int fcount = 0;
   // fork and save the map sections with no PIDSID entry
   if (CMSQ.size() >= 128 && PIDSIDQ.size() < 4) {
      for (int u = 0; u < CMSQ.size(); u++) {

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

bool once = false;
Json::Value JVAL;
void pricion(void) {
   Error e = NULL;
   string jsonfile = "data/pricion_hist.json";
   if (!once) {
      e = tools::load_json_value_from_file(JVAL, jsonfile);
      if (e != NULL) {
         cout << "client::pricion: " <<  e << "\n";
      }
      if (JVAL.isArray()) {
         cout << "client::pricion: JVAL is an array!!!\n";
         cout << "client::pricion: there are " << JVAL.size() << " elements\n";

         GLfloat c1 = 0.5;//rand() % 100 / 100.0;
         GLfloat c2 = 0.1;//rand() % 100 / 100.0;
         GLfloat c3 = 0.5;//rand() % 100 / 100.0;

         for (int i = 0; i < JVAL.size(); i++) {
            //Json::Members mv;
            cout << i << ": date: "
                 << JVAL[i]["date"].asInt64() << " weightedAverage: "
                 << JVAL[i]["weightedAverage"].asDouble() << "\n";
            Object tmpcube;

            c2 += 0.009;

            for (int i2 = 0; i2 < 6; i2++) {
               // set the default color
               tmpcube.faceColors[i2][0] = c1;
               tmpcube.faceColors[i2][1] = c2;
               tmpcube.faceColors[i2][2] = c3;
            }

            tmpcube.translate_by(
                  (GLfloat) i,
                  (GLfloat) JVAL[i]["weightedAverage"].asDouble() - 500.000,
                  0.0);
            objs.push_back(tmpcube);

            // write the time and price
            // must be done inside draw block
            //string tag = "date: ";
            //tag += JVAL[i]["date"].asInt64();
            //tag += " weightedAverage: $";
            //tag += JVAL[i]["weightedAverage"].asDouble();
            //draw_bitmap_string(
            //      (GLfloat) i,
            //      (GLfloat) JVAL[i]["weightedAverage"].asDouble() - 499.250,
            //      0.0,
            //      GLUT_BITMAP_9_BY_15,
            //      tag);
         }

      }
      else {
         cout << "jval is not an array\n";
      }
      once = true;
   }
}

void hot_pause(void) {
   if (MAP_GEN) {
      map_generation();
   }
   if (ANIMATION) {
      animation();
   }
   if (PRICION) {
      pricion();
   }

   glutPostRedisplay();

   count++;
   if (count > 60) {
      count = 0;
   }
}

void draw_glut_menu() {
   while (glutGet(GLUT_MENU_NUM_ITEMS) > 0) {
      glutRemoveMenuItem(1);
   }
 
   if (MAP_GEN) {
      glutAddMenuEntry("1 - Map Generation", 0);
   }
   else {
      glutAddMenuEntry("0 - Map Generation", 0);
   }
   if (ANIMATION) {
      glutAddMenuEntry("1 - Animation", 1);
   }
   else {
      glutAddMenuEntry("0 - Animation", 1);
   }
   if (PRICION) {
      glutAddMenuEntry("1 - Pricion", 2);
   }
   else {
      glutAddMenuEntry("0 - Pricion", 2);
   }
   glutAddMenuEntry("Reset", 3);
}

// mouse right click menu
void menu(int choice) {

   switch (choice) {
   case 0: // Map Generation
      if (MAP_GEN) {
         MAP_GEN = false;
      }
      else {
         MAP_GEN = true;
      }
      break;
   case 1: // Animation
      count = 0;
      if (ANIMATION) {
         ANIMATION = false;
      }
      else {
         ANIMATION = true;
      }
      break;
   case 3: // Reset
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
   case 2: // Pricion
      if (PRICION) {
         PRICION = false;
      }
      else {
         PRICION = true;
      }
      break;
   }

   DRAW_GLUT_MENU = true;
   //if (choice != 0) {
       //draw_glut_menu();
   //}
   glutPostRedisplay();
}

void menu_1_help() {
   string msg;
   msg += "\n*** Menu level: 1\n";
//   msg += "  -  selected object: " << selected_object;
//   msg += "  -  time: " << mech.current_time << "\n";
   msg += "   p -   Print the object information\n";
   msg += "   r -   Set motion function to null.\n";
   msg += "   c -   Randomize the colors of selected object.\n";
   msg += "   e -   Enter command.\n";
   msg += "   h -   Print this help message.\n";
   msg += "   q -   exit menu\n";
   msg += "\n";
   cout << msg;
   menu_output.push_back(msg);
   timed_menu_display = 60;
}

/* ARGSUSED1 */
void
keyboard(unsigned char c, int x, int y)
{

   string prefix = "-> ";
   for (int i = 0; i < menu_depth; ++i) {
      prefix += "   ";
   }

   // timed display of in game menu output on button-press menu

   if (menu_depth != 0) { // go into menu mode

      if (menu_depth == 37) { // menu 37 is cmd line input
         if (c != 13 && c != 8 && c != 27) { // 13 is Enter
            // use a buffer to hold the input while writing the characters
            // to stdout.
            menu_input += c;
            cout << (int)c << "\n";
         }
         else if (c == 8) { // backspace
            if (menu_input.size() > 3) {
               menu_input.resize(menu_input.size()-1);
            }
         }
         else if (c == 27) { // ESC
            menu_output.push_back("exiting menu");
            menu_depth = 0;
            timed_menu_display = 20;
            draw_menu_lines = 20;
            return;
         }
         else  {
            menu_input[0] = ' ';
            menu_input[1] = ' ';

            // get the command and arguments from the menu_input
            // space characters can be escaped with forwardslash
            string cmd;
            vector<string> cmd_argv;
            string word;
            menu_input += " "; // laziness

            for (int z = 0; z < menu_input.size(); z++) {
               if (menu_input[z] == '\\') {
                  word += ' ';
                  z++;
                  continue;
               }
               if (menu_input[z] == ' ') {
                  if (word.size() > 0) {
                     if (cmd.size() > 0) {
                        cmd_argv.push_back(word);
                     }
                     else {
                        cmd = word;
                     }
                     word.clear();
                  }
                  continue;
               }
               word += menu_input[z];
            }
            menu_input[0] = '\\';
            menu_input[1] = '>';
            menu_output.push_back(menu_input);
            menu_input = "^> ";

            string msg = "CMDS: Unknown command: `";
            IN_GAME_CMDS.run(cmd, cmd_argv);
            if (cmd != "help" && cmd != "" && !IN_GAME_CMDS.resolved()) {
               msg += cmd;
               msg += "`";
               menu_output.push_back(msg);
            }
         }
         return;
      }

      string msg;
      char buffer [100];
      sprintf(buffer, "\n*** Menu level: %i", menu_depth); msg += buffer;
      msg += "  -  selected object: " + selected_glob;
      sprintf(buffer, "  -  time: %f\n", mech.current_time); msg += buffer;

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
         objs[selected_object].function = NULL;
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
         draw_menu_lines = 20;
         menu_input = "^> ";
         cout << prefix << "entering command mode:\n";
      }
      if (c == 'h') {
         menu_1_help();
      }
      if (c == 'q') {
         cout << prefix << "exiting menu\n";
         msg += prefix;
         msg += "exiting menu\n";
         menu_depth = 0;
         draw_menu_lines = 20;
      }
      if (c == 27) {
         cout << prefix << "exiting menu\n";
         msg += prefix;
         msg += "exiting menu\n";
         menu_depth = 0;
         timed_menu_display = 20;
         draw_menu_lines = 20;
      }
      cout << msg;
      menu_output.push_back(msg);
      return;
   }

   // may need incase player presses b
   Object tmp("object_1");
   Glob spawn_glob(tmp);

   // gameplay level controls
   char m0_buffer[100];
   if (c == 'h' | c == '?') {
      menu_output.push_back("\n   Controls:\n");
      menu_output.push_back("      m -             open menu");
      menu_output.push_back("      b -             spawn a cube");
      menu_output.push_back("      v -             spawn a vertice");
      menu_output.push_back("      i j k l y n -   move selected cube");
      menu_output.push_back("      u o -           stretch selected cube");
      menu_output.push_back("      0-9 -           select cube");
      menu_output.push_back("      +/- -           add or subtract number of points drawn\n");
      
      timed_menu_display = 60;
   }
   if (c == 'e') {
      menu_input = "^> ";
      menu_depth = 37;
      draw_menu_lines = 25;
   }
   if (c == 'b') {
      string id ;
      char n[100];
      sprintf(n, "spawned_g_object_%i", gs.size());
      id = n;

      cout << prefix;
      cout << "spawning glob object: " << id << "\n";

      string msg = prefix;
      msg += "spawning glob object: " + id + "\n";
      menu_output.push_back(msg);
      draw_menu_lines = 2;
      timed_menu_display = 60;


      spawn_glob.id = id;
      spawn_glob.objs["object_1"].setConstQ("m", 1);

      //switch (rand() % 3) {
      //case 0:
      //   cout << "1!\n";
      //   spawned_test_object.function = random_motion;
      //   break;
      //case 1:
      //   cout << "2!\n";
      //   spawned_test_object.function = random_motion_2;
      //   break;
      //case 2:
      //   cout << "2!\n";
      //   spawned_test_object.function = random_motion_2;
      //   break;
      //}

      GLfloat nx, ny, nz;
      cam.pos_in_los(SD, nx, ny, nz);

      spawn_glob.translate_by(nx, ny, nz);
      gs[spawn_glob.id] = spawn_glob;

   }
   if (c == '+') { // add to draw_x_vertices
      draw_x_vertices++;
      sprintf(m0_buffer, "draw_x_vertices: %i", draw_x_vertices);
      string msg = m0_buffer;
      cout << msg << "\n";
      menu_output.push_back(msg);
      timed_menu_display = 60;
      return;
   }
   if (c == '-') { // subtract from draw_x_vertices
      if (draw_x_vertices > -1) draw_x_vertices--;
      sprintf(m0_buffer, "draw_x_vertices: %i", draw_x_vertices);
      string msg = m0_buffer;
      cout << msg << "\n";
      menu_output.push_back(msg);
      timed_menu_display = 60;
      return;
   }

   Vertex center;


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
      gs[selected_glob].translate_by(0, 0, -0.1);
      break;
   case 'j':
      gs[selected_glob].translate_by(-0.1, 0, 0);
      break;
   case 'k':
      gs[selected_glob].translate_by(0, 0, 0.1);
      break;
   case 'l':
      gs[selected_glob].translate_by(0.1, 0, 0);
      break;
   case 'y':
      gs[selected_glob].translate_by(0, 0.1, 0);
      break;
   case 'n':
      gs[selected_glob].translate_by(0, -0.1, 0);
      break;
   case 'o':
//      gs[selected_glob].scale_by(1.1, 1.1, 1.1);
      break;
   case 'u':
//      gs[selected_glob].scale_by(0.9, 0.9, 0.9);
      break;
   case 'I':
      gs[selected_glob].rotate_abt_center(7.0*(M_PI/4.0), 0.0, 0.0);
      break;
   case 'J':
      gs[selected_glob].rotate_abt_center(0.0, M_PI/4.0, 0.0);
      break;
   case 'K':
      gs[selected_glob].rotate_abt_center(M_PI/4.0, 0.0, 0.0);
      break;
   case 'L':
      gs[selected_glob].rotate_abt_center(0.0, 7.0*(M_PI/4.0), 0.0);
      break;
   case 'U':
      gs[selected_glob].rotate_abt_center(0.0, 0.0, M_PI/4.0);
      break;
   case 'O':
      gs[selected_glob].rotate_abt_center(0.0, 0.0, 7.0*(M_PI/4.0));
      break;
   case '`':
      menu_depth = 1;
      menu_1_help();
      break;
   case '1':
      selector_function = "select";
      menu_output.push_back("mouse function: select");
      draw_menu_lines = 1;
      timed_menu_display = 50;
      break;
   case '2':
      selector_function = "remove";
      menu_output.push_back("mouse function: remove");
      draw_menu_lines = 1;
      timed_menu_display = 50;
      break;
   case '3':
      selector_function = "build";
      menu_output.push_back("mouse function: build");
      draw_menu_lines = 1;
      timed_menu_display = 50;
      break;
   default: // hotkeys
      if (c == '4' || c == '5' || c == '0')
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

Tetrahedron generate_tetra_from_side(
      Tetrahedron& source_tetra, int source_face) {

   Tetrahedron new_tetra;
   // reflect opposite point across selected side

   // temporary vector placed at position of opposite point
   Vertex opp_v;
   // get the index of the vertex that is reflected
   int opp_v_i = 4;
   if (source_face == 0) opp_v_i = 3;
   if (source_face == 1) opp_v_i = 2;
   if (source_face == 2) opp_v_i = 1;
   if (source_face == 3) opp_v_i = 0;

   opp_v = source_tetra.points[opp_v_i];

   // use 2 lines from source face to reflect opp_v_i across plane
   Vertex tnorm;
   Vertex tpa = source_tetra.points[source_tetra.faceIndex[source_face][0]];
   Vertex tpb = source_tetra.points[source_tetra.faceIndex[source_face][1]];
   Vertex tpc = source_tetra.points[source_tetra.faceIndex[source_face][2]];
   tnorm = ((tpb - tpa).cross(tpc - tpa)).normalize();

   Vertex u;
   u.x = (opp_v.dot(tnorm) - tnorm.dot(tpa)) * tnorm.x;
   u.y = (opp_v.dot(tnorm) - tnorm.dot(tpa)) * tnorm.y;
   u.z = (opp_v.dot(tnorm) - tnorm.dot(tpa)) * tnorm.z;
   Vertex v = opp_v - u;

   Vertex new_v = v - u;

   // loop through source face indices copy points for each face
   
   if (source_face == 0) {
      new_tetra.points[0] = 
         source_tetra.points[source_tetra.faceIndex[source_face][1]];
      new_tetra.points[1] =
         source_tetra.points[source_tetra.faceIndex[source_face][0]];
      new_tetra.points[2] =
         source_tetra.points[source_tetra.faceIndex[source_face][2]];
      new_tetra.points[3] = new_v;
   }
   if (source_face == 1) {
      new_tetra.points[0] = 
         source_tetra.points[source_tetra.faceIndex[source_face][1]];
      new_tetra.points[1] =
         source_tetra.points[source_tetra.faceIndex[source_face][0]];
      new_tetra.points[2] = new_v;

      new_tetra.points[3] = 
         source_tetra.points[source_tetra.faceIndex[source_face][2]];

   }
   if (source_face == 2) {
      new_tetra.points[0] = new_v;
      new_tetra.points[1] =
         source_tetra.points[source_tetra.faceIndex[source_face][0]];
      new_tetra.points[2] =
         source_tetra.points[source_tetra.faceIndex[source_face][1]];
      new_tetra.points[3] = 
         source_tetra.points[source_tetra.faceIndex[source_face][2]];

   }
   if (source_face == 3) {
      new_tetra.points[0] =
         source_tetra.points[source_tetra.faceIndex[source_face][0]];
      new_tetra.points[1] = new_v;

      new_tetra.points[2] =
         source_tetra.points[source_tetra.faceIndex[source_face][1]];
      new_tetra.points[3] = 
         source_tetra.points[source_tetra.faceIndex[source_face][2]];

   }

   return new_tetra;
}
