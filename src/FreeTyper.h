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

#include "BitmapImage.h"

class FreeTyper
{
	FT_Library _library;

	FT_Face _face; /* handle to face object */

	FreeTyper();
public:
	virtual ~FreeTyper();


	//used for debugging
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
				std::cout << output << ' ';
			}
			std::cout << std::endl;
		}
	}

	/**
	 * Factory that generates a FreeTyper
	 */
	static std::shared_ptr<FreeTyper> init(int characterHeight, std::string fontPath);

	/**
	 * Attempts to render the given character
	 *
	 * returns a null shared_ptr on error
	 */
	std::shared_ptr<BitmapImage> renderCharacter(char32_t character);
};

#endif /* FREETYPER_H_ */
