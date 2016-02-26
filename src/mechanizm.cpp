// 
// mechanizm.cpp
//
// Created by Daniel Kozitza
//

#include "mechanizm.hpp"

mechanizm::mechanizm() {
   current_time = 0;
}

tools::Error mechanizm::spawn(Object &object) {
   objects[object.id] = &object;
   return NULL;
}

tools::Error mechanizm::unspawn(string id) {
   objects[id] = NULL;
   return NULL;
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
   for (
         current_time;
         current_time < init_time + seconds
         && !tools::equal(current_time, init_time + seconds); )
   {
      current_time += tick;
      for (obj_iter oi = objects.begin(); oi != objects.end(); ++oi) {
         e = (*oi->second).func_Motion(current_time, *oi->second);
         if (e != NULL) {
            return tools::errorf(
                  "mechanizm::run: at %d seconds object %s returned error %s",
                  current_time, (*oi->second).id.c_str(), e);
         }
      }
   }
   if (!tools::equal(current_time, init_time + seconds))
      return tools::errorf(
            "mechanizm::run: current time %d != expected time %d\n",
            current_time, init_time + seconds);
   return NULL;
}
