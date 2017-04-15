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
      string map_name = "default";

      // esm - existing sections map is a map with keys set to string sid's
      //       and values as a struct of the file id for the files within
      //       the maps/<map_name> directory structure. used to check in
      //       logarithmic time for the existence of a map section saved on
      //       the file system.
      struct EsmV {
         int fid;
      };
      map<string, EsmV> esm;

      // check to see if the section is already loaded into the Q
      bool section_loaded(int x, int y, int z);
      // check the filesystem for an existing map section
      bool section_exists(int x, int y, int z);
      bool section_near_center(int, int, int, int, int, int, int);

   public:
		Q<MapSection> ms;

      struct PidSid {
         int pid;
         int sid[3];
      };

		Map();

      // load_section_list
      // 
      // Load the list of map sections found on the file system.
      //
      void load_section_list();

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
		void update(GLfloat x, GLfloat y, GLfloat z,
            Q<MapSection>& cmsq,
            Q<PidSid>& ps);
};

#endif
