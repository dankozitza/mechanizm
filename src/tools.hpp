//
// tools.hpp
//
// utility functions for mc
//
// Created by Daniel Kozitza
//

#ifndef _TOOLS
#define _TOOLS

#include <iostream>
#include <map>
#include <stdio.h>
#include <stdarg.h>
#include <vector>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "Vertex.hpp"

using namespace std;

namespace tools {

   // utils.cpp
   bool get_vfnmkmc_conf(map<string, string>& config);
   bool save_vfnmkmc_conf(map<string, string>& config);
   string get_src_files(string src_dir);

   // system.cpp
   typedef char const * Error;
   Error error(string message = "");
   Error errorf(char const * format, ...);
   void signals_callback_handler(int signum);
   void signals(int sig, void (*callback_func)(int));
   bool require(int sys_exit_val, string msg = "");
   bool require(bool func_return_val, string msg = "");
   bool dir_exists(string dir_name);
   bool pid_exists(int pid);
   bool list_dir(string dir_name, vector<string>& contents);
   bool list_dir_r(string dir_name, vector<string>& contents);
   bool list_dir_r(
         string dir_name,
         vector<string>& contents,
         string prefix);
   Error read_file(string file_path, string& contents);
   Error write_file(string file_path, const string& contents);

   // strings.cpp
   string fold(int indent_width, int max_line_width, string s);
   int as_int(string& str);
   unsigned int as_uint(string& str);
   double as_double(string& str);
   string as_string(GLfloat f);
   string eval_rand(string &s);
   //GLfloat as_glfloat(string& str);
   //bool matches(string s, string str_re);
   //bool matches(string results[], string s, string str_re);
   //bool matches(vector<string>& results, string s, string str_re);
   //bool matches(smatch& sm, string s, string str_re);
   //bool replace_all(
   //      string &s,
   //      string str_re,
   //      string fmt,
   //      regex_constants::match_flag_type mf = regex_constants::match_default);
   //bool find_in_file(string str_re, string fname);
   //void test_matches();
   //void test_replace();

   // pcre_utils.cpp
   bool pmatches(string s, string str_re);
   bool pmatches(string results[], string s, string str_re);
   bool pmatches(vector<string> &results, string s, string str_re);
   bool pmatches(string results[], string s, string str_re, bool get_res);
   bool pmatches(
         vector<string> &results,
         string s,
         string str_re,
         bool get_res);
   void test_pmatches();
   bool replace_first(string& s, string str_re, string rpl);
   size_t replace_all(string& s, string str_re, string rpl);

   // vectors.cpp
   void newlines_to_indices(vector<string>& v);
   istream& operator>>(istream& is, vector<string>& v);
   ostream& operator<<(ostream& os, vector<string>& v);
   void vectors_test();

   // doubles.cpp
   bool equal(double const& d1, double const& d2, double epsilon = 0.0001);

   // json_utils.cpp
   Error load_json_value_from_string(Json::Value& jv, const string& s);
   Error load_json_value_from_file(Json::Value& jv, string fname);

   // ogl_utils.cpp
   Vertex to_vert(vector<GLfloat> cmpnts);
   bool line_intersects_triangle(
         Vertex line[2], Vertex tri[3], Vertex* point);
   bool verts_within_eps(Vertex v1, Vertex v2, GLfloat eps);
   GLfloat brtns(string sbrightness, string sinput);
   GLfloat brtns(string sbrightness, GLfloat input);
   GLfloat brtns(GLfloat brightness, GLfloat input);
}

#endif
