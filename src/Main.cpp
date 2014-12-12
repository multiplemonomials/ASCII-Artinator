#include <iostream>
#include <memory>
#include <thread>
#include <cmath>
#include <map>

#include <Windows.h>

#include "FreeTyper.h"
#include "CharImage.h"

#include <png++/png.hpp>

//#define ENABLE_DEBUG_OUT
#define ENABLE_INFO_OUT

const unsigned int blockHeight = 8;

typedef long long score_t;

//thanks StackOverflow
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}


int main()
{
	//load and convert image
	png::image<png::gray_pixel> sourceImage;
	try
	{
		sourceImage = png::image<png::gray_pixel>("C:\\eclipse\\workspace\\ASCII Artinator\\Debug\\test.png");
	}
	catch(png::std_error & error)
	{
		std::cerr << "error loading input image: " << error.what() << std::endl;
		return 1;
	}


	//verify image - make sure dimensions are multiples of blockHeight
	if(sourceImage.get_width() % blockHeight)
	{
		std::cerr << "error: the width of the image is not divisible by " << std::dec << blockHeight << std::endl;
		return 1;
	}

	if(sourceImage.get_height() % blockHeight)
	{
		std::cerr << "error: the height of the image is not divisible by " << std::dec << blockHeight << std::endl;
		return 1;
	}

	//obtain rasterizer from factory
	auto typer = FreeTyper::init(blockHeight);

	std::cerr << "Building greyscale to character mapping..." << std::endl;

	//fill a hash map with ascii characters and their greyscale values
	//note: this loop relies on -funsigned-char
	std::map<BitmapImage::greyscaleType, char> greyscaleToCharacterMap;
	for(char character = 32; character < 127; ++character)
	{
		//render character, or null if not printable
		auto bitmap = typer->renderCharacter(character);

		//if printable char
		if(!bitmap->isEmpty())
		{
#ifdef ENABLE_INFO_OUT
			std::cout << "Adding char " << character << " with a greyscale value of " << std::dec << (int16_t)bitmap->greyscaleValue(blockHeight, blockHeight) << std::endl;
#endif
			greyscaleToCharacterMap[bitmap->greyscaleValue(blockHeight, blockHeight)] = character;
		}

	}


	std::cerr << "Translating image..." << std::endl;

	//init variable to hold the object
	CharImage outputImage(sourceImage.get_height() / blockHeight, sourceImage.get_width() / blockHeight);

	// for each row in the source image...
	for(unsigned int blockStartY = 0; blockStartY < sourceImage.get_height(); blockStartY += blockHeight)
	{
		//and each column in each row in the source image...
		for(unsigned int blockStartX = 0; blockStartX < sourceImage.get_width(); blockStartX += blockHeight)
		{
#ifdef ENABLE_INFO_OUT
			std::cout << "Processing block (" << std::dec << blockStartX / blockHeight << ", " << blockStartY / blockHeight << ")" << std::endl;
#endif


			// read the block's pixels into an object
			BitmapImage blockPixels(sourceImage, blockStartX, blockStartY, blockHeight, blockHeight);

			BitmapImage::greyscaleType imageGreyscaleValue = blockPixels.greyscaleValue();

#ifdef ENABLE_INFO_OUT
			std::cout << "block greyscale value:" << std::dec << (int16_t)imageGreyscaleValue << std::endl;
#endif

			//if there is an exact match, our job is easy
			if(greyscaleToCharacterMap.find(imageGreyscaleValue) != greyscaleToCharacterMap.end())
			{
#ifdef ENABLE_DEBUG_OUT
				std::cout << "Found an exact match in char " << '\'' << greyscaleToCharacterMap.at(imageGreyscaleValue) << '\'' << std::endl;
#endif
				//use our best character to the CharImage for this block
				outputImage.add(blockStartX / blockHeight, blockStartY / blockHeight, greyscaleToCharacterMap.at(imageGreyscaleValue));
			}
			else
			{
				//try to find a close match by going around the actual value
				int offsetFromIdeal = 1;
				while(greyscaleToCharacterMap.find(imageGreyscaleValue + offsetFromIdeal) == greyscaleToCharacterMap.end())
				{
#ifdef ENABLE_DEBUG_OUT
				std::cout << "Looking for a match offset from the ideal greyscale value by " << offsetFromIdeal << std::endl;
#endif
					offsetFromIdeal = offsetFromIdeal > 0 ? -offsetFromIdeal : -offsetFromIdeal + 1;
				}

#ifdef ENABLE_DEBUG_OUT
				std::cout << "Found a rough match in char " << std::hex << greyscaleToCharacterMap.at(imageGreyscaleValue + offsetFromIdeal) << std::endl;
#endif

				outputImage.add(blockStartX / blockHeight, blockStartY / blockHeight, greyscaleToCharacterMap.at(imageGreyscaleValue + offsetFromIdeal));

			}

		}
	}


	//delete and re-create the file
	DeleteFile("output.txt");

	std::ofstream outputStream;

	outputStream.open("output.txt");

	outputStream << outputImage;

	outputStream.close();

	std::cerr << "Done!" << std::endl;


	return 0;
}
