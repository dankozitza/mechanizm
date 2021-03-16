// 
// mechanizm.cpp
//
// Created by Daniel Kozitza
//

#include "mechanizm.hpp"

mechanizm::mechanizm() {
   current_time = 0;
}

void mechanizm::add_object(Object nobj) {
   objs[size] = nobj;
   if (size < objcap) {size++;}
   else {size = 0;}
   return;
}

tools::Error mechanizm::run(double seconds) {
   return run(seconds, 0, 0.001);
}
tools::Error mechanizm::run(double seconds, double skip) {
   return run(seconds, skip, 0.001);
}
tools::Error mechanizm::run(double seconds, double skip, double tick) {
   tools::Error e = NULL;
   double init_time = current_time;

   for ( current_time;
         current_time < init_time + seconds
         && !tools::equal(current_time, init_time + seconds); ) {

      current_time += tick;
      for (int i = 0; i < size; ++i) {
         if (objs[i].function == NULL) {
            continue;
         }
         e = objs[i].function(current_time, objs[i]);
         if (e != NULL) {
            return tools::errorf(
                  "mechanizm::run: at %d seconds object %s returned error %s",
                  current_time, objs[i].id.c_str(), e);
         }
      }
   }
   if (!tools::equal(current_time, init_time + seconds))
      return tools::errorf(
            "mechanizm::run: current time %d != expected time %d\n",
            current_time, init_time + seconds);
   return NULL;
}
