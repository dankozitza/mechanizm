// 
// Map.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_MAP
#define _MECH_MAP

#include <cmath>
#include "tools.hpp"

#include <GL/glut.h>
#include "MapSection.hpp"
#include "Q.hpp"

using namespace tools;

class Map {
   private:
      int sections = 0;

      // esm - existing sections map is a map with keys set to string sid's
      //       and values as a struct of the file id for the files within
      //       the maps/<map_name> directory structure. used to check in
      //       logarithmic time for the existence of a map section saved on
      //       the file system.
      struct EsmV {
         int fid;
      };
      map<string, EsmV> esm;

   public:
      Q<MapSection> ms;
      string map_name = "default";

      struct PidSid {
         int pid;
         int sid[3];
      };

      Map();

      // check the map section queue for a section
      // returns index or -1 for none
      int section_i_loaded(int x, int y, int z);
      bool section_loaded(int x, int y, int z);
      // check the cached map section queue for a section
      bool section_cached(Q<MapSection>& cmsq, int& i,
            int x, int y, int z);
      // check the filesystem for an existing map section
      bool section_exists(int x, int y, int z);
      // check if a section is within 2 * size blocks from center
      bool section_near_center(int, int, int, int, int, int, int);

      // load_section_list
      // 
      // Load the list of existing map sections found on the file system.
      //
      void load_section_list();

      // add_esm_entry
      //
      // Add a single map section to the existing sections map.
      //
      void add_esm_entry(MapSection& ms);

      // update
      //
      // Generate, load, and unload the proper map sections based on the 
      // new position of the camera. cmsq is a reference to a cached map
      // section queue. It's used to cache map sections during the update
      // process. Child process' are assigned to save map sections and
      // exit. ps is a queue of PidSid structs which is used to keep track
      // of each child process id and the section id of the cached map
      // section it is saving. The parent process (from outside of update)
      // iterates the ps queue periodically to retrieve the child pid and
      // delete the proper map section from cmsq.
      //
      void update(GLfloat x, GLfloat y, GLfloat z, Q<MapSection>& cmsq);

      // generate_visible_edges
      //
      // Generate sides using the types of adjacent edge blocks throughout
      // the map sections.
      //
      void generate_visible_edges();
};

#endif
