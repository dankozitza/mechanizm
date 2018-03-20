//
// Json_utils.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"

tools::Error tools::load_json_value_from_file(Json::Value& jv, string fname) {

   tools::Error e = NULL;
   string filetxt;
   e = read_file(fname, filetxt);
   if (e != NULL) {
      return errorf("tools::load_json_value_from_file: %s", e);
   }

   Json::Reader reader;
   if (!reader.parse(filetxt, jv, false)) {
      return errorf(
"tools::load_json_value_from_file: could not parse file %s. Json::Reader::parse returned false!", fname.c_str());
   }
   return NULL;
}
