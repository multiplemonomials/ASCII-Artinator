/*
 * CharImage.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: Jamie
 */

#include "CharImage.h"

#include <uchar.h>
#include <iomanip>

std::ostream & operator<<(std::ostream & stream, const CharImage & charImage)
{
	//each block is a 32-bit character
	for(auto row : charImage._blocks)
	{

		for(auto character : row)
		{
			stream << character;
		}

		stream << std::endl;
	}

	return stream;
}

void CharImage::add(int blockx, int blocky, char character)
{
	_blocks.at(blocky).at(blockx) = character;
}
