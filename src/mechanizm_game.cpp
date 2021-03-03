//
// mechanizm_game.cpp
//
// Asteroid mining and block building game.
//
// Created by Daniel Kozitza
//

#include <chrono>
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

void
   animation(void),
   setMatrix(int w, int h),
   map_generation(void),
   pricion(void),
   g_pause(void),
   help(string prog_name),
   menu(int choice),
   resize(int w, int h),
   mouse(int button, int state, int x, int y),
   mouse_motion(int x, int y),
   keyboard(unsigned char c, int x, int y),
   keyboard_up(unsigned char c, int x, int y),
   motion(int x, int y),
   drawScene(void),
   drawWireframe(int objs_index, int face),
   drawVisibleTriangles(string gsid, string obid, Tetrahedron& tetra),
   draw_glut_menu(),
   draw_circle(float cx, float cy, float r, int num_segments),
   draw_cam_spheres(),
   draw_sphere(Sphere& s),
   draw_spheres(),
   reset_sphere_distance();

tools::Error random_motion(double t, Object &self);
tools::Error player_gravity(double t, Object &self);

Tetrahedron generate_tetra_from_side(Tetrahedron& source, int source_face);
tools::Error save_map(string mapname);
tools::Error load_map(string mapname);

unordered_map<string, Glob> GS;
vector<Sphere> SPHRS;
string select_gobj;
string select_obj;
int select_face;
long unsigned int gen_obj_cnt = 0;
long unsigned int gen_map_cnt = 1;
string selector_function = "select";
int draw_x_vertices = 0;
int menu_depth = 0;
int timed_menu_display = 0;
int draw_menu_lines = 10;
string menu_input;
vector<string> menu_output;
int block_count = 1;
int mouse_left_state = 1;
int WIN = -1;

// X Y Z theta (ax) psi (ay) rotationSpeed translationSpeed
static Camera CAM(2.0, 2.0, 3.5, 0.0, 0.0, 0.6, 0.0022);

static mechanizm MECH;
vector<Object> M_OBJS;
int PO = 0;
int count = 1; // cycles through 1/60th of a second

commands IN_GAME_CMDS;
map<string, string> CMDS_STORE;
int IGCMD_MOUT_SCROLL = 0;
map<string, unsigned long> GAME_STATS;
GLfloat SD; // current distance of the selection sphere
bool SD_DONE = true;

bool DRAW_GLUT_MENU = false;

// menu options
bool MAP_GEN = false;
bool ANIMATION = false;
bool MOUSE_GRAB = false;

// commands for the in-game cli
void
   cmd_help(vector<string>& argv),
   cmd_tp(vector<string>& argv),
   cmd_set(vector<string>& argv),
   cmd_grow(vector<string>& argv),
   cmd_stat(vector<string>& argv),
   cmd_load(vector<string>& argv),
   cmd_save_map(vector<string>& argv),
   cmd_load_map(vector<string>& argv),
   cmd_reseed(vector<string>& argv),
   cmd_controls(vector<string>& argv);

