/*
 * BitmapImage.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: Jamie
 */

#include "BitmapImage.h"

//create an empty bitmap
BitmapImage::BitmapImage()
:_pixels(1, std::vector<greyscaleType>(0))
{
}

//construct from a FreeType bitmap
BitmapImage::BitmapImage(FT_Bitmap & bitmap)
:_pixels(bitmap.rows, std::vector<greyscaleType>(bitmap.width))
{
	// Iterate through the pixels in the bitmap and add them to the _pixels array
	for(int bitmapY = 0; bitmapY < bitmap.rows; ++bitmapY)
	{
		for(int bitmapX = 0; bitmapX < bitmap.width; ++bitmapX)
		{
			uint8_t bitmapPixelValueRaw = *(bitmap.buffer + (bitmapY * bitmap.pitch) + bitmapX);

			//convert from 8-bit greyscale to 16-bit greyscale
			greyscaleType bitmapPixelValue = 257 * bitmapPixelValueRaw;

			_pixels.at(bitmapY).at(bitmapX) = bitmapPixelValue;

		}
	}
}

//construct from a PNG image, a starting point, and a width and height to go left and down from
BitmapImage::BitmapImage(png::image<png::gray_pixel> & image, unsigned int startx, unsigned int starty, unsigned int width, unsigned int height)
:_pixels(height, std::vector<greyscaleType>(width))
{
	// Iterate through the pixels in the bitmap and add them to the _pixels array
	for(unsigned int imageY = 0; imageY < height; ++imageY)
	{
		for(unsigned int imageX = 0; imageX < width; ++imageX)
		{
			uint8_t imagePixelRawValue = image.get_pixel(startx + imageX, starty + imageY);

			//convert from 8-bit greyscale to 16-bit greyscale
			greyscaleType imagePixelValue = 257 * imagePixelRawValue;

			//for some reason, png++ loads greyscale images inverted
			//fix that here
			imagePixelValue = 65535-imagePixelValue;

			_pixels.at(imageY).at(imageX) = imagePixelValue;

		}
	}
}

unsigned int BitmapImage::getHeight()
{
	return _pixels.size();
}

unsigned int BitmapImage::getWidth()
{
	if(_pixels.empty())
	{
		return 0;
	}

	else
	{
		return _pixels.at(0).size();
	}
}

bool BitmapImage::isEmpty()
{
	return _pixels.empty() || _pixels.at(0).empty();
}

BitmapImage::greyscaleType BitmapImage::getPixel(unsigned int x, unsigned int y)
{
	return _pixels.at(y).at(x);
}


BitmapImage::~BitmapImage()
{
}

BitmapImage::greyscaleType BitmapImage::greyscaleValue(unsigned int fullHeight, unsigned int fullWidth)
{
	unsigned long totalGrayscale = 0;
	unsigned int height = getHeight();
	unsigned int width = getWidth();
	for(unsigned int y = 0; y < height; ++y)
	{
		for(unsigned int x = 0; x < width; ++x)
		{
			totalGrayscale += getPixel(x, y);
		}
	}

	//divide by the assumed height and width, not the actual ones
	totalGrayscale /= fullWidth * fullHeight;

	return totalGrayscale;
}

