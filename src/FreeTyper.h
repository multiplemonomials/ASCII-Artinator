/*
 * FreeTyper.h
 *
 *  Created on: Oct 8, 2014
 *      Author: Jamie
 */

#ifndef FREETYPER_H_
#define FREETYPER_H_

#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftbitmap.h>
#include <memory>
#include <iostream>

class FreeTyper
{
	FT_Library _library;

	FT_Face _face; /* handle to face object */

	std::shared_ptr<FT_Bitmap> _whiteSpaceBitmap;

	FreeTyper();
public:
	virtual ~FreeTyper();

	char getApproximateGreyscaleCharacter(uint8_t greyscaleValue)
	{
		char output = ' ';
		if(greyscaleValue > 180)
		{
			output = '#';
		}
		else if(greyscaleValue > 150)
		{
			output = 'B';
		}
		else if(greyscaleValue > 100)
		{
			output = '?';
		}
		else if(greyscaleValue > 50)
		{
			output = '^';
		}
		else if(greyscaleValue > 25)
		{
			output = '.';
		}
		return output;
	}

	/**
	 * Tries to roughly print an FT_Bitmap to the console
	 *
	 * Used for debugging
	 */

	void printBitmap(FT_Bitmap bitmap)
	{
		std::cout << "Rows: " << bitmap.rows <<" Width: " << bitmap.width << " Pitch: " << bitmap.pitch << std::endl;
		for(int row = 0; row < bitmap.rows; ++row)
		{
			for(int column = 0; column < bitmap.width; ++column)
			{
				char output = getApproximateGreyscaleCharacter(*(bitmap.buffer + (row * bitmap.pitch) + column));
				//uint8_t pixel = *(bitmap.buffer + (row * bitmap.pitch) + column);
				//char output = ((pixel) > 128) ? '#' : '.';
				std::cout << output << ' ';
			}
			std::cout << std::endl;
		}
	}

	/**
	 * Factory that generates a FreeTyper
	 */
	static std::shared_ptr<FreeTyper> init(int characterHeight);

	/**
	 * Attempts to render the given character
	 *
	 * returns a null shared_ptr on error
	 */
	std::shared_ptr<FT_Bitmap> renderCharacter(char32_t character);
};

#endif /* FREETYPER_H_ */