int main(int argc, char *argv[]) {

   char buf[100];
   sprintf(buf, "%d", time(NULL));
   CMDS_STORE["rng_seed"] = buf;

   srand(as_double(CMDS_STORE["rng_seed"]));
   string prog_name = string(argv[0]);
   options opt;
   bool quiet       = false;
   bool show_help   = false;
   bool file_given  = false;
   bool realtime    = false;
   bool time        = false;
   bool seed_b      = false;
   vector<string> seed_argv(1);
   vector<string> file_path_av(1);
   string m[5];
   vector<string> args;
   Error e = NULL;

   Object tmp_player_obj("player_grav_obj", 1.0, player_gravity);
   tmp_player_obj.setConstQ("AGM_GRAVITY", 9.8);
   tmp_player_obj.setConstQ("AGM_FALLING", 0.0);
   tmp_player_obj.setConstQ("AGM_FSTARTT", 3.0);
   tmp_player_obj.setConstQ("AGM_FSTARTY", 5.0);
   tmp_player_obj.translate_by(10.0, 0.0, 10.0);
   M_OBJS.push_back(tmp_player_obj);
   MECH.set_objects(M_OBJS);
   MECH.current_time = 0.0;

   // build handler for glut window / game save
   signal(SIGINT, signals_callback_handler);

   opt.handle('q', quiet);
   opt.handle('h', show_help);
   opt.handle('f', file_given, file_path_av);
   opt.handle('r', realtime);
   opt.handle('s', seed_b, seed_argv);

   for (int i = 1; i < argc; ++i)
      args.push_back(argv[i]);

   opt.evaluate(args);

   if (pmatches(m, prog_name, "^.*/([^/]+)$"))
      prog_name = m[1];

   if (show_help) {
      help(prog_name);
      return 0;
   }

   if (seed_b) {
      srand(as_double(seed_argv[0]));
      CMDS_STORE["rng_seed"] = seed_argv[0];
   }

   // set up the in game command line
   vector<string> cmd_argv;

   CMDS_STORE["agm_climb_spd"]         = "0.05";
   CMDS_STORE["n_click_speed"]         = "0.25";
   CMDS_STORE["n_click_max_distance"]  = "4.0";
   CMDS_STORE["n_click_distance"]      = "0.6";
   CMDS_STORE["map_name"]              = "map.json";
   CMDS_STORE["mapgen_rndmns"]         = "0.0005";
   CMDS_STORE["mapgen_g_opt"]          = "r";
   CMDS_STORE["mapgen_size"]           = "500";
   CMDS_STORE["hotkey_g"]              = "grow 1";
   CMDS_STORE["hotkey_G"]              = "grow 500";
   CMDS_STORE["igcmd_mout_max_size"]   = "100";
   CMDS_STORE["igcmd_scroll_speed"]    = "5";
   CMDS_STORE["block_color"]           = "0.25 0.3 0.3";
   CMDS_STORE["player_move_speed"]     = "0.001";

   // X Y Z theta (ax) psi (ay) rotationSpeed translationSpeed
   CAM = Camera(CAM.getX(), CAM.getY(), CAM.getZ(), 0.0, 0.0, 0.5,
                  as_double(CMDS_STORE["player_move_speed"]));

   SD = as_double(CMDS_STORE["n_click_distance"]);
   Sphere selector_sphere_1(0.0, 0.0, 0.0, 0.025, 0.4, 0.8, 0.6);
   SPHRS.push_back(selector_sphere_1);

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
         "save_map",
         cmd_save_map,
         "Save the map.",
         "save_map [mapname]");

   IN_GAME_CMDS.handle(
         "grow",
         cmd_grow,
         "Generate x tetrahedrons from selected object.",
         "grow [x]");

   IN_GAME_CMDS.handle(
         "stat",
         cmd_stat,
         "Display game stats.",
         "stat");

   IN_GAME_CMDS.handle(
         "load",
         cmd_load,
         "Load an object into the game.",
         "load [name] [x] [y] [z]",
         "Options:\n   -m [sizex] [sizey] [sizez] [randomness]\n      Load a matrix of objects centered at x,y,z with dimentions\n      sizex, sizey, sizez. randomness is a float from\n      0 to 1 that describes the likelyhood of an object being loaded.\n   -s [unit_size]\n      Sets the unit size of the 3d grid. default: 1.0\n   -g [count]\n      Grow each object count new blocks. `-g r` selects a\n      random count from 0 to 1000.");

   IN_GAME_CMDS.handle(
         "load_map",
         cmd_load_map,
         "Load a saved map from the given json file.",
         "load_map [file_name.json]");

   IN_GAME_CMDS.handle(
         "reseed",
         cmd_reseed,
         "Re-set the rng seed.",
         "reseed [double]",
         "Reseed sets the stored seed to the given double and runs srand.");

   IN_GAME_CMDS.handle(
         "controls",
         cmd_controls,
         "Enter 'help controls' for a list of controls.",
         "help controls",
         "Controls:\n      e                 Open console.\n"
         "      w a s d space c   Movement.\n"
         "      x                 Toggle AGM.\n"
         "      Esc               Grab/ungrab mouse, exit console.\n"
         "      b                 Spawn a block.\n"
         "      i j k l h n       Move selected block.\n"
         "      +/-               Add or subtract number of points drawn.\n"
         "      1                 Set mouse left click mode to 'select'.\n"
         "      2                 Set mouse left click mode to 'remove'.\n"
         "      3                 Set mouse left click mode to 'build'.\n"
         "      4                 Set mouse left click mode to 'paint'.\n"
         "      t                 Hold t and click in paint mode to copy color.\n");

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   int glinit = 0;
   char** glinit2;
   glutInit(&glinit, glinit2);

   glutInitWindowSize(800, 600);
   glutInitDisplayMode(GLUT_RGB | GLUT_STENCIL | GLUT_DOUBLE | GLUT_DEPTH);
   WIN = glutCreateWindow("Mechanizm");

   glutIdleFunc(g_pause);
   glutDisplayFunc(drawScene);
   glutMouseFunc(mouse);
   glutMotionFunc(mouse_motion);
   glutPassiveMotionFunc(mouse_motion);
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
   else if (argv.size() >= 2) {

      for (int i = 2; i < argv.size(); i++) {
         argv[1] += " " + argv[i];
      }

      CMDS_STORE[argv[0]] = argv[1];
      msg = "setting ";
      msg += argv[0];
      msg += ": ";
      msg += CMDS_STORE[argv[0]];
      menu_output.push_back(msg);
   }

   // update cam in case settings are modified
   CAM = Camera(CAM.getX(), CAM.getY(), CAM.getZ(), CAM.getAX(), CAM.getAY(),
                  0.5, as_double(CMDS_STORE["player_move_speed"]));
}

void cmd_save_map(vector<string>& argv) {
   if (argv.size() > 0) {
      CMDS_STORE["map_name"] = argv[0];
   }

   menu_output.push_back("saving map: " + CMDS_STORE["map_name"]);
   save_map(CMDS_STORE["map_name"]);
}

