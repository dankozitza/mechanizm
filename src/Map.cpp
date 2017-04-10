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
         cout << "Map: loading flatland\n";
         return MapGen_flatland(ms);
         break;

      case 6:
      case 1:
         cout << "Map: loading hills\n";
         return MapGen_hills(ms);
         break;

      case 4:
      case 5:
      case 2:
         cout << "Map: loading single_hill\n";
         return MapGen_single_hill(ms);
         break;

      case 3:
         cout << "Map: loading bg_random\n"; 
         return MapGen_bg_random(ms);
         break;
   }
}

bool Map::section_loaded(int x, int y, int z) {
   for (int i = 0; i < ms.size(); i++) {
      if (ms[i].sid[0] == x &&
          ms[i].sid[1] == y &&
          ms[i].sid[2] == z) {
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

//   int l = cy - 2 * ms_size;
//   int sr = cz - 2 * ms_size;
//   int sc = cx - 2 * ms_size;
//   for (l; l < cy + 2 * ms_size; l += ms_size) {
//      for (int r = sr; r < cz + 2 * ms_size; r += ms_size) {
//         for (int c = sc; c < cx + 2 * ms_size; c += ms_size) {
//            if (sx == c && sy == l && sz == r) {
//               return true;
//            }
//         }
//      }
//   }
//   return false;
}

void Map::update(GLfloat x, GLfloat y, GLfloat z) {
   MapSection emss; // empty section used only for the size (number of blocks
   int nx = roundf(x / (GLfloat) emss.size) * emss.size; // in x, y, and z directions)
   int ny = roundf(y / (GLfloat) emss.size) * emss.size;
   int nz = roundf(z / (GLfloat) emss.size) * emss.size;

   cout << "position: (" << x << ", " << y << ", " << z << "):\n";
   cout << "center of map (" << nx << ", " << ny << ", " << nz;
   cout << ")\n";

   int i;

   // delete any sections that are not the 8 sections surrounding nx, ny, nz
   for (i = 0; i < ms.size();) {
      if (section_near_center(
            nx, ny, nz,
            ms[i].sid[0], ms[i].sid[1], ms[i].sid[2],
            emss.size)) {
         // only increment when we don't delete something. When we delete an
         // item from Q the following item will take it's place
         i++;
      }
      else {
         cout << "deleting section (" << ms[i].sid[0] << ", " << ms[i].sid[1];
         cout << ", " << ms[i].sid[2] << ")\n";

         // delete the item from wherever it is in the queue
         ms.delete_item(i);
      }
   }

//   ms.reset_item_ptr();
//   MapSection* ms_it = ms.get_item_ptr();
//
//   // delete any sections that are not the 8 sections surrounding nx, ny, nz
//   for (i = 0; i < ms.size();) {
//      if (
//            (ms[i].sid[0] != nx && ms[i].sid[0] != nx - emss.size) ||
//            (ms[i].sid[1] != ny && ms[i].sid[1] != ny - emss.size) ||
//            (ms[i].sid[2] != nz && ms[i].sid[2] != nz - emss.size)) {
//         cout << "deleting section (" << ms[i].sid[0] << ", " << ms[i].sid[1];
//         cout << ", " << ms[i].sid[2] << ")\n";
//
//         // delete the item from wherever it is in the queue
//         ms.delete_item(i);
//      }
//      else {
//         // only increment when we don't delete something. When we delete an
//         // item from Q the following item will take it's place
//         i++;
//      }
//   }


//            if (section_near_center(nx, ny, nz, 20, 20, 20, emss.size)) {
//               cout << "FINAL TRUE\n";
//            }
//            else {
//               cout << "FINAL FALSE\n";
//            }


// load 64 sections starting from bottom (y) north (-z) west (x) to form
// a 4x4x4 cube.
   // bottom layer
   int l = ny - 2 * emss.size;
   // north most row
   int sr = nz - 2 * emss.size;
   // west most column
   int sc = nx - 2 * emss.size;

   for (l; l < ny + 2 * emss.size; l += emss.size) {
      for (int r = sr; r < nz + 2 * emss.size; r += emss.size) {
         for (int c = sc; c < nx + 2 * emss.size; c += emss.size) {

            if (!section_loaded(c, l, r)) {

               cout << "test building map section (" << c << ", " << l << ", ";
               cout << r << ") |: ";

               MapSection nms(c, l, r, MapGen_random);
               nms.generate_blocks();
               ms.enq(nms);                
            }
         }
      }
   }
}

//// load 8 sections starting with the bottom north west and ending with
//// the top south east, which is placed at nx, ny, nz;
//
//   bool skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//   // bottom-north-west
//   for (i = 0; i < ms.size(); ++i) {
//      if (section_loaded(nx - emss.size, ny - emss.size, nz - emss.size)) {
//         skip = true;
//         break;
//      }
//
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx - emss.size << ", " << ny - emss.size << ", " << nz - emss.size;
//     cout << ")\n";
//
//      MapSection bnw(nx - emss.size, ny - emss.size, nz - emss.size, MapGen_random);
//      bnw.generate_blocks();
//      ms.enq(bnw);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//   // bottom-north-east
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx &&
//            ms_it->sid[1] == ny - emss.size &&
//            ms_it->sid[2] == nz - emss.size) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx << ", " << ny - emss.size << ", " << nz - emss.size;
//     cout << ")\n";
//
//      MapSection bne(nx, ny - emss.size, nz - emss.size, MapGen_random);
//      bne.generate_blocks();
//      ms.enq(bne);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//   // bottom-south-west
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx - emss.size &&
//            ms_it->sid[1] == ny - emss.size &&
//            ms_it->sid[2] == nz) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx - emss.size << ", " << ny - emss.size << ", " << nz;
//     cout << ")\n";
//
//      MapSection bsw(nx - emss.size, ny - emss.size, nz, MapGen_random);
//      bsw.generate_blocks();
//      ms.enq(bsw);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//   // bottom-south-east
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx &&
//            ms_it->sid[1] == ny - emss.size &&
//            ms_it->sid[2] == nz) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx << ", " << ny - emss.size << ", " << nz;
//     cout << ")\n";
//
//      MapSection bse(nx, ny - emss.size, nz, MapGen_random);
//      bse.generate_blocks();
//      ms.enq(bse);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//
//   // top-north-west
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx - emss.size &&
//            ms_it->sid[1] == ny &&
//            ms_it->sid[2] == nz - emss.size) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx - emss.size << ", " << ny << ", " << nz - emss.size;
//     cout << ")\n";
//
//      MapSection tnw(nx - emss.size, ny, nz - emss.size, MapGen_random);
//      tnw.generate_blocks();
//      ms.enq(tnw);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//
//   // bottom-north-east
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx &&
//            ms_it->sid[1] == ny &&
//            ms_it->sid[2] == nz - emss.size) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx << ", " << ny << ", " << nz - emss.size;
//     cout << ")\n";
//
//      MapSection tne(nx, ny, nz - emss.size, MapGen_random);
//      tne.generate_blocks();
//      ms.enq(tne);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//
//   // top-south-west
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx - emss.size &&
//            ms_it->sid[1] == ny &&
//            ms_it->sid[2] == nz) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//   cout << "loading map section (" << nx - emss.size << ", " << ny << ", " << nz;
//     cout << ")\n";
//
//      MapSection tsw(nx - emss.size, ny, nz, MapGen_random);
//      tsw.generate_blocks();
//      ms.enq(tsw);
//   }
//   skip = false;
//   ms.reset_item_ptr();
//   ms_it = ms.get_item_ptr();
//
//   // top-south-east
//   for (i = 0; i < ms.size(); ++i) {
//      if (
//            ms_it->sid[0] == nx &&
//            ms_it->sid[1] == ny &&
//            ms_it->sid[2] == nz) {
//         skip = true;
//      }
//      ms.increment_item_ptr();
//      ms_it = ms.get_item_ptr();
//   }
//   if (!skip) {
//      cout << "loading map section (" << nx << ", " << ny << ", " << nz;
//      cout << ")\n";
//
//      MapSection tse(nx, ny, nz, MapGen_random);
//      tse.generate_blocks();
//      ms.enq(tse);
//   }
//   ms.reset_item_ptr();
//}
