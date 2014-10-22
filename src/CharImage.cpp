/*
 * CharImage.cpp
 *
 *  Created on: Oct 10, 2014
 *      Author: Jamie
 */

#include "CharImage.h"

#include <uchar.h>
#include <iomanip>

std::wostream & operator<<(std::wostream & stream, const CharImage & charImage)
{
	//each block is a 32-bit character
	for(auto row : charImage._blocks)
	{
		mbstate_t mbstate{};
		for(auto character : row)
		{
			//char multibyteChar[MB_CUR_MAX];
			//c32rtomb(multibyteChar, character, &mbstate);
			stream << (char)character;//multibyteChar;
		}

		stream << std::endl;
	}

	return stream;
}

void CharImage::add(int blockx, int blocky, char32_t character)
{
	_blocks.at(blocky).at(blockx) = character;
}