void cmd_grow(vector<string>& argv) {
   if (argv.size() != 1) {
      menu_output.push_back("invalid arguments: try 'help grow'");
      return;
   }
   int arg = as_int(argv[0]);

   while (arg-- > 0) {
      string gi = select_gobj;

      if (GS[gi].vis_objs.size() == 0) {
         cout << "cmd_grow: gobj " << gi << " has 0 visible objects\n";
         arg--;
         continue;
      }

      char buffer[100];
      sprintf(buffer, "generated_object_%i", gen_obj_cnt);
      gen_obj_cnt++;
      string id = buffer;
      Object o(id);

      string robjkey;

      int robji = rand() % GS[gi].vis_objs.size();
      robjkey = GS[gi].vis_objs[robji];

      if (GS[gi].objs[robjkey].tetra.vis_faces.size() < 1) {
         cout << "cmd_grow: ERROR random object "
              << robjkey << " has no vis faces\n";
      }

      int rvfacei = rand() % GS[gi].objs[robjkey].tetra.vis_faces.size();
      int rfacei = GS[gi].objs[robjkey].tetra.vis_faces[rvfacei];
      o.tetra = generate_tetra_from_side(
                     GS[gi].objs[robjkey].tetra, rfacei);

      select_obj = robjkey;
      tools::Error e = GS[gi].attach(robjkey, rfacei, o);
      if (e != NULL) {
         cout << "cmd_grow: error generating tetrahedron from object "
              << robjkey << " face " << rfacei << ".\nERROR: " << e << endl;
         cout << "cmd_grow: re-running attach_interior_sides.\n";

         // run attach_interior_faces here to fix remaining interior sides
         e = NULL;
         e = GS[gi].attach_interior_sides(robjkey);
         if (e != NULL) {
            cout << "cmd_grow: error running attach_interior_sides("
                 << robjkey << ").\nERROR: " << e << endl;
         }

         //TODO: fix remaining visible sides left over from grow cmd.
         //GS[gi].objs[robjkey].tetra.remove_vis_face(rfacei);
         //GS[gi].objs[robjkey].tetra.set_faceColors(Tetrahedron().fc_blue);
         //GS[gi].objs[robjkey].tetra.faceColors[rfacei][0] = 0.8;
         //GS[gi].objs[robjkey].tetra.faceColors[rfacei][2] = 0.2;
         //if (GS[gi].objs[robjkey].tetra.vis_faces.size() == 0) {
         //   GS[gi].remove_vis_obj(robjkey);
         //}
         //cout << "cmd_grow::" << e << "\n";
         //cout << "cmd_grow: removing face " << rfacei << " from "
         //     << "object (blue) " << robjkey << ".\n";
         //return;
      }
   }
}

void cmd_stat(vector<string>& argv) {
   // set GAME_STATS

   GAME_STATS["total_bodies"] = GS.size();
   GAME_STATS["total_blocks"] = 0;
   GAME_STATS["visible_blocks"] = 0;
   GAME_STATS["mapped_sections"] = M().size();
   for (auto git = GS.begin(); git != GS.end(); git++) {
      GAME_STATS["total_blocks"] += GS[git->first].objs.size();
      GAME_STATS["visible_blocks"] += GS[git->first].vis_objs.size();
   }

   for (auto gsit = GAME_STATS.begin(); gsit != GAME_STATS.end(); gsit++) {
      string msg = gsit->first;
      char buffer[100];
      sprintf(buffer, ": %i", GAME_STATS[gsit->first]);
      msg += buffer;
      menu_output.push_back(msg);
   }

   return;
}

void cmd_load(vector<string>& argv) {

   bool matrix = false;
   unsigned long sizex, sizey, sizez;
   int grow_i = -1;
   GLfloat gsc = 1.0; // grid scale
   GLfloat rndmns;
   string msg;
   Vertex v;

   vector<string> nargv(0);
   for (int argi = 0; argi < argv.size(); argi++) {
      if (argv[argi] == "-m") {
         matrix = true;

         if (argi+4 >= argv.size()) {
            menu_output.push_back("invalid argument sequence.");
            return;
         }

         sizex = as_double(argv[argi+1]);
         sizey = as_double(argv[argi+2]);
         sizez = as_double(argv[argi+3]);

         rndmns = as_double(argv[argi+4]);

         argi += 4;
      }
      else if (argv[argi] == "-s") {
         gsc = as_double(argv[argi+1]);
         argi++;
      }
      else if (argv[argi] == "-g") {
         grow_i = argi+1;
         argi++;
      }
      else {
         nargv.push_back(argv[argi]);
      }
   }

   if (nargv.size() != 4) {
      menu_output.push_back("Invalid arguments. Try `help load`.");
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
      Glob g(name, obj);
      Vertex c = obj.tetra.center();
      g.translate_by(-c.x, -c.y, -c.z);

      g.translate_by(v.x, v.y, v.z);
      GS[name] = g;
      select_gobj = name;

      if (grow_i > 0) {
         vector<string> tmp(1);
         tmp[0] = argv[grow_i];
         cmd_grow(tmp);
      }

      menu_output.push_back(msg);
   }
   else {
      // get points to generate blocks
      //unsigned long int gen_obj_cnt = 1;
      for (GLfloat xi = v.x - (sizex/2.0); xi < v.x+(sizex/2.0); xi += gsc) {
         for (GLfloat yi = v.y-(sizey/2.0); yi < v.y+(sizey/2.0); yi += gsc) {
            for (
              GLfloat zi = v.z-(sizez/2.0); zi < v.z+(sizez/2.0); zi += gsc) {

               if (10000.0 * rndmns > rand() % 10001) {

                  char buffer[100];
                  sprintf(buffer, "_matrix_g_obj_%i", gen_obj_cnt);
                  gen_obj_cnt++;
                  string g_obj_name = name + buffer;

                  cout << "generating block " << g_obj_name;
                  cout << " at (" << xi << " " << yi << " " << zi << ")\n";

                  Object obj(g_obj_name);
                  Glob g(g_obj_name, obj);
                  Vertex c = g.objs[g_obj_name].tetra.center();
                  g.translate_by(-c.x, -c.y, -c.z);
                  g.translate_by(xi, yi, zi);
                  GS[g_obj_name] = g;
                  GS[g_obj_name].objs[g_obj_name].setConstQ(
                        "v", {0.0, 0.0, -0.05});

                  if (grow_i > 0) {
                     vector<string> tmp(1);
                     if (argv[grow_i][0] != 'r') {
                        tmp[0] = argv[grow_i];
                     }
                     else {
                        int rcnt = rand() % 1000;
                        sprintf(buffer, "%i", rcnt);
                        tmp[0] = buffer;
                     }
                     select_gobj = g_obj_name;
                     cmd_grow(tmp);
                  }
               }
            }
         }
      }
   }
}

