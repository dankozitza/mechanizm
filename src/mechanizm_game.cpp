//
// mechanizm_game.cpp
//
// Asteroid mining and block building game.
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
#include "Glob.hpp" // includes Vertex, Tetrahedron, & Object
#include "Sphere.hpp"


using namespace tools;

void help(string prog_name);

void
   drawScene(void),
   setMatrix(int w, int h),
   map_generation(void),
   animation(void),
   pricion(void),
   g_pause(void),
   draw_glut_menu(),
   menu(int choice),
   resize(int w, int h),
   mouse(int button, int state, int x, int y),
   mouse_passive(int x, int y),
   keyboard(unsigned char c, int x, int y),
   keyboard_up(unsigned char c, int x, int y),
   motion(int x, int y),
   drawWireframe(int objs_index, int face),
   drawVisibleTriangles(Tetrahedron& tetra),
   draw_circle(float cx, float cy, float r, int num_segments),
   draw_cam_spheres(),
   draw_sphere(Sphere& s),
   draw_spheres(),
   reset_sphere_distance();

tools::Error
   random_motion(double t, Object &self),
   random_motion_2(double t, Object &self),
   spawned_motion(double t, Object &self),
   tryhard_motion(double t, Object &self);

Tetrahedron generate_tetra_from_side(Tetrahedron& source, int source_face);

static unordered_map<string, Glob> gs;
static vector<Sphere> sphrs;
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
static Camera CAM(2.0, 2.0, 3.5, 0.0, 0.0, 0.6, 0.0022);

static mechanizm MECH;
int count = 1; // cycles through 1/60th of a second

commands IN_GAME_CMDS;
map<string, string> CMDS_STORE;
map<string, unsigned long> GAME_STATS;
GLfloat SD; // current distance of the selection sphere
bool SD_DONE = true;

bool DRAW_GLUT_MENU = false;

// menu options
bool MAP_GEN = false;
bool ANIMATION = false;

// commands for the in-game cli
void
   cmd_help(vector<string>& argv),
   cmd_tp(vector<string>& argv),
   cmd_set(vector<string>& argv),
   cmd_select(vector<string>& argv),
   cmd_grow(vector<string>& argv),
   cmd_stat(vector<string>& argv),
   cmd_load(vector<string>& argv),
   cmd_exit(vector<string>& argv);

int main(int argc, char *argv[]) {

   string prog_name = string(argv[0]);
   options opt;
   bool quiet      = false;
   bool show_help  = false;
   bool file_given = false;
   bool realtime   = false;
   bool time       = false;
   string file_path;
   string m[5];
   vector<string> args;
   Error e = NULL;

   //MECH.set_objects(objs);
   MECH.current_time = 0.0;

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

   // set up the in game command line
   vector<string> cmd_argv;

   CMDS_STORE["n_click_speed"]         = "0.075";
   CMDS_STORE["n_click_max_distance"]  = "3.8";
   CMDS_STORE["n_click_distance"]      = "1.25";
   CMDS_STORE["hotkey_g"] = "grow 1";
   CMDS_STORE["hotkey_G"] = "grow 500";
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
         "stats",
         cmd_stat,
         "Display game stats.",
         "stats");

   IN_GAME_CMDS.handle(
         "load",
         cmd_load,
         "Load an object into the game.",
         "load [name] [x] [y] [z]",
         "Options:\n   -m [sizex] [sizey] [sizez] [randx] [randy] [randz]\n\nload a matrix of objects centered at x,y,z with dimentions\nsizex, sizey, sizez. randx, randy, and randz are floats from\n0 to 1 that describe the likelyhood of an object being loaded\nat each whole number cross section.");

   IN_GAME_CMDS.handle(
         "e",
         cmd_exit,
         "Exit the command line.",
         "e");

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   int glinit = 0;
   char** glinit2;
   glutInit(&glinit, glinit2);

   glutInitWindowSize(700, 500);
   glutInitDisplayMode(GLUT_RGB | GLUT_STENCIL | GLUT_DOUBLE | GLUT_DEPTH);
   glutCreateWindow("Mechanizm");

   glutIdleFunc(g_pause);
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
   CAM.setX(as_double(argv[0]));
   CAM.setY(as_double(argv[1]));
   CAM.setZ(as_double(argv[2]));
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
   selected_glob = argv[0];
}

