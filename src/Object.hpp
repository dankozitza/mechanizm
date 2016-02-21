// 
// Object.hpp
//
// Created by Daniel Kozitza
//

#ifndef _MECH_OBJECT
#define _MECH_OBJECT

#include <string>
#include <vector>
#include "tools.hpp"
#include "Object.hpp"

using namespace std;


class Object {

   public:

      typedef tools::Error (*Function)(double, Object&);

      struct Position {
         double x;
         double y;
         double z;
      };

      string   id;
      Position initial_pos;
      Position current_pos;
      double   last_run_time; // used to determine whether or not to run func
      double   mass;
//      vector<string> connected; // list of objects this object is connected to
      // this function describes the objects motion as a function of time.
      // It will be used by mechanizm to get the objects position.
      // the reference passed to it is a reference to this very object.
      Function func_Motion;

      Object();
      Object(
            string identity,
            Position initial_position,
            double mass_d,
            Function function_Motion);
};

#endif