void cmd_load_map(vector<string>& argv) {

   GS.clear();
   M().clear();
   gen_obj_cnt = 0;
   gen_map_cnt = 1;

   string mname = CMDS_STORE["map_name"];
   if (argv.size() > 0) {
      mname = argv[0];
   }
   menu_output.push_back("loading map: " + mname);
   tools::Error e = load_map(mname);
   if (e != NULL) {
      menu_output.push_back("load_map: Error loading map '" + mname + "':");
      menu_output.push_back(e);
   }
   return;
}

void cmd_reseed(vector<string>& argv) {
   if (argv.size() >= 1) {
      CMDS_STORE["rng_seed"] = argv[0];
   }
   
   srand(as_double(CMDS_STORE["rng_seed"]));
}

void cmd_controls(vector<string>& argv) {return;}

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

tools::Error player_gravity(double t, Object &self) {

   // check for collision with object below cam, if collision is detected
   // move player above point of intersection if detected
   // loop through all visible sides, check if line intersects triangles
   //if ((count + 1) % 2 == 0) {
   //
   GLfloat falling = self.getConstQ("AGM_FALLING")[0];
   if (falling < 0.5)  {
      return NULL;
   }

   Vertex cc;
   cc.x = CAM.getX();
   cc.y = CAM.getY();
   cc.z = CAM.getZ();
   string cc_om_key = om_get_section_key(cc);

   vector<string> sections;
   om_get_nearby_sections(cc_om_key, sections);

   for (int si = 0; si < sections.size(); si++) {

      auto mit = M().find(sections[si]);
      if (mit == M().end()) {continue;}

      for (int omvi = 0; omvi < mit->second.size(); omvi++) {
         string gi = mit->second[omvi].gid;
         string j = mit->second[omvi].oid;

         Vertex line[2];
         line[0] = M_OBJS[PO].tetra.center();
         line[0].y += 0.3;

         line[1].x = M_OBJS[PO].tetra.center().x;
         line[1].z = M_OBJS[PO].tetra.center().z;
         line[1].y = M_OBJS[PO].tetra.center().y - 1.0;

         // check each visable face
         Tetrahedron tetra = GS[gi].objs[j].tetra;
         for (int vfi = 0; vfi < tetra.vis_faces.size();
               vfi++) {
            int fi = tetra.vis_faces[vfi];
            Vertex tripnts[3];

            // get cam position above center of intersecting face.
            Vertex c;
            tetra.face[fi].center(c);
            GLfloat new_cam_y = c.y + 0.85;

            for (int tripti = 0; tripti < 3; tripti++) {
               tripnts[tripti].x =
                  *tetra.face[fi].pnts[tripti][0];
               tripnts[tripti].y =
                  *tetra.face[fi].pnts[tripti][1];
               tripnts[tripti].z =
                  *tetra.face[fi].pnts[tripti][2];
            }

            if (tools::line_intersects_triangle(
                     line,
                     tripnts,
                     NULL)) {
               //cout << "player_gravity: intersection with object!\n";
               M_OBJS[PO].translate_by(-M_OBJS[PO].tetra.center().x,
                                       -M_OBJS[PO].tetra.center().y,
                                       -M_OBJS[PO].tetra.center().z);
               M_OBJS[PO].translate_by(CAM.getX(), new_cam_y, CAM.getZ());

               GLfloat climb_mv = as_double(CMDS_STORE["agm_climb_spd"]);
               if (CAM.getY() + climb_mv < new_cam_y) {
                  CAM.setY((CAM.getY() + climb_mv));
               }
               else {
                  CAM.setY(new_cam_y);
               }

               M_OBJS[PO].setConstQ("AGM_FSTARTT", MECH.current_time);
               // skip the rest because we are at rest on a surface
               return NULL;
            }
         }
      }
   }

   GLfloat gravity = self.getConstQ("AGM_GRAVITY")[0];
   GLfloat fstartt = self.getConstQ("AGM_FSTARTT")[0];
   GLfloat fstarty = self.getConstQ("AGM_FSTARTY")[0];

   if (self.last_t < fstartt) {
      self.last_t = fstartt;
   }

   // use AGM_FSTARTT to calculate fall distance
   GLfloat tt = t - fstartt;
   GLfloat tlast_t = self.last_t - fstartt;

   GLfloat movement = 0.5*gravity*tt*tt -
                      0.5*gravity*tlast_t*tlast_t;

   GLfloat camy = CAM.getY() - movement;
   CAM.setY(camy);

   self.translate_by(0.0, -movement, 0.0);
   self.last_t = t;

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
   cout << "   h             - Print this information.\n";
   cout << "   s <integer>   - Set the random number generator seed.\n\n";
}

