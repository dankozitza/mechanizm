//
// vectors.cpp
//
// Created by Daniel Kozitza
//

#include "../tools.hpp"

void tools::newlines_to_indices(vector<string>& v) {
   for (int i = 0; i < v.size(); i++) {
      // v.insert(v.begin()+i, s); // places item BEFORE i. makes a new i-1.

      // any time there's a newline replace v[i] with the previous string,
      // insert the remaining string at v[i+1], continue.
      string pstr;
      string fullstr = v[i];
      bool append = false;
      for (int z = 0; z < fullstr.size(); z++) {
         if (!append) {
            if (fullstr[z] == '\n') {
               // replace v[i]
               v[i] = pstr;
               // insert after i or append to v a new string
               if (v.size() > i+1) {
                  v.insert(v.begin()+i+1, "");
               }
               else {
                  v.push_back("");
               }
               append = true;
            }
            else {
               pstr += fullstr[z];
            }
         }
         else {// add the rest of the string to v[i+1]
            v[i+1] += fullstr[z];
         }
      }
   }
}

istream& tools::operator>>(istream& is, vector<string>& v) {
   char c, last = ' ';
   int cnt = 0;

   v.clear();

   is.get(c);
   while (is.good() && c != '\n') {

      if (c != ' ' && c != '	') {

         if (v.empty() || v.size() == cnt)
            v.push_back("");

         v[cnt] += c;
      }
      else if (last != ' ' && last != '	') {
         // if c is a space but last is not then the word is done
         cnt++;
      }

      last = c;
      is.get(c);
   }

   return is;
}

ostream& tools::operator<<(ostream& os, vector<string>& v) {
   os << "{";
   if (!v.empty()) {
      for (int i = 0; i < v.size() - 1; ++i)
         os << v[i] << ", ";
      
      cout << v[v.size() - 1];
   }
   os << "}";

   return os;
}

void tools::vectors_test() {

   vector<string> v1;

   cout << "enter command: ";
   cin >> v1;
   cout << "vector size: `" << v1.size() << "`\n";
   cout << "got vector: `" << v1 << "`\n\n";

   cout << "enter command: ";
   cin >> v1;
   cout << "vector size: `" << v1.size() << "`\n";
   cout << "got vector: `" << v1 << "`\n\n";
}
