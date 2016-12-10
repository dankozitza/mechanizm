// 
// Side.cpp
//
// Created by Daniel Kozitza
//

#include "Side.hpp"

Side::Side(int layer, int row, int column, int side_index, int section_index) {
	id[0] = layer;
	id[1] = row;
	id[2] = column;
	index = side_index;
	s_index = section_index;
}
