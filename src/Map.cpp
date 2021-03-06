// 
// Map.cpp
//
// Created by Daniel Kozitza
//


#include "Map.hpp"
#include <math.h>

// used by all MapGen functions to get the general height
int height = 0;
int h_change = 3;
int x_h_scale = 24;
int z_h_scale = 24;
int h_s_change = 4;

Map::Map() {
}

void ground(GLfloat& g, GLfloat x, GLfloat z) {
   GLfloat y1, y2;
   y1 = height + sin(x/((GLfloat) x_h_scale/3.14)) * h_change;
   y2 = height + sin(z/((GLfloat) z_h_scale/3.14)) * h_change;
   g  = ((y1 + y2) / 2.0);
}

// these functions are passed to MapSection objects to be used for generating
// block type, color, and the general shape of the map
//
tools::Error MapGen_flatland(MapSection& ms) {
   for (int l = 0; l < ms.size; ++l) {
      for (int r = 0; r < ms.size; ++r) {
         for (int c = 0; c < ms.size; ++c) {
            Block block(l, r, c);
            for (int i = 0; i < 6; i++) {
               // set the default color
               // green
               block.faceColors[i][0] = (GLfloat) (rand() % 2 + 1) / 4.0;
               block.faceColors[i][1] = (GLfloat) (rand() % 2 + 1) / 4.0 + 0.4;
               block.faceColors[i][2] = (GLfloat) (rand() % 2 + 1) / 4.0;
            }
            block.translate_by(
               (GLfloat) ms.sid[0] + (GLfloat) r,
               (GLfloat) ms.sid[1] + (GLfloat) l,
               (GLfloat) ms.sid[2] + (GLfloat) c);

            block.type = 1;

            if (ms.sid[1] + l > height)
               block.type = 0;

            ms.blocks[l][r][c] = block;
         }
      }
   }
   return NULL;
}

tools::Error MapGen_hills(MapSection& ms) {

   // change the overall elevation of generated terrain
   if (rand() % 16 == 1) {
      if (rand() % 2 == 1)
         height += (rand() % h_change) + 1;
      else
         height -= (rand() % h_change) + 1;
   }

   for (int l = 0; l < ms.size; ++l) {
      for (int r = 0; r < ms.size; ++r) {
         for (int c = 0; c < ms.size; ++c) {
            Block block(l, r, c);
            for (int i = 0; i < 6; i++) {
               // set the default color
               // green
               block.faceColors[i][0] = (GLfloat) (rand() % 2 + 1) / 4.0;
               block.faceColors[i][1] = (GLfloat) (rand() % 2 + 1) / 4.0 + 0.4;
               block.faceColors[i][2] = (GLfloat) (rand() % 2 + 1) / 4.0;
            }
            block.translate_by(
               (GLfloat) ms.sid[0] + (GLfloat) r,
               (GLfloat) ms.sid[1] + (GLfloat) l,
               (GLfloat) ms.sid[2] + (GLfloat) c);

            block.type = 1;

            if (ms.sid[1] + l > height)
               block.type = 0;

            ms.blocks[l][r][c] = block;
         } // c
      } // r
   } // l
   return NULL;
}

