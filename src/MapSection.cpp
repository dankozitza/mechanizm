// 
// MapSection.cpp
//
// Created by Daniel Kozitza
//

#include "MapSection.hpp"
#include <fstream>
#include <unistd.h>

tools::Error dummy_func(MapSection& ms) {
   return NULL;
}

MapSection::MapSection() {
   sid[0] = 0;
   sid[1] = 0;
   sid[2] = 0;
   func_gen = dummy_func;
}

MapSection::MapSection(int x, int y, int z, tools::Error (*func)(MapSection&)) {
   sid[0] = x;
   sid[1] = y;
   sid[2] = z;
   func_gen = func;
}

string MapSection::str_sid() const {
   string ssid;
   char buff[100];
   sprintf(buff, "%i_%i_%i", sid[0], sid[1], sid[2]);
   ssid += buff;
   return ssid;
}

string MapSection::get_file_name(string map_name) const {
   string fname = "maps/";
   fname += map_name;
   fname += "/";

   // get the directory name within map_name
   int idir = 0;
   for (idir = 0; idir >= 0; idir++) {
      if (fid < idir * 100 + 100) {
         break;
      }
   }
   char buff[100];
   sprintf(buff, "%i", idir);
   fname += buff;
   sprintf(buff, "/ms_%i_%i_%i_%i.data", fid, sid[0], sid[1], sid[2]);
   fname += buff;
   return fname;
}

// TODO: section id map to keep track of sections on the file system

void MapSection::save(string map_name) const {
   string fname(get_file_name(map_name));

   cout << "MapSection::save: Saving map section " << fname << ".\n";

   ofstream out(fname);

   for (int l = 0; l < size; ++l) { // y
      for (int r = 0; r < size; ++r) { // z
         for (int c = 0; c < size; ++c) { // x

            if (blocks[l][r][c].type == 0) {
               out << blocks[l][r][c].type << "\n";
               continue;
            }

            out << blocks[l][r][c].type << " ";

            for (int v = 0; v < blocks[l][r][c].vertices; ++v) {
               out << blocks[l][r][c].cube[v][0] << " "
                   << blocks[l][r][c].cube[v][1] << " "
                   << blocks[l][r][c].cube[v][2] << " ";
            }

            for (int fc = 0; fc < 6; ++fc) {
               out << blocks[l][r][c].faceColors[fc][0] << " "
                   << blocks[l][r][c].faceColors[fc][1] << " "
                   << blocks[l][r][c].faceColors[fc][2] << " ";
            }
            out << endl;
         }
      }
   }

   out.close();
}

tools::Error MapSection::load(string map_name) {

   string fname(get_file_name(map_name));

   FILE* fh = fopen(fname.c_str(), "r");
   if (!fh) {
      return tools::errorf(
            "MapSection::load: failed to open map section file `%s`.\n",
            fname.c_str());
   }

   for (int l = 0; l < size; ++l) {
      for (int r = 0; r < size; ++r) {
         for (int c = 0; c < size; ++c) {

            if (feof(fh)) {
               return tools::errorf(
                 "MapSection::load: map section file `%s` is missing blocks.\n",
                     fname.c_str());
            }

            //string line;
            //getline(fh, line);


            Block block (l, r, c);

            int nom = fscanf(fh, "%i", &block.type);

            //if (line.size() == 0) {
            if (block.type == 0) { // empty block
//               cout << "MapSection::load: empty block\n";
               block.type = 0;
               blocks[l][r][c] = block;
               continue;
            }
//            cout << "MapSection::load: non empty block\n";
//            cout << "MapSection::load: got block type: `" << block.type;
            
            for (int z = 0; z < block.vertices; ++z) {
               fscanf(fh, "%f", &block.cube[z][0]);
               fscanf(fh, "%f", &block.cube[z][1]);
               fscanf(fh, "%f", &block.cube[z][2]);
            }

            for (int z = 0; z < 6; ++z) {
               fscanf(fh, "%f", &block.faceColors[z][0]);
               fscanf(fh, "%f", &block.faceColors[z][1]);
               fscanf(fh, "%f", &block.faceColors[z][2]);
//               cout << "` fc[" << z << "][0]: `" << block.faceColors[z][0] << "`\n";
//               cout << "` fc[" << z << "][1]: `" << block.faceColors[z][1] << "`\n";
//               cout << "` fc[" << z << "][2]: `" << block.faceColors[z][2] << "`\n";
            }

            blocks[l][r][c] = block;
         } // c
      } // r
   } // l

   fclose(fh);
   return NULL;
}