void cmd_grow(vector<string>& argv) {
   if (argv.size() != 1) {
      menu_output.push_back("invalid arguments: try 'help grow'");
      return;
   }
   int arg = as_int(argv[0]);

   while (arg-- > 0) {
      string gi = selected_glob;

      char buffer[100];
      sprintf(buffer, "generated_object_%i", gen_obj_cnt);
      gen_obj_cnt++;
      string id = buffer;
      Object o(id);

      string robjkey;

      int robji = rand() % gs[gi].vis_objs.size();
      robjkey = gs[gi].vis_objs[robji];

//      string msg = "growing object: " + robjkey;
//      menu_output.push_back(msg);

      int rvfacei = rand() % gs[gi].objs[robjkey].tetra.vis_faces.size();
      int rfacei = gs[gi].objs[robjkey].tetra.vis_faces[rvfacei];
      o.tetra = generate_tetra_from_side(
                     gs[gi].objs[robjkey].tetra, rfacei);

      gs[gi].attach(robjkey, rfacei, o);
   }
}

void cmd_stat(vector<string>& argv) {
   // set GAME_STATS
   GAME_STATS["total_bodies"] = gs.size();
   GAME_STATS["total_blocks"] = 0;
   GAME_STATS["visible_blocks"] = 0;
   for (auto git = gs.begin(); git != gs.end(); git++) {
      GAME_STATS["total_blocks"] += gs[git->first].objs.size();
      GAME_STATS["visible_blocks"] += gs[git->first].vis_objs.size();
   }

   for (auto gsit = GAME_STATS.begin(); gsit != GAME_STATS.end(); gsit++) {
      string msg = gsit->first;
      char buffer[100];
      sprintf(buffer, ": %i", GAME_STATS[gsit->first]);
      msg += buffer;
      menu_output.push_back(msg);
   }
}

void cmd_load(vector<string>& argv) {

   bool matrix = false;
   unsigned long sizex, sizey, sizez;
   GLfloat randx, randy, randz;
   string msg;
   Vertex v;

   vector<string> nargv(0);
   for (int argi = 0; argi < argv.size(); argi++) {
      if (argv[argi] == "-m") {
         matrix = true;

         if (argi+6 >= argv.size()) {
            menu_output.push_back("invalid argument sequence.");
            return;
         }

         sizex = as_double(argv[argi+1]);
         sizey = as_double(argv[argi+2]);
         sizez = as_double(argv[argi+3]);

         randx = as_double(argv[argi+4]);
         randy = as_double(argv[argi+5]);
         randz = as_double(argv[argi+6]);

         argi += 6;
      }
      else {
         nargv.push_back(argv[argi]);
      }
   }

   if (nargv.size() < 4) {
      menu_output.push_back("try `help load`.");
      return;
   }

   string name = nargv[0];

   v.x = as_double(nargv[1]);
   v.y = as_double(nargv[2]);
   v.z = as_double(nargv[3]);

   if (!matrix) {
      // if name exists load or copy from gs, otherwise generate tetrahedron
      msg = "loading new tetrahedron " + name;
      Object obj(name + "_obj_0");
      Glob g(obj);
      Vertex c = obj.tetra.center();
      g.translate_by(-c.x, -c.y, -c.z);

      g.translate_by(v.x, v.y, v.z);
      gs[name] = g;
      selected_glob = name;

      if (nargv.size() >= 5) {
         vector<string> tmp(1);
         tmp[0] = nargv[4];
         cmd_grow(tmp);
      }

      menu_output.push_back(msg);
   }
   else {
      // get points to generate blocks
      unsigned long int gen_obj_cnt = 1;
      for (GLfloat xi = v.x - (sizex/2.0); xi < v.x+(sizex/2.0); xi += 1.0) {
         for (GLfloat yi = v.y-(sizey/2.0); yi < v.y+(sizey/2.0); yi += 1.0) {
            for (
              GLfloat zi = v.z-(sizez/2.0); zi < v.z+(sizez/2.0); zi += 1.0) {

               bool x_ok, y_ok, z_ok = false;
               if (1000.0 * randx > rand() % 1001) x_ok = true;
               if (1000.0 * randy > rand() % 1001) y_ok = true;
               if (1000.0 * randz > rand() % 1001) z_ok = true;

               if (x_ok && y_ok && z_ok) {
                  
                  char buffer[100];
                  sprintf(buffer, "_matrix_g_obj_%i", gen_obj_cnt);
                  gen_obj_cnt++;
                  string g_obj_name = name + buffer;

                  cout << "generating block " << g_obj_name;
                  cout << " at (" << xi << " " << yi << " " << zi << ")\n";

                  Object obj(g_obj_name);
                  Glob g(obj);
                  Vertex c = g.objs[g_obj_name].tetra.center();
                  g.translate_by(-c.x, -c.y, -c.z);
                  g.translate_by(xi, yi, zi);
                  gs[g_obj_name] = g;

                  if (nargv.size() >= 5) {
                     vector<string> tmp(1);
                     tmp[0] = nargv[4];
                     selected_glob = g_obj_name;
                     cmd_grow(tmp);
                  }
               }
            }
         }
      }
   }
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

   tryhard_motion(t, tmp);

   self.set_cube(tmp.cube);
   self.last_t = t;

   return NULL;
}