tools::Error MapGen_single_hill(MapSection& ms) {

   // change the x and z scale for the sin waves in ground
   if (rand() % 8 == 1) {
      if (rand() % 2 == 1) {
         if (x_h_scale < 300)
            x_h_scale += (rand() % h_s_change) + 1;
         if (z_h_scale < 300)
            z_h_scale += (rand() % h_s_change) + 1;
      }
      else {
         if (x_h_scale > 12)
            x_h_scale -= (rand() % h_s_change) + 1;
         if (z_h_scale > 12)
            z_h_scale -= (rand() % h_s_change) + 1;
      }
   }

   for (int l = 0; l < ms.size; ++l) {
      for (int r = 0; r < ms.size; ++r) {
         for (int c = 0; c < ms.size; ++c) {
            Block block(l, r, c);
            for (int i = 0; i < 6; i++) {
               // set the default color
               // green
               block.faceColors[i][0] = (GLfloat) (rand() % 2 + 1) / 4.0;
               block.faceColors[i][1] = (GLfloat) (rand() % 2 + 1) / 4.0 + 0.4;
               block.faceColors[i][2] = (GLfloat) (rand() % 2 + 1) / 4.0;
            }
            block.translate_by(
               (GLfloat) ms.sid[0] + (GLfloat) r,
               (GLfloat) ms.sid[1] + (GLfloat) l,
               (GLfloat) ms.sid[2] + (GLfloat) c);

            block.type = 1;

            GLfloat g;
            ground(g, ms.sid[0] + r, ms.sid[2] + c);
            if (ms.sid[1] + l > height + roundf(g))
               block.type = 0;

            ms.blocks[l][r][c] = block;
         } // c
      } // r
   } // l
   return NULL;
}

tools::Error MapGen_bg_random(MapSection& ms) {
   for (int l = 0; l < ms.size; ++l) {
      for (int r = 0; r < ms.size; ++r) {
         for (int c = 0; c < ms.size; ++c) {
            Block block(l, r, c);
            for (int i = 0; i < 6; i++) {
               // set the default color

               // blue-green world
               block.faceColors[i][0] = 0.3;
               block.faceColors[i][1] = rand() % 8 / 8.0;
               block.faceColors[i][2] = rand() % 8 / 8.0;

               //block.faceColors[i][0] = rand() % 100 / (GLfloat) (sid[0] + r);
               //block.faceColors[i][1] = rand() % 100 / (GLfloat) (sid[1] + l);
               //block.faceColors[i][2] = rand() % 100 / (GLfloat) (sid[2] + c);
            }
            block.translate_by(
               (GLfloat) ms.sid[0] + (GLfloat) r,
               (GLfloat) ms.sid[1] + (GLfloat) l,
               (GLfloat) ms.sid[2] + (GLfloat) c);

            block.type = 1;

            // more empty blocks the higher you get
            //if (sid[1] + l > -5 && rand() % 20 - 10 < sid[1] + l)
            //if (rand() % 1000 < sid[1] + l + 900)
            if (rand() % 70 < r || rand() % 200 < ms.sid[1] + l + 150)
               block.type = 0;

            ms.blocks[l][r][c] = block;
         }
      }
   }
   return NULL;
}

tools::Error MapGen_random(MapSection& ms) {
   switch (rand() % 7) {

      case 0:
//         cout << "Map: generating flatland\n";
         return MapGen_flatland(ms);
         break;

      case 6:
      case 1:
//         cout << "Map: generating hills\n";
         return MapGen_hills(ms);
         break;

      case 4:
      case 5:
      case 2:
//         cout << "Map: generating single_hill\n";
         return MapGen_single_hill(ms);
         break;

      case 3:
//         cout << "Map: generating bg_random\n"; 
         return MapGen_bg_random(ms);
         break;
   }
   return NULL;
}

int Map::section_i_loaded(int x, int y, int z) {
   for (int i = 0; i < ms.size(); i++) {
      if (ms[i].sid[0] == x &&
          ms[i].sid[1] == y &&
          ms[i].sid[2] == z) {
         return i;
      }
   }
   return -1;
}

bool Map::section_loaded(int x, int y, int z) {
   if (section_i_loaded(x, y, z) == -1)
      return false;
   else
      return true;
}

bool Map::section_cached(Q<MapSection>& cmsq, int& i,
      int x, int y, int z) {
   for (int o = 0; o < cmsq.size(); o++) {
      if (cmsq[o].sid[0] == x &&
          cmsq[o].sid[1] == y &&
          cmsq[o].sid[2] == z) {
         i = o;
         return true;
      }
   }
   return false;
}

bool Map::section_near_center(
      int cx, int cy, int cz,
      int sx, int sy, int sz,
      int ms_size) {

   if (
         sx < cx - 2 * ms_size ||
         sx >= cx + 2 * ms_size ||
         sy < cy - 2 * ms_size ||
         sy >= cy + 2 * ms_size ||
         sz < cz - 2 * ms_size ||
         sz >= cz + 2 * ms_size ) {
      return false;
   }
   return true;
}