void drawVisibleTriangles(string gsid, string obid, Tetrahedron& tetra) {

   int drawn = 0;

   if (draw_x_vertices > 0) {
      for (int pi = 0; pi < draw_x_vertices; pi++) {
         Sphere new_sphere(tetra.points[pi].x,
                           tetra.points[pi].y,
                           tetra.points[pi].z,
                           0.0369, 0.4, 0.8, 0.6);
         draw_sphere(new_sphere);
      }
   }
   else if (draw_x_vertices < 0) {
      // draw player object at feet
      Sphere new_sphere(M_OBJS[PO].tetra.center().x,
                        M_OBJS[PO].tetra.center().y - 0.8,
                        M_OBJS[PO].tetra.center().z,
                        0.1369, 0.4, 0.8, 0.6);
      draw_sphere(new_sphere);
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
         //if (drawn > draw_x_vertices) {
         //   glEnd();
         //   return;
         //}
      }

      if (gsid == select_gobj && obid == select_obj) {
         glEnd();
         glBegin(GL_LINE_LOOP);
         glColor3f(0.0, 0.0, 0.0);
         for (int pi = 0; pi < 3; pi++) {
            Vertex tmpv;
            tmpv.x = *tetra.face[fi].pnts[pi][0];
            tmpv.y = *tetra.face[fi].pnts[pi][1];
            tmpv.z = *tetra.face[fi].pnts[pi][2];

            glVertex3f(
               tmpv.x,
               tmpv.y,
               tmpv.z);
         }
         glEnd();
         glBegin(GL_TRIANGLES);
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

   if (menu_depth != 0) {return;}

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
      SPHRS[0].center[0] = nsx;
      SPHRS[0].center[1] = nsy;
      SPHRS[0].center[2] = nsz;

      int i = -1;
      int sidx, sidy, sidz;

      // check for collision with nearby objects
      Vertex cc;
      cc.x = CAM.getX();
      cc.y = CAM.getY();
      cc.z = CAM.getZ();
      string cc_om_key = om_get_section_key(cc);

      vector<string> sections;
      om_get_nearby_sections(cc_om_key, sections);

      for (int si = 0; si < sections.size(); si++) {

      auto mit = M().find(sections[si]);
      if (mit == M().end()) {continue;}

      for (int omvi = 0; omvi < mit->second.size(); omvi++) {
         string gi = mit->second[omvi].gid;
         string j = mit->second[omvi].oid;

         Tetrahedron tetra = GS[gi].objs[j].tetra;

         if (GS[gi].objs[j].shape == "tetrahedron") {

            // get line segment points
            Vertex line[2];
            line[0].x = nsx;
            line[0].y = nsy;
            line[0].z = nsz;

            GLfloat tx, ty, tz;
            CAM.pos_in_los(SD+n_s, tx, ty, tz);
            line[1].x = tx;
            line[1].y = ty;
            line[1].z = tz;

            glBegin(GL_LINE_LOOP);
            glVertex3f(
               line[0].x,
               line[0].y,
               line[0].z);
            glVertex3f(
               line[1].x,
               line[1].y,
               line[1].z);
            glEnd();

            // get triangle points of each visible face
            for (int vfi = 0; vfi < tetra.vis_faces.size();
                 vfi++) {
               Vertex tripnts[3];
               int fi = tetra.vis_faces[vfi];

               for (int tripti = 0; tripti < 3; tripti++) {
                  tripnts[tripti].x = *tetra.face[fi].pnts[tripti][0];
                  tripnts[tripti].y = *tetra.face[fi].pnts[tripti][1];
                  tripnts[tripti].z = *tetra.face[fi].pnts[tripti][2];
               }

               if (tools::line_intersects_triangle(
                        line,
                        tripnts,
                        NULL)) {

                  // glob: gi object: j
                  if (selector_function == "select") {
                     select_gobj = gi;
                     select_obj = j;

                     string msg("selected glob: " + gi + "\nobject: " + j);
                     char buffer[100];
                     sprintf(buffer,
                             ", face %i, vis_faces: %i",
                             fi,
                             tetra.vis_faces.size());

                     msg += buffer;
                     menu_output.push_back(msg);
                     timed_menu_display = 150;
                     draw_menu_lines = 2;

                     reset_sphere_distance();
                     return;
                  }
                  else if (selector_function == "remove") {

                     GS[gi].detach(j);

                     SD = as_double(CMDS_STORE["n_click_distance"]);
                     SD_DONE = true;

                     menu_output.push_back(
                           "glob: " + gi + ": removed object " + j);
                     timed_menu_display = 150;
                     draw_menu_lines = 1;

                     if (GS[gi].objs.size() == 0) {
                        auto it = GS.find(gi);
                        GS.erase(it);
                     }

                     return;
                  }
                  else if (selector_function == "build") {
                     select_gobj = gi;

                     char buffer[100];
                     sprintf(buffer, "generated_object_%i", gen_obj_cnt++);
                     select_obj = buffer;

                     Object o(buffer);
                     o.tetra = generate_tetra_from_side(tetra, fi);

                     vector<string> m(4);
                     if (pmatches(m,
                        CMDS_STORE["block_color"], "(.+) (.+) (.+)")) {

                        GLfloat os = (GLfloat)(rand() % 10) / 100.0;

                        for (int fi = 0; fi < 4; fi++) {
                           o.tetra.faceColors[fi][0] = as_double(m[1]) + os;
                           o.tetra.faceColors[fi][1] = as_double(m[2]) + os;
                           o.tetra.faceColors[fi][2] = as_double(m[3]) + os;
                        }
                     }
                     else {
                        cout << "build: block color does not match\n";
                     }

                     e = GS[gi].attach(j, fi, o);
                     if (e != NULL) {
                        cout << "draw_cam_spheres::" << e << "\n";
                        cout << "draw_cam_spheres: running "
                             << "attach_interior_sides(" << j << ").\n";
                        e = NULL;
                        e = GS[gi].attach_interior_sides(j);
                        if (e != NULL) {
                           cout << "draw_cam_spheres::" << e << "\n";
                        }
                     }

                     SD = as_double(CMDS_STORE["n_click_distance"]);
                     SD_DONE = true;
                     return;
                  }
                  if (selector_function == "paint") {
                     select_gobj = gi;
                     select_obj = j;

                     vector<string> m(4);
                     if (pmatches(m,
                        CMDS_STORE["block_color"], "(.+) (.+) (.+)")) {

                        GLfloat os = (GLfloat)(rand() % 10) / 100.0;

                        if (CAM._keyboard['t'] == false) {
                           for (int i = 0; i < 4; i++) {

                              GS[gi].objs[j].tetra.faceColors[i][0] =
                                 as_double(m[1]) + os;
                              GS[gi].objs[j].tetra.faceColors[i][1] =
                                 as_double(m[2]) + os;
                              GS[gi].objs[j].tetra.faceColors[i][2] =
                                 as_double(m[3]) + os;
                           }
                        } else {
                           char buf[100];
                           sprintf(buf, "%lf %lf %lf",
                              GS[gi].objs[j].tetra.faceColors[0][0],
                              GS[gi].objs[j].tetra.faceColors[0][1],
                              GS[gi].objs[j].tetra.faceColors[0][2]
                           );
                           CMDS_STORE["block_color"] = string(buf);

                           menu_output.push_back(
                                 "copied color " + string(buf));
                           timed_menu_display = 150;
                           draw_menu_lines = 1;
                        }
                     }

                     SD = as_double(CMDS_STORE["n_click_distance"]);
                     SD_DONE = true;
                     return;
                  }
               }
            }

         } // end of tetrahedron loop
      } // end of obect loop
      } // end of map section loop
   }
   else {
      CAM.pos_in_los(SD,
            SPHRS[0].center[0], SPHRS[0].center[1], SPHRS[0].center[2]);
   }


   // draw the sphere
   glColor3f(SPHRS[0].color[0], SPHRS[0].color[1], SPHRS[0].color[2]);
   glPushMatrix();
   glTranslatef(SPHRS[0].center[0], SPHRS[0].center[1], SPHRS[0].center[2]);
   glutWireSphere(SPHRS[0].r, 8, 6);
   glPopMatrix();

}

