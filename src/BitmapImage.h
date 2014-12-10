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
public:
	typedef uint16_t greyscaleType;

private:

	//the outer vector is columns, or y
	//the inner vector is rows, or x
	std::vector<std::vector<greyscaleType>> _pixels;
public:

	//construct empty bitmap
	BitmapImage();

	//construct from a FreeType bitmap
	BitmapImage(FT_Bitmap & bitmap);

	//construct from a PNG image, a starting point, and a width and height to go left and down from
	BitmapImage(png::image<png::gray_pixel> & image, unsigned int startx, unsigned int starty, unsigned int width, unsigned int height);

	unsigned int getHeight();

	unsigned int getWidth();

	bool isEmpty();

	//(0,0) is at the top left
	greyscaleType getPixel(unsigned int x, unsigned int y);

	virtual ~BitmapImage();

	//returns the average greyscale value of the image when it is assumed to be fullHeight by fullWidth
	//this is used for, say, getting the average greyscale value of a FreeType-generated character,
	//which is only as wide and as tall as it needs to be, over the full-size space
	greyscaleType greyscaleValue(unsigned int fullHeight, unsigned int fullWidth);

	//returns the average greyscale value of the image
	greyscaleType greyscaleValue()
	{
		return greyscaleValue(getHeight(), getWidth());
	}
};

#endif /* BITMAPIMAGE_H_ */