void Map::add_esm_entry(MapSection& ms) {
   esm[ms.str_sid()].fid = ms.fid;
}

void Map::load_section_list() {
   // check for maps
   string dname = "maps";
   if (!dir_exists(dname)) {
      string scall = "mkdir ";
      scall += dname;
      cout << "Map::load_section_list: Executing system call `"
           << scall << "`\n";
      system(scall.c_str());
   }

   // check for map_name
   dname += "/";
   dname += map_name;
   if (!dir_exists(dname)) {
      string scall = "mkdir ";
      scall += dname;
      cout << "Map::load_section_list: Executing system call `"
           << scall << "`\n";
      system(scall.c_str());
   }

   vector<string> idirs;
   tools::list_dir(dname, idirs);

   if (idirs.size() <= 2) {
      string scall = "mkdir ";
      scall += dname;
      scall += "/0";
      cout << "Map::load_section_list: Executing system call `"
           << scall << "`\n";
      system(scall.c_str());
      return;
   }

   int idir;
   for (int i = 0; i < idirs.size(); ++i) {
      if (pmatches(idirs[i], "^\\d+$")) {
         //cout << "`" << idirs[i] << "` ";
         //sscanf(idirs[i].c_str(), "%i", &idir);
         //cout << "idir: `" << idir << "`\n";

   string idirname = dname;
   idirname += "/";
   idirname += idirs[i];
   vector<string> msfiles;
   tools::list_dir(idirname, msfiles);

   for (int j = 0; j < msfiles.size(); ++j) {

      // get the fid and sid from the file name
      string m[4];
      if (pmatches(m, msfiles[j],
               "^ms_(\\d+)_(\\-?\\d+_\\-?\\d+_\\-?\\d+)\\.data$")) {

         EsmV tmp;
         sscanf(m[1].c_str(), "%i", &tmp.fid);
         esm[m[2]] = tmp;

         if (tmp.fid >= sections) {
            sections = tmp.fid + 1;
         }
      }
   }

      } // end of idir match
   }
}