void MapSection::generate_blocks() {

   tools::Error e = func_gen(*this);
   if (e != NULL)
      cerr << "MapSection::generate_blocks: Error: " << e << "\n";
}

void MapSection::mark_invalid_block_sides() {
   has_sides = false;
   for (int l = 0; l < size; ++l) {
      for (int r = 0; r < size; ++r) {
         for (int c = 0; c < size; ++c) {
            blocks[l][r][c].mark_invalid_sides();
         }
      }
   }
}

void MapSection::populate_visible_sides(vector<Side> &vsides, Camera &cam) {
   has_sides = true;
   for (int l = 0; l < size; ++l) {
      for (int r = 0; r < size; ++r) {
         for (int c = 0; c < size; ++c) {
            // skip empty blocks
            //cout << "populating visible sides for block: (";
            //cout << l << " " << r << " " << c << ")\n";
            if (blocks[l][r][c].type == 0)
               continue;

            int up    = 2;
            int down  = 0;
            int north = 3;
            int south = 1;
            int east  = 4;
            int west  = 5;

            // set pointer to side to last element in visual sides vector
            Side* tmpsp = vsides.data();
            cout << "MapSection::pop...: tmpsp->valid `" << tmpsp->valid << "`" << endl;
            for (int i = 0; i < vsides.size()-1; ++i) {
               tmpsp++;
            }

            // up    -> l+1 r   c
            if (l+1 >= 0 && l+1 < size) {
               // check adjacent block's type
               if (blocks[l+1][r][c].type == 0) {
                  Side s(l, r, c, up);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[up][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[up];
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_up = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_up = tmpsp;
               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, up);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[up][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[up];
               vsides.push_back(s);
               //blocks[l][r][c].sp_up = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_up = tmpsp;
            }

            // down  -> l-1 r   c
            if (l-1 >= 0 && l-1 < size) {
               // check adjacent block's type
               if (blocks[l-1][r][c].type == 0) {
                  Side s(l, r, c, down);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[down][p] ];
                  }
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_down = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_down = tmpsp;

               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, down);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[down][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[down];
               vsides.push_back(s);
               //blocks[l][r][c].sp_down = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_down = tmpsp;
            }
            
            // north -> l   r-1   c
            if (r-1 >= 0 && r-1 < size) {
               // check adjacent block's type
               if (blocks[l][r-1][c].type == 0) {
                  Side s(l, r, c, north);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[north][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[north];
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_north = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_north = tmpsp;
               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, north);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[north][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[north];
               vsides.push_back(s);
               //blocks[l][r][c].sp_north = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_north = tmpsp;
            }

            // south -> l   r+1   c
            if (r+1 >= 0 && r+1 < size) {
               // check adjacent block's type
               if (blocks[l][r+1][c].type == 0) {
                  Side s(l, r, c, south);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[south][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[south];
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_south = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_south = tmpsp;
               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, south);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[south][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[south];
               vsides.push_back(s);
               //blocks[l][r][c].sp_south = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_south = tmpsp;
            }

            // east  -> l   r   c+1
            if (c+1 >= 0 && c+1 < size) {
               // check adjacent block's type
               if (blocks[l][r][c+1].type == 0) {
                  Side s(l, r, c, east);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[east][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[east];
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_east = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_east = tmpsp;
               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, east);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[east][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[east];
               vsides.push_back(s);
               //blocks[l][r][c].sp_east = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_east = tmpsp;
            }

            // west  -> l   r  c-1
            if (c-1 >= 0 && c-1 < size) {
               // check adjacent block's type
               if (blocks[l][r][c-1].type == 0) {
                  Side s(l, r, c, west);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[west][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[west];
                  vsides.push_back(s);
                  //blocks[l][r][c].sp_west = &vsides[vsides.size()-1];
                  if (vsides.size() != 0)
                     tmpsp++;
                  blocks[l][r][c].sp_west = tmpsp;
               }
            }
            else {
               // edge of section always gets drawn
               Side s(l, r, c, west);
               for (int p = 0; p < 4; ++p) {
                  s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[west][p] ];
               }
               s.color = (GLfloat *) blocks[l][r][c].faceColors[west];
               vsides.push_back(s);
               //blocks[l][r][c].sp_west = &vsides[vsides.size()-1];
               if (vsides.size() != 0)
                  tmpsp++;
               blocks[l][r][c].sp_west = tmpsp;
            }
         } // c
      } // r
   } // l
}
