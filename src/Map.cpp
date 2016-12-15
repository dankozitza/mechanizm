// 
// Map.cpp
//
// Created by Daniel Kozitza
//

#include "Map.hpp"
#include <math.h>

Map::Map() {
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

	ms.reset_item_ptr();
	MapSection* ms_it = ms.get_item_ptr();

	// delete any sections that are not the 8 sections surrounding nx, ny, nz
	for (i = 0; i < ms.size();) {
		if (
				(ms[i].sid[0] != nx && ms[i].sid[0] != nx - emss.size) ||
				(ms[i].sid[1] != ny && ms[i].sid[1] != ny - emss.size) ||
				(ms[i].sid[2] != nz && ms[i].sid[2] != nz - emss.size)) {
			cout << "deleting section (" << ms[i].sid[0] << ", " << ms[i].sid[1];
			cout << ", " << ms[i].sid[2] << ") index: " << i << "\n";

			// delete the item from wherever it is in the queue
			ms.delete_item(i);
		}
		else {
			// only increment when we don't delete something. When we delete an
			// item from Q the following item will take it's place
			i++;
		}
	}

// load 8 sections starting with the bottom north west and ending with
// the top south east, which is placed at nx, ny, nz;

	bool skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();
	// bottom-north-west
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx - emss.size &&
				ms_it->sid[1] == ny - emss.size &&
				ms_it->sid[2] == nz - emss.size) {
			skip = true;
		}

		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section (" << nx - emss.size << ", " << ny - emss.size << ", " << nz - emss.size;
  	cout << ")\n";

		MapSection bnw(nx - emss.size, ny - emss.size, nz - emss.size, 0);
		bnw.generate_blocks();
		ms.enq(bnw);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();

	// bottom-north-east
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx &&
				ms_it->sid[1] == ny - emss.size &&
				ms_it->sid[2] == nz - emss.size) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx << ", " << ny - emss.size << ", " << nz - emss.size;
  	cout << ")\n";

		MapSection bne(nx, ny - emss.size, nz - emss.size, 1);
		bne.generate_blocks();
		ms.enq(bne);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();

	// bottom-south-west
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx - emss.size &&
				ms_it->sid[1] == ny - emss.size &&
				ms_it->sid[2] == nz) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx - emss.size << ", " << ny - emss.size << ", " << nz;
  	cout << ")\n";

		MapSection bsw(nx - emss.size, ny - emss.size, nz, 2);
		bsw.generate_blocks();
		ms.enq(bsw);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();

	// bottom-south-east
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx &&
				ms_it->sid[1] == ny - emss.size &&
				ms_it->sid[2] == nz) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx << ", " << ny - emss.size << ", " << nz;
  	cout << ")\n";

		MapSection bse(nx, ny - emss.size, nz, 3);
		bse.generate_blocks();
		ms.enq(bse);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();


	// top-north-west
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx - emss.size &&
				ms_it->sid[1] == ny &&
				ms_it->sid[2] == nz - emss.size) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx - emss.size << ", " << ny << ", " << nz - emss.size;
  	cout << ")\n";

		MapSection tnw(nx - emss.size, ny, nz - emss.size, 4);
		tnw.generate_blocks();
		ms.enq(tnw);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();


	// bottom-north-east
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx &&
				ms_it->sid[1] == ny &&
				ms_it->sid[2] == nz - emss.size) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();

	}
	if (!skip) {
	cout << "loading map section section (" << nx << ", " << ny << ", " << nz - emss.size;
  	cout << ")\n";

		MapSection tne(nx, ny, nz - emss.size, 5);
		tne.generate_blocks();
		ms.enq(tne);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();


	// top-south-west
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx - emss.size &&
				ms_it->sid[1] == ny &&
				ms_it->sid[2] == nz) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx - emss.size << ", " << ny << ", " << nz;
  	cout << ")\n";

		MapSection tsw(nx - emss.size, ny, nz, 6);
		tsw.generate_blocks();
		ms.enq(tsw);
	}
	skip = false;
	ms.reset_item_ptr();
	ms_it = ms.get_item_ptr();

	// top-south-east
	for (i = 0; i < ms.size(); ++i) {
		if (
				ms_it->sid[0] == nx &&
				ms_it->sid[1] == ny &&
				ms_it->sid[2] == nz) {
			skip = true;
		}
		ms.increment_item_ptr();
		ms_it = ms.get_item_ptr();
	}
	if (!skip) {
	cout << "loading map section section (" << nx << ", " << ny << ", " << nz;
  	cout << ")\n";

		MapSection tse(nx, ny, nz, 7);
		tse.generate_blocks();
		ms.enq(tse);
	}
	ms.reset_item_ptr();
}