void Map::update(GLfloat x, GLfloat y, GLfloat z, Q<MapSection>& cmsq) {

   MapSection emss; // empty section used only for the size (number of blocks
                    // in x, y, and z directions)
   int nx = roundf(x / (GLfloat) emss.size) * emss.size;
   int ny = roundf(y / (GLfloat) emss.size) * emss.size;
   int nz = roundf(z / (GLfloat) emss.size) * emss.size;

   cout << "Map::update: position: (" << x << ", " << y << ", " << z
        << ") center of map (" << nx << ", " << ny << ", " << nz << ")\n";

   // ensure only one child makes the idir directory
   bool make_dir = false;
   // get the directory name within map_name for current number of sections
   int idir;
   for (idir = 0; idir >= 0; idir++) {
      if (sections < idir * 100 + 100) {
         break;
      }
   }

   int i;
   // remove any sections that are not the 64 sections surrounding nx, ny, nz
   for (i = 0; i < ms.size();) {
      if (section_near_center(
            nx, ny, nz,
            ms[i].sid[0], ms[i].sid[1], ms[i].sid[2],
            emss.size)) {
         // only increment when we don't transfer something. Transfering an item
         // removes it from this queue and the following item takes it's place.
         i++;
      }
      else {

         cout << "Map::update: transfering section ";

         if (ms[i].fid != -1)
            cout << ms[i].fid << " (";
         else
            cout << sections << " (";

         cout << ms[i].sid[0] << ", " << ms[i].sid[1] << ", " << ms[i].sid[2]
              << ") to cache...\n";

         // transfer the item from the map section queue to the cache queue
         ms.transfer_item(i, cmsq);

         // every 100 sections make a new directory
         if (sections == idir * 100 + 100) {
            string dname = "maps/";
            dname += map_name;
            dname += "/";
            char buff[100];
            sprintf(buff, "%i", idir + 1);
            dname += buff;

            cout << "Map::update: Number of map sections: `"
                 << sections << "` idir: `" << idir << "`\n"
                 << "Map::update: checking for dir `"
                 << dname << "`.\n";
            if (!dir_exists(dname)) {
               cout << "Map::update: Dir not found. Running system "
                    << "call: `";
               string syscall = "mkdir ";
               syscall += dname;
               cout << syscall << "`.\n";
               system(syscall.c_str());
            }
            else {
               cout << "MAP::update: dir `" << dname
                    << "` was already made.\n";
            }
         }

         // set the fid if this section doesn't have one
         if (cmsq[cmsq.size()-1].fid == -1) {
            cmsq[cmsq.size()-1].fid = sections++;
         }
      }
   }

// load 64 sections starting from bottom (y) north (-z) west (x) to form
// a 4x4x4 cube.

   // bottom layer
   int l = ny - 2 * emss.size;
   // north most row
   int sr = nz - 2 * emss.size;
   // west most column
   int sc = nx - 2 * emss.size;

   // only load 8 at a time
   int load_cnt = 1;
   for (l; l < ny + 2 * emss.size; l += emss.size) {
      bool br_l = false;
      for (int r = sr; r < nz + 2 * emss.size; r += emss.size) {
         bool br_r = false;
         for (int c = sc; c < nx + 2 * emss.size; c += emss.size) {

            if (!section_loaded(c, l, r)) {

               // check the cache queue
               int i;
               if (section_cached(cmsq, i, c, l, r)) {
                  cout << "Map::update: Transfering cached section "
                       << cmsq[i].fid << " (" << cmsq[i].sid[0] << ", "
                       << cmsq[i].sid[1] << ", " << cmsq[i].sid[2]
                       << ") into main queue.\n";
         
                  cmsq.transfer_item(i, ms);
               }
               else {

                  MapSection nms(c, l, r, MapGen_random);

                  // check if this section exists on the file system
                  auto it = esm.find(nms.str_sid());
                  if (it != esm.end()) {
                     cout << "Map::update: test loading map section ("
                          << c << ", " << l << ", " << r << ") fid: `"
                          << esm[nms.str_sid()].fid << "`\n";

                     nms.fid = esm[nms.str_sid()].fid;

                     tools::Error e = nms.load(map_name);
                     if (e != NULL) {
                        cerr << "Map::update: got error: `" << e << "`.\n";
                     }
                  }
                  else { // generate a new section
                     cout << "Map::update: test building map section ("
                          << c << ", " << l << ", " << r << ")\n";

                     nms.generate_blocks();
                  }

                  ms.enq(nms);
               }

               load_cnt++;
               if (load_cnt > 8) {
                  br_r = true;
                  br_l = true;
                  break;
               }
            }
         } // end of c loop
         if (br_r) {break;}
      } // end of r loop
      if (br_l) {break;}
   } // end of l loop
}