void draw_spheres() {
   for (int i = 1; i < SPHRS.size(); i++) {
      draw_sphere(SPHRS[i]);
   }
}

void draw_sphere(Sphere& s) {
      glColor3f(s.color[0], s.color[1], s.color[2]);

      glPushMatrix();
      glTranslatef(s.center[0], s.center[1], s.center[2]);
      //glutWireSphere(s.r, 36, 36);
      glutWireSphere(s.r, 4, 4);
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

   if (state == 0) {
      int scroll_speed = as_int(CMDS_STORE["igcmd_scroll_speed"]);
      if (button == 4) {
         if (IGCMD_MOUT_SCROLL - scroll_speed < 0) {
            IGCMD_MOUT_SCROLL = 0;
         }
         else {
            IGCMD_MOUT_SCROLL = IGCMD_MOUT_SCROLL - scroll_speed;
         }
      }
      if (button == 3) {
         if (IGCMD_MOUT_SCROLL + scroll_speed >=
            as_int(CMDS_STORE["igcmd_mout_max_size"])) {
            IGCMD_MOUT_SCROLL = 0;
         }
         else {
            IGCMD_MOUT_SCROLL += scroll_speed;
         }
      }
   }
}

void mouse_motion(int x, int y) {
   if (MOUSE_GRAB && !DRAW_GLUT_MENU) {
      CAM.rotation(x, y);
   }
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

   int max_lines = as_int(CMDS_STORE["igcmd_mout_max_size"]);
   newlines_to_indices(menu_output);

   while (menu_output.size() > max_lines) {
      menu_output.erase(menu_output.begin());
   }

   int dmli = menu_output.size() - draw_menu_lines;
   while (dmli < 0) { dmli++; }

   for (int i =
        menu_output.size() - 1 - IGCMD_MOUT_SCROLL;
        i >= dmli - IGCMD_MOUT_SCROLL && i > 0; i--) {
      y += 0.0369;
      glColor3f(0.1, 0.69, 0.99);
      glRasterPos3f(x, y, z);
      for (int j = 0; j < menu_output[i].size(); ++j) {
         glutBitmapCharacter(font, menu_output[i][j]);
      }
   }
}

