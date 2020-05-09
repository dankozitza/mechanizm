// 
// mechanizm.cpp
//
// Created by Daniel Kozitza
//

#include "mechanizm.hpp"

mechanizm::mechanizm() {
   objects = NULL;
   current_time = 0;
}

mechanizm::mechanizm(vector<Object> &objs) {
   objects = &objs;
   current_time = 0;
}

//tools::Error mechanizm::spawn(Object &object) {
//   objects.push_back(&object);
//   cout << "got object: " << object.id << ": ";
//   cout << (*objects[objects.size()-1]).func_Motion << endl;
//   return NULL;
//}

void mechanizm::set_objects(vector<Object> &objs) {
   objects = &objs;
}

//tools::Error mechanizm::unspawn(int i) {
//   objects[i] = NULL;
//   return NULL;
//}

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
      for (int i = 0; i < objects->size(); ++i) {
         if (objects->at(i).function == NULL) {

//            cout << "func motion for object: `";
//            cout << objects->at(i).id.c_str();
//            cout << "` is NULL!\n";
            continue;//return NULL;
         }
         e = objects->at(i).function(current_time, objects->operator[](i));
         if (e != NULL) {
            return tools::errorf(
                  "mechanizm::run: at %d seconds object %s returned error %s",
                  current_time, objects->operator[](i).id.c_str(), e);
         }
      }
   }
   if (!tools::equal(current_time, init_time + seconds))
      return tools::errorf(
            "mechanizm::run: current time %d != expected time %d\n",
            current_time, init_time + seconds);
   return NULL;
}
