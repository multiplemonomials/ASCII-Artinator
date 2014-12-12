/*
 * CharImage.h
 *
 *  Created on: Oct 10, 2014
 *      Author: Jamie
 */

#ifndef CHARIMAGE_H_
#define CHARIMAGE_H_

#include <vector>
#include <cassert>
#include <iostream>

/**
 * class to represent a set of characters in a bitmap-like format
 */
class CharImage
{
	std::vector<std::vector<char>> _blocks;
public:

	/**
	 * Construct from height and width in chars
	 */
	CharImage(unsigned int height, unsigned int width)
	: _blocks(height, std::vector<char>(width))
	{

	}

	void add(int blockx, int blocky, char character);


	//print char image to stream
	friend std::ostream & operator<<(std::ostream & stream, const CharImage & charImage);

	~CharImage(){}
};

std::ostream & operator<<(std::ostream & stream, const CharImage & charImage);


#endif /* CHARIMAGE_H_ */