void drawScene(void) {

   if (MOUSE_GRAB && !DRAW_GLUT_MENU) {
      CAM.last_mouse_pos_x = 75;
      CAM.last_mouse_pos_y = 75;
      glutWarpPointer(75, 75);
   }

   glClearColor(0.0, 0.0, 0.025, 0.0); // background color
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();

   //// set up camera
   glLoadIdentity();
   CAM.translation();
   glRotatef(180 * CAM.getAY() / 3.141592653, 1.0, 0.0, 0.0);
   glRotatef(180 * CAM.getAX() / 3.141592653, 0.0, 1.0, 0.0);
   glTranslated(-CAM.getX(), -CAM.getY(), -CAM.getZ());

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
   for (auto glit = GS.begin(); glit != GS.end(); glit++) {
      string gid = glit->first;

      for (int voit = 0; voit < GS[gid].vis_objs.size(); voit++) {
         string oid = GS[gid].vis_objs[voit];
         drawVisibleTriangles(gid, oid, GS[gid].objs[oid].tetra);
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
   }

   glPopMatrix();

   glDisable(GL_STENCIL_TEST);

   glutSwapBuffers();
}

void
setMatrix(int w, int h)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   // set the perspective
   // (angle of sight, width/height ratio, clip near, depth)
   gluPerspective(60, (GLfloat)w / (GLfloat)h, 0.1, 500.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void map_generation(void) {
   char buffer[100];
   sprintf(buffer, "initmapid%d", gen_map_cnt++);

   char size_y[100];
   sprintf(size_y, "%f",
         (GLfloat)(as_double(CMDS_STORE["mapgen_size"]) / 6.0));

   vector<string> targv = {buffer, "0", "0", "0",
      "-g", CMDS_STORE["mapgen_g_opt"], "-s", "10.0",
      "-m", CMDS_STORE["mapgen_size"],
            string(size_y),
            CMDS_STORE["mapgen_size"],
      CMDS_STORE["mapgen_rndmns"]};
   cmd_load(targv);
}

void animation(void) {

   tools::Error e = NULL;
   // 1/64th of a second
   e = MECH.run(0.015625, 0, 0.015625);
   if (e != NULL) {
      cout << "mechanizm_game::animation::" << e << endl;
      return;
   }
   //cout << "count: " << count << " t: " << MECH.current_time << "\n";
   //glutPostRedisplay();

   if (count == 60) { // make up for the time lost
      e = MECH.run(0.046875, 0, 0.046875);
      //cout << "last t: " << MECH.current_time << "\n";

   }
   if (e != NULL) {
      cout << e;
      return;
   }

   // move the player object M_OBJS[0] to cam position
   M_OBJS[0].translate_by(-M_OBJS[0].tetra.center().x, -M_OBJS[0].tetra.center().y, -M_OBJS[0].tetra.center().z);
   M_OBJS[0].translate_by(CAM.getX(), CAM.getY(), CAM.getZ());
   if (M_OBJS[0].getConstQ("AGM_FALLING")[0] < 0.5) {
      M_OBJS[0].setConstQ("AGM_FSTARTY", CAM.getY());
   }
}

void g_pause(void) {
   if (MAP_GEN) {
      MAP_GEN = false;
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
      glutAddMenuEntry("1 - Generate Map", 0);
   }
   else {
      glutAddMenuEntry("0 - Generate Map", 0);
   }
   if (ANIMATION) {
      glutAddMenuEntry("1 - Animation", 1);
   }
   else {
      glutAddMenuEntry("0 - Animation", 1);
   }
   if (MOUSE_GRAB) {
      glutAddMenuEntry("1 - Ungrab Mouse", 2);
   }
   else {
      glutAddMenuEntry("0 - Grab Mouse", 2);
   }
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
   case 2: // Grab Mouse
      if (MOUSE_GRAB) {
         MOUSE_GRAB = false;
      }
      else {
         MOUSE_GRAB = true;
         CAM.last_mouse_pos_x = 75;
         CAM.last_mouse_pos_y = 75;
         glutWarpPointer(75, 75);
      }
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

   if (menu_depth != 0) { // go into menu mode

      if (menu_depth == 37) { // menu 37 is command line input
         if (c != 13 && c != 8 && c != 27) { // 13 is Enter
            menu_input += c;
            //cout << (int)c << "\n";
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
      return;
   }


   // gameplay level controls
   char m0_buffer[100];
   if (c == 'e' || c == '`') {
      menu_input = "^> ";
      menu_depth = 37;
      draw_menu_lines = 25;
      return;
   }
   if (c == 'b') {
      string id ;
      char n[100];
      sprintf(n, "spawned_g_object_%i", GS.size());
      id = n;

      Object tmp(id);
      Glob spawn_glob(id, tmp);

      cout << prefix;
      cout << "spawning glob object: " << id << "\n";

      string msg = prefix;
      msg += "spawning glob object: " + id + "\n";
      menu_output.push_back(msg);
      draw_menu_lines = 2;
      timed_menu_display = 60;

      select_gobj = id;

      GLfloat nx, ny, nz;
      CAM.pos_in_los(SD, nx, ny, nz);

      spawn_glob.translate_by(nx, ny, nz);
      GS[spawn_glob.id] = spawn_glob;
      return;
   }
   if (c == 'x' || c == 'X') { // enable/disable artificial gravity module
      GLfloat f = M_OBJS[PO].getConstQ("AGM_FALLING")[0];

      if (f < 0.5) {
         // make AGM_start_fall();
         ANIMATION = true;
         M_OBJS[PO].setConstQ("AGM_FALLING", 1.0);
         M_OBJS[PO].setConstQ("AGM_FSTARTT", MECH.current_time);
         CAM.agm_enabled = true;
         menu_output.push_back("AGM Enabled");
         timed_menu_display = 60;
         draw_menu_lines = 1;
      }
      else {
         // make AGM_stop_fall();
         M_OBJS[PO].setConstQ("AGM_FALLING", 0.0);
         CAM.agm_enabled = false;
         menu_output.push_back("AGM Disabled");
         timed_menu_display = 60;
         draw_menu_lines = 1;
      }
   }
   if (c == ' ') { // AGM jump
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

   switch (c) {
   case 27: // Esc
      if (MOUSE_GRAB) {
         MOUSE_GRAB = false;
      }
      else {
         MOUSE_GRAB = true;
      }
      break;
   case 'i':
      if (select_gobj != "") {GS[select_gobj].translate_by(0, 0, -0.1);}
      break;
   case 'j':
      if (select_gobj != "") {GS[select_gobj].translate_by(-0.1, 0, 0);}
      break;
   case 'k':
      if (select_gobj != "") {GS[select_gobj].translate_by(0, 0, 0.1);}
      break;
   case 'l':
      if (select_gobj != "") {GS[select_gobj].translate_by(0.1, 0, 0);}
      break;
   case 'h':
      if (select_gobj != "") {GS[select_gobj].translate_by(0, 0.1, 0);}
      break;
   case 'n':
      if (select_gobj != "") {GS[select_gobj].translate_by(0, -0.1, 0);}
      break;
   case 'o':
//      GS[select_gobj].scale_by(1.1, 1.1, 1.1);
      break;
   case 'u':
//      GS[select_gobj].scale_by(0.9, 0.9, 0.9);
      break;
   case 'I':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(7.0*(M_PI/4.0), 0.0, 0.0);
      }
      break;
   case 'J':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(0.0, M_PI/4.0, 0.0);
      }
      break;
   case 'K':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(M_PI/4.0, 0.0, 0.0);
      }
      break;
   case 'L':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(0.0, 7.0*(M_PI/4.0), 0.0);
      }
      break;
   case 'U':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(0.0, 0.0, M_PI/4.0);
      }
      break;
   case 'O':
      if (select_gobj != "") {
         GS[select_gobj].rotate_abt_center(0.0, 0.0, 7.0*(M_PI/4.0));
      }
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
   case '4':
      selector_function = "paint";
      menu_output.push_back("mouse function: paint");
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

tools::Error save_map(string mapname) {

   string jmap = "{\n";

   unsigned long int i = 0;
   for (auto it = GS.begin(); it != GS.end(); it++) {
      jmap += "\"" + it->first + "\": " + it->second.getJSON();

      if (i != GS.size()-1) {jmap += ",\n";}
      i++;
   }

   jmap += "\n}";

   return tools::write_file(mapname, jmap);
}

tools::Error load_map(string mapname) {

   tools::Error e = NULL;
   string msg = "load_map: ";
   Json::Value jv;
   e = load_json_value_from_file(jv, mapname);
   if (e != NULL) {
     msg += e;
     return error(msg);
   }

   for (auto jvit = jv.begin(); jvit != jv.end(); jvit++) {
      string gid = jvit.key().asString();
      string ioid = jv[gid]["g_objs"].begin().key().asString();
      Object obj(ioid);
      Glob nglb(gid, obj);

      for (auto gobjit = jv[gid].begin(); gobjit != jv[gid].end(); gobjit++) {
         string gobjkey = gobjit.key().asString();

         if (gobjkey == "g_vis_objs") {
            vector<string> vobjs;
            for (int i = 0; i < jv[gid][gobjkey].size(); i++) {
               vobjs.push_back(jv[gid][gobjkey][i].asString());
            }
            nglb.set_vis_objs(vobjs);
         }
         if (gobjkey == "g_objs") {
            Json::Value g_objs = jv[gid][gobjkey];
            for (auto g_objsit = g_objs.begin();
                 g_objsit != g_objs.end(); g_objsit++) {
               string obj_id = g_objsit.key().asString();

               vector<string> m(2);
               if (pmatches(m,
                  obj_id, "(\\d+)$")) {
                  if (gen_obj_cnt <= as_int(m[1])) {
                     gen_obj_cnt = as_int(m[1]) + 1;
                  }
               }

               Json::Value objval = *g_objsit;
               Object nobj(obj_id);
               nobj.shape = objval["obj_shape"].asString();
               nobj.gid = gid;

               Json::Value tetraval = objval["obj_tetra"];
               Json::Value pointsval = tetraval["points"];
               for (int ii = 0; ii < 4; ii++) {
                  Json::Value vertval = pointsval[ii];
                  nobj.tetra.points[ii].x = vertval[0].asFloat();
                  nobj.tetra.points[ii].y = vertval[1].asFloat();
                  nobj.tetra.points[ii].z = vertval[2].asFloat();
               }

               vector<int> nvfaces;
               Json::Value vfacesval = tetraval["vis_faces"];
               for (int iii = 0; iii < vfacesval.size(); iii++) {
                  nvfaces.push_back(vfacesval[iii].asInt());
               }
               nobj.tetra.set_vis_faces(nvfaces);

               Json::Value facecolorsval = tetraval["faceColors"];
               for (int ii = 0; ii < 4; ii++) {
                  Json::Value fcval = facecolorsval[ii];
                  nobj.tetra.faceColors[ii][0] = fcval[0].asFloat();
                  nobj.tetra.faceColors[ii][1] = fcval[1].asFloat();
                  nobj.tetra.faceColors[ii][2] = fcval[2].asFloat();
               }

               // populate object map
               nobj.om_tracking = true;
               nobj.last_om_key = om_get_section_key(nobj.tetra.center());
               om_add_obj(nobj.gid, nobj.id, nobj.last_om_key);

               nglb.objs[obj_id] = nobj;
            }
         }
      }

      GS[gid] = nglb;
   }

   return NULL;
}