void Map::generate_visible_edges() {


   for (int i = 0; i < ms.size(); ++i) {

      // used to determine the vertices of each side
      int up    = 2;
      int down  = 0;
      int north = 5;
      int south = 4;
      int east  = 1;
      int west  = 3;

      // up
      int nbr_i = -1;
      if (ms[i].has_edge_sides.up == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0],
               ms[i].sid[1] + ms[i].size,
               ms[i].sid[2]);
//         cout << "Map::generate_visible_edges: \n\n\n   i: " << i << " sid: ("
//              << ms[i].sid[0] << ", " << ms[i].sid[1] << ", " << ms[i].sid[2]
//              << ") nbr_i: "
//              << nbr_i << "\n\n\n";
      }
      if (nbr_i != -1) {

         ms[i].has_edge_sides.up = true;

         // iterate over all blocks in the top layer of the map section
         for (int xi = 0; xi < ms[i].size; ++xi) {
            for (int zi = 0; zi < ms[i].size; ++zi) {

               int yi = ms[i].size - 1;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[0][xi][zi].type == 0) {
                  Side s(7, 7, 7, up);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[up][p] ];
                  }
                  s.color = (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[up];
                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }

      // down
      if (ms[i].has_edge_sides.down == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0],
               ms[i].sid[1] - ms[i].size,
               ms[i].sid[2]);
      }
      if (nbr_i != -1) {
         ms[i].has_edge_sides.down = true;

         // iterate over all blocks in the top layer of the map section
         for (int xi = 0; xi < ms[i].size; ++xi) {
            for (int zi = 0; zi < ms[i].size; ++zi) {

               int yi = 0;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[ms[nbr_i].size - 1][xi][zi].type == 0) {
                  Side s(7, 7, 7, down);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[down][p] ];
                  }
                  s.color =
                     (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[down];

                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }

      // north (-z)
      if (ms[i].has_edge_sides.north == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0],
               ms[i].sid[1],
               ms[i].sid[2] - ms[i].size);
      }
      if (nbr_i != -1) {
         ms[i].has_edge_sides.north = true;

         // iterate over all blocks in the top layer of the map section
         for (int xi = 0; xi < ms[i].size; ++xi) {
            for (int yi = 0; yi < ms[i].size; ++yi) {

               int zi = 0;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[yi][xi][ms[nbr_i].size - 1].type == 0) {
                  Side s(7, 7, 7, north);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[north][p] ];
                  }
                  s.color =
                     (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[north];

                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }

      // south
      if (ms[i].has_edge_sides.south == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0],
               ms[i].sid[1],
               ms[i].sid[2] + ms[i].size);
      }
      if (nbr_i != -1) {
         ms[i].has_edge_sides.south = true;

         // iterate over all blocks in the top layer of the map section
         for (int xi = 0; xi < ms[i].size; ++xi) {
            for (int yi = 0; yi < ms[i].size; ++yi) {

               int zi = ms[i].size - 1;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[yi][xi][0].type == 0) {
                  Side s(7, 7, 7, south);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[south][p] ];
                  }
                  s.color =
                     (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[south];

                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }

      // east
      if (ms[i].has_edge_sides.east == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0] + ms[i].size,
               ms[i].sid[1],
               ms[i].sid[2]);
      }
      if (nbr_i != -1) {
         ms[i].has_edge_sides.east = true;

         // iterate over all blocks in the top layer of the map section
         for (int zi = 0; zi < ms[i].size; ++zi) {
            for (int yi = 0; yi < ms[i].size; ++yi) {

               int xi = ms[i].size - 1;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[yi][0][zi].type == 0) {
                  Side s(7, 7, 7, east);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[east][p] ];
                  }
                  s.color =
                     (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[east];

                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }

      // west
      if (ms[i].has_edge_sides.west == false) {
         nbr_i = section_i_loaded(
               ms[i].sid[0] - ms[i].size,
               ms[i].sid[1],
               ms[i].sid[2]);
      }
      if (nbr_i != -1) {
         ms[i].has_edge_sides.west = true;

         // iterate over all blocks in the top layer of the map section
         for (int zi = 0; zi < ms[i].size; ++zi) {
            for (int yi = 0; yi < ms[i].size; ++yi) {

               int xi = 0;

               // blocks are indexed y, x, z
               if (ms[i].blocks[yi][xi][zi].type == 0)
                  continue;

               // check the type of the adjacent block
               if (ms[nbr_i].blocks[yi][ms[nbr_i].size - 1][zi].type == 0) {
                  Side s(7, 7, 7, west);

                  // assign the correct vertices to the pointers in the points
                  // array
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) ms[i].blocks[yi][xi][zi].cube[ ms[i].blocks[yi][xi][zi].faceIndex[west][p] ];
                  }
                  s.color =
                     (GLfloat *) ms[i].blocks[yi][xi][zi].faceColors[west];

                  ms[i].visible_sides.enq(s);
               }
            }
         }

         // reset neighbor index
         nbr_i = -1;
      }
   }
}

