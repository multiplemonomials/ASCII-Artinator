/*
 * BitmapImage.h
 *
 *  Created on: Oct 24, 2014
 *      Author: Jamie
 */

#ifndef BITMAPIMAGE_H_
#define BITMAPIMAGE_H_

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <png++/png.hpp>


/**
 * Class to represent an image made of pixels
 * For simplicity, internally it uses vectors.
 *
 * All constructors make copies of the data provided to them.
 */

class BitmapImage
{
	//the outer vector is columns, or y
	//the inner vector is rows, or x
	std::vector<std::vector<uint8_t>> _pixels;
public:

	//construct from a FreeType bitmap
	BitmapImage(FT_Bitmap * bitmap);

	//construct from a PNG image, a starting point, and a width and height to go left and down from
	BitmapImage(png::image & image, unsigned int startx, unsigned int starty, unsigned int width, unsigned int height);

	unsigned int getHeight();

	unsigned int getWidth();

	bool isEmpty();

	//(0,0) is at the top left
	uint8_t getPixel(unsigned int x, unsigned int y);

	virtual ~BitmapImage();

	//returns the average greyscale value of the image when it is assumed to be fullHeight by fullWidth
	//this is used for, say, getting the average greyscale value of a FreeType-generated character,
	//which is only as wide and as tall as it needs to be, over the full-size space
	uint8_t greyscaleValue(int fullHeight, int fullWidth);

	//returns the average greyscale value of the image
	uint8_t greyscaleValue()
	{
		return greyscaleValue(getHeight(), getWidth());
	}
};

#endif /* BITMAPIMAGE_H_ */
