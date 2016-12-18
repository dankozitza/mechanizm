// 
// MapSection.cpp
//
// Created by Daniel Kozitza
//

#include "MapSection.hpp"

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

void MapSection::generate_blocks() {

   tools::Error e = func_gen(*this);
   if (e != NULL)
      cout << "MapSection::generate_blocks: Error: " << e << "\n";

	//// generate a cube composed of size blocks
	//for (int l = 0; l < size; ++l) {
	//	for (int r = 0; r < size; ++r) {
	//		for (int c = 0; c < size; ++c) {
	//			Block block(l, r, c);
	//			for (int i = 0; i < 6; i++) {
	//				// set the default color

   //            // blue-green world
	//				block.faceColors[i][0] = 0.3;
   //            block.faceColors[i][1] = rand() % 8 / 8.0;
	//				block.faceColors[i][2] = rand() % 8 / 8.0;

	//				//block.faceColors[i][0] = rand() % 100 / (GLfloat) (sid[0] + r);
   //            //block.faceColors[i][1] = rand() % 100 / (GLfloat) (sid[1] + l);
	//				//block.faceColors[i][2] = rand() % 100 / (GLfloat) (sid[2] + c);
	//			}
	//			block.translate_by(
	//				(GLfloat) sid[0] + (GLfloat) r,
	//				(GLfloat) sid[1] + (GLfloat) l,
	//				(GLfloat) sid[2] + (GLfloat) c);

	//			block.type = 1;

   //         // more empty blocks the higher you get
	//			//if (sid[1] + l > -5 && rand() % 20 - 10 < sid[1] + l)
   //         //if (rand() % 1000 < sid[1] + l + 900)
   //         if (rand() % 70 < r || rand() % 200 < sid[1] + l + 150)
	//				block.type = 0;


	//			blocks[l][r][c] = block;
	//		}
	//	}
	//}
}

void MapSection::populate_visible_sides(vector<Side> &vsides, Camera &cam) {
	for (int l = 0; l < size; ++l) {
		for (int r = 0; r < size; ++r) {
			for (int c = 0; c < size; ++c) {
				// skip empty blocks
            //cout << "populating visible sides for block: (";
            //cout << l << " " << r << " " << c << ")\n";
				if (blocks[l][r][c].type == 0)
					continue;

            // skip blocks that are further than size from the camera
            //GLfloat section_pos[3];
            //section_pos[0] = (GLfloat) sid[0] + (GLfloat) r;
            //section_pos[1] = (GLfloat) sid[1] + (GLfloat) l;
            //section_pos[2] = (GLfloat) sid[2] + (GLfloat) c;
            //GLfloat cam_pos[3];
            //cam_pos[0] = cam.getX();
            //cam_pos[1] = cam.getY();
            //cam_pos[2] = cam.getZ();
            //GLfloat d = distance(section_pos, cam_pos);
         
            //cout << "block distance from cam: " << d << " ";

				int up    = 2;
				int down  = 0;
				int north = 1;
				int south = 3;
				int east  = 4;
				int west  = 5;

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
				}
				
				// north -> l   r+1   c
				if (r+1 >= 0 && r+1 < size) {
					// check adjacent block's type
					if (blocks[l][r+1][c].type == 0) {
						Side s(l, r, c, north);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[north][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[north];
						vsides.push_back(s);
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
				}

				// south -> l   r-1   c
				if (r-1 >= 0 && r-1 < size) {
					// check adjacent block's type
					if (blocks[l][r-1][c].type == 0) {
						Side s(l, r, c, south);
                  for (int p = 0; p < 4; ++p) {
                     s.points[p] = (GLfloat *) blocks[l][r][c].cube[ blocks[l][r][c].faceIndex[south][p] ];
                  }
                  s.color = (GLfloat *) blocks[l][r][c].faceColors[south];
						vsides.push_back(s);
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
				}
			} // c
		} // r
	} // l
}
