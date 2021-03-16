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

   public:

      double current_time;
      size_t size = 0;
      const static size_t objcap = 100;
      Object objs[objcap];

      mechanizm();

      void add_object(Object nobj);

      tools::Error run(double seconds = 0.0001);
      tools::Error run(double seconds, double skip);
      tools::Error run(double seconds, double skip, double tick);

};

#endif