tools::Error spawned_motion(double t, Object &self) {

   //if (self.cube[0][1] <= 0.0000001) {
   //   glutIdleFunc(g_pause);
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

// TODO: incorperate into drawVisibleTriangles and add options
//void
//drawWireframe(int objs_index, int face)
//{
//   int i, j = objs_index;
//   glBegin(GL_LINE_LOOP);
//   for (i = 0; i < 4; i++)
//      //glVertex3fv((GLfloat *) objs[j].cube[(objs[j].faceIndex[face][i])]);
//   glEnd();
//}

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
      CAM.pos_in_los(SD, nsx, nsy, nsz);

      // set the sphere position
      sphrs[0].center[0] = nsx;
      sphrs[0].center[1] = nsy;
      sphrs[0].center[2] = nsz;

      int i = -1;
      int sidx, sidy, sidz;

      // check for collision with glob
      for (auto git = gs.begin(); git != gs.end(); git++) {

      string gi = git->first;

      // check each visible object in glob gs[gi]
      for (int voi = 0; voi < gs[gi].vis_objs.size(); voi++) {

         // get object id
         string j = gs[gi].vis_objs[voi];

         if (gs[gi].objs[j].shape == "tetrahedron") {

            // get line segment points
            Vertex line[2];
            line[0].x = nsx;
            line[0].y = nsy;
            line[0].z = nsz;

            GLfloat tx, ty, tz;
            CAM.pos_in_los(SD+0.08f, tx, ty, tz);
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
   }
   else {
      CAM.pos_in_los(SD,
            sphrs[0].center[0], sphrs[0].center[1], sphrs[0].center[2]);
   }


   // draw the sphere
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
   CAM.rotation(x, y);
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

   glClearColor(0.0, 0.0, 0.025, 0.0); // background color
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();

   //// set up camera
   //glViewport(0, 0, 500, 500);

   glLoadIdentity();
   CAM.translation();
   glRotatef(180 * CAM.getAY() / 3.141592653, 1.0, 0.0, 0.0);
   glRotatef(180 * CAM.getAX() / 3.141592653, 0.0, 1.0, 0.0);
   glTranslated(-CAM.getX(), -CAM.getY(), -CAM.getZ());

   // draw stuff
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);

   glBegin(GL_TRIANGLE_STRIP);
   glColor3f(1, 0, 0); glVertex3f(0, 0, 0);
   glColor3f(0, 1, 0); glVertex3f(-1, 0, 1);
   glColor3f(0, 0, 1); glVertex3f(1, 0, 1);
   glEnd();

   draw_circle(0.0, 1.0, 3.0, 36);

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

   // draw visible triangles in globs map
   for (auto glit = gs.begin(); glit != gs.end(); glit++) {
      string gid = glit->first;

      for (int voit = 0; voit < gs[gid].vis_objs.size(); voit++) {
         string oid = gs[gid].vis_objs[voit];
         drawVisibleTriangles(gs[gid].objs[oid].tetra);
      }
   }

   if (menu_depth != 0 || timed_menu_display-- > 0) {
      // write character bitmaps
      draw_bitmap_string(
         // these calculate the location in front of the face
         CAM.getX() + (cos(CAM.getAX() - 2*(M_PI/3)) * cos(-CAM.getAY()))*1.3,
         //CAM.getY() + sin(-CAM.getAY())*1.3 - 0.5,
         CAM.getY() - 0.5,
         CAM.getZ() + (sin(CAM.getAX() - 2*(M_PI/3)) * cos(-CAM.getAY()))*1.3,
         GLUT_BITMAP_9_BY_15,
         menu_input);

      // draw the output next
      draw_menu_output(
         // these calculate the location in front of the face
         CAM.getX() + (cos(CAM.getAX() - 2*(M_PI/3)) * cos(-CAM.getAY()))*1.3,
         CAM.getY() - 0.5,
         CAM.getZ() + (sin(CAM.getAX() - 2*(M_PI/3)) * cos(-CAM.getAY()))*1.3,
         GLUT_BITMAP_9_BY_15);

//         CAM.getX() + (cos(CAM.getAX() - M_PI/2) * cos(-CAM.getAY()))*1.3,
//         (GLfloat) (CAM.getY() + sin(-CAM.getAY())*1.3 - 0.33),
//         CAM.getZ() + (sin(CAM.getAX() - M_PI/2) * cos(-CAM.getAY()))*1.3,
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

//   gluLookAt(CAM.getX(), CAM.getY(), CAM.getZ(), 0, 0, 0, 0, 1, 0);


   //glRotatef(CAM.getTheta(), 1.0, 0.0, 0.0);
   //glRotatef(CAM.getPsi(), 0.0, 1.0, 0.0);
   //glTranslated(-CAM.getX(), -CAM.getY(), -CAM.getZ());

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void map_generation(void) {

}

void animation(void) {

   tools::Error e = NULL;
   // 1/64th of a second
   e = MECH.run(0.015625, 0, 0.015625);
   if (e != NULL) {
      cout << e;
      return;
   }
   //cout << "count: " << count << " t: " << MECH.current_time << "\n";
   //glutPostRedisplay();

   if (count == 60) { // make up for the time lost
      e = MECH.run(0.046875, 0, 0.046875);
      cout << "last t: " << MECH.current_time << "\n";

   }
   if (e != NULL) {
      cout << e;
      return;
   }
}

void g_pause(void) {
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
   }

   DRAW_GLUT_MENU = true;
   glutPostRedisplay();
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
      sprintf(buffer, "  -  time: %f\n", MECH.current_time); msg += buffer;

      // run menu 1 level buttons
      if (c == 'e') {
         menu_depth = 37;
         draw_menu_lines = 20;
         menu_input = "^> ";
         cout << prefix << "entering command mode:\n";
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
      return;
   }
   if (c == 'e') {
      menu_input = "^> ";
      menu_depth = 37;
      draw_menu_lines = 25;
      return;
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

      selected_glob = id;

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
      CAM.pos_in_los(SD, nx, ny, nz);

      spawn_glob.translate_by(nx, ny, nz);
      gs[spawn_glob.id] = spawn_glob;
      return;
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
      //exit(0);
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

      if (CAM.pressKey(c)) {
         glutPostRedisplay();
         break;
      }

      // loop through config and find hotkey entries
      for (auto it = CMDS_STORE.begin(); it != CMDS_STORE.end(); it++) {
         string key = it->first;

         if (key.size() == 8 && key.substr(0, 6) == "hotkey") {
            string hkey = key.substr(7, 1);

            if (hkey[0] == c) {
               
               string cmd = CMDS_STORE[key];
               if (cmd.substr(0, 4) == "grow") {
                  // run grow command
                  vector<string> cmd_argv;
                  cmd_argv.push_back(cmd.substr(5, 50));
                  IN_GAME_CMDS.run("grow", cmd_argv);
               }
            }
         }
      }

      break;
  }
}

void keyboard_up(unsigned char c, int x, int y) {
   CAM.setKeyboard(c, false);
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
