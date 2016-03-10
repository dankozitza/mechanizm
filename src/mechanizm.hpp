// 
// mechanizm.hpp
//
// A simple physics simulator.
//
// Created by Daniel Kozitza
//

#ifndef _MECHANIZM
#define _MECHANIZM

#include <map>
#include <string>
#include <vector>
#include "tools.hpp"
#include "Object.hpp"

using namespace std;

class mechanizm {

   private:
      //typedef map<string, Object*>::iterator obj_iter;
      //map<string, Object*> objects;
      vector<Object>* objects;

   public:
      double current_time;

      mechanizm();
      mechanizm(vector<Object> &objs);
      //tools::Error spawn(Object &object);
      void set_objects(vector<Object> &objs);
      //tools::Error unspawn(int i);
      tools::Error run(double seconds = 0.0001);
      tools::Error run(double seconds, double skip);
      tools::Error run(double seconds, double skip, double tick);
};

#endif