//            // down  -> l-1 r   c
//            if (l-1 >= 0 && l-1 < size) {
//               // check adjacent block's type
//               if (blocks[l-1][r][c].type == 0) {
//                  Side s(l, r, c, down);
//                  for (int p = 0; p < 4; ++p) {
//                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[down][p] ];
//                  }
//                  visible_sides.enq(s);
//               }
//            }
//            //else {
//            //   // edge of section always gets drawn
//            //   Side s(l, r, c, down);
//            //   for (int p = 0; p < 4; ++p) {
//            //      s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[down][p] ];
//            //   }
//            //   s.color = (GLfloat *) blocks[l][r][c].faceColors[down];
//            //   visible_sides.enq(s);
//            //}
//            
//            // north -> l   r-1   c
//            if (r-1 >= 0 && r-1 < size) {
//               // check adjacent block's type
//               if (blocks[l][r-1][c].type == 0) {
//                  Side s(l, r, c, north);
//                  for (int p = 0; p < 4; ++p) {
//                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[north][p] ];
//                  }
//                  s.color = (GLfloat *) blocks[l][r][c].faceColors[north];
//                  visible_sides.enq(s);
//               }
//            }
//            //else {
//            //   // edge of section always gets drawn
//            //   Side s(l, r, c, north);
//            //   for (int p = 0; p < 4; ++p) {
//            //      s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[north][p] ];
//            //   }
//            //   s.color = (GLfloat *) blocks[l][r][c].faceColors[north];
//            //   visible_sides.enq(s);
//            //}
//
//            // south -> l   r+1   c
//            if (r+1 >= 0 && r+1 < size) {
//               // check adjacent block's type
//               if (blocks[l][r+1][c].type == 0) {
//                  Side s(l, r, c, south);
//                  for (int p = 0; p < 4; ++p) {
//                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[south][p] ];
//                  }
//                  s.color = (GLfloat *) blocks[l][r][c].faceColors[south];
//                  visible_sides.enq(s);
//               }
//            }
//            //else {
//            //   // edge of section always gets drawn
//            //   Side s(l, r, c, south);
//            //   for (int p = 0; p < 4; ++p) {
//            //      s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[south][p] ];
//            //   }
//            //   s.color = (GLfloat *) blocks[l][r][c].faceColors[south];
//            //   visible_sides.enq(s);
//            //}
//
//            // east  -> l   r   c+1
//            if (c+1 >= 0 && c+1 < size) {
//               // check adjacent block's type
//               if (blocks[l][r][c+1].type == 0) {
//                  Side s(l, r, c, east);
//                  for (int p = 0; p < 4; ++p) {
//                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[east][p] ];
//                  }
//                  s.color = (GLfloat *) blocks[l][r][c].faceColors[east];
//                  visible_sides.enq(s);
//               }
//            }
//            //else {
//            //   // edge of section always gets drawn
//            //   Side s(l, r, c, east);
//            //   for (int p = 0; p < 4; ++p) {
//            //      s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[east][p] ];
//            //   }
//            //   s.color = (GLfloat *) blocks[l][r][c].faceColors[east];
//            //   visible_sides.enq(s);
//            //}
//
//            // west  -> l   r  c-1
//            if (c-1 >= 0 && c-1 < size) {
//               // check adjacent block's type
//               if (blocks[l][r][c-1].type == 0) {
//                  Side s(l, r, c, west);
//                  for (int p = 0; p < 4; ++p) {
//                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[west][p] ];
//                  }
//                  s.color = (GLfloat *) blocks[l][r][c].faceColors[west];
//                  visible_sides.enq(s);
//               }
//            }
//            //else {
//            //   // edge of section always gets drawn
//            //   Side s(l, r, c, west);
//            //   for (int p = 0; p < 4; ++p) {
//            //      s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[west][p] ];
//            //   }
//            //   s.color = (GLfloat *) blocks[l][r][c].faceColors[west];
//            //   visible_sides.enq(s);
//            //}
//         } // c
//      } // r
//   } // l
