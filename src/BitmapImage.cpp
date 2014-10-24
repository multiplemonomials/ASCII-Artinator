/*
 * BitmapImage.cpp
 *
 *  Created on: Oct 24, 2014
 *      Author: Jamie
 */

#include "BitmapImage.h"

//construct from a FreeType bitmap
BitmapImage::BitmapImage(FT_Bitmap * bitmap)
:_pixels(bitmap->rows, std::vector<uint8_t>(bitmap->width))
{
	// Iterate through the pixels in the bitmap and add them to the _pixels array
	for(int bitmapY = 0; bitmapY < bitmap->rows; ++bitmapY)
	{
		for(int bitmapX = 0; bitmapX < bitmap->width; ++bitmapX)
		{
			uint8_t bitmapPixelValue = *(bitmap->buffer + (bitmapY * bitmap->pitch) + bitmapX);

			_pixels.at(bitmapY).at(bitmapX) = bitmapPixelValue;

		}
	}
}

//construct from a PNG image, a starting point, and a width and height to go left and down from
BitmapImage::BitmapImage(png::image & image, unsigned int startx, unsigned int starty, unsigned int width, unsigned int height)
:_pixels(height, std::vector<uint8_t>(width))
{
	// Iterate through the pixels in the bitmap and add them to the _pixels array
	for(int imageY = 0; imageY < height; ++imageY)
	{
		for(int imageX = 0; imageX < width; ++imageX)
		{
			uint8_t bitmapPixelValue = image.get_pixel(startx + imageX, starty + imageY);

			_pixels.at(imageY)[imageX] = bitmapPixelValue;

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
		return _pixels.at(0).size;
	}
}

bool BitmapImage::isEmpty()
{
	return _pixels.empty() || _pixels.at(0).empty();
}

uint8_t BitmapImage::getPixel(unsigned int x, unsigned int y)
{
	return _pixels.at(y).at(x);
}


BitmapImage::~BitmapImage()
{
}

uint8_t BitmapImage::greyscaleValue(int fullHeight, int fullWidth)
{
	unsigned int totalGrayscale = 0;
	for(unsigned int y = 0; y <= fullWidth; ++y)
	{
		for(unsigned int x = 0; x <= fullHeight; ++x)
		{
			totalGrayscale += getPixel(x, y);
		}
	}

	totalGrayscale /= fullWidth * fullHeight;

	return totalGrayscale;
}

