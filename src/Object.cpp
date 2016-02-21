// 
// Object.cpp
//
// Created by Daniel Kozitza
//

#include "Object.hpp"

Object::Object() {
   id = "";
   initial_pos = {.x = 0, .y = 0, .z = 0};
   current_pos = initial_pos;
   mass = 0;
   func_Motion = NULL;
}

Object::Object(
      string identity,
      Position initial_position,
      double mass_d,
      Function function_Motion) {
   id = identity;
   initial_pos = initial_position;
   mass = mass_d;
   func_Motion = function_Motion;
}
