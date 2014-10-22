#include <iostream>
#include <memory>
#include <thread>
#include <cmath>

#include "FreeTyper.h"
#include "CharImage.h"

#include <png++/png.hpp>

unsigned int blockHeight = 16;

typedef long long score_t;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

//gets the match score between the gray values of two pixels
score_t computeScore(int16_t sourceValue, int16_t bitmapValue)
{
	int difference = std::abs(((int16_t)sourceValue) - ((int16_t)bitmapValue));
	return 128 - difference;
}

int main()
{
	//load and convert image
	png::image<png::gray_pixel> sourceImage;
	try
	{
		sourceImage = png::image<png::gray_pixel>("C:\\eclipse\\workspace\\Unicode Artinator\\Debug\\test.png");
	}
	catch(png::std_error & error)
	{
		std::cerr << "error loading input image: " << error.what() << std::endl;
		return 1;
	}


	//verify image - make sure dimensions are multiples of blockHeight
	if(sourceImage.get_width() % blockHeight)
	{
		std::cerr << "error: the width of the image is not divisible by " << blockHeight << std::endl;
		return 1;
	}

	if(sourceImage.get_height() % blockHeight)
	{
		std::cerr << "error: the height of the image is not divisible by " << blockHeight << std::endl;
		return 1;
	}

	//obtain rasterizer from factory
	auto typer = FreeTyper::init(blockHeight);

	//init variable to hold the object
	CharImage outputImage(sourceImage.get_height() / blockHeight, sourceImage.get_width() / blockHeight);

	// for each row in the source image...
	for(unsigned int blockStartY = 0; blockStartY < sourceImage.get_height(); blockStartY += blockHeight)
	{
		//and each column in each row in the source image...
		for(unsigned int blockStartX = 0; blockStartX < sourceImage.get_width(); blockStartX += blockHeight)
		{
			std::cout << "Processing block (" << blockStartX / blockHeight << ", " << blockStartY / blockHeight << ")" << std::endl;

			//Will contain best character match for this block
			std::pair<score_t, char32_t> currentBest(0, ' ');

			//print the block

			for(unsigned int row = 0; row < blockHeight; ++row)
			{
				for(unsigned int column = 0; column < blockHeight; ++column)
				{
					char output = typer->getApproximateGreyscaleCharacter(255 - sourceImage.get_pixel(blockStartX + column, blockStartY + row));
					std::cout << output << ' ';
				}
				std::cout << std::endl;
			}

			//for each unicode character...
			for(char32_t character = 0; character < 255; ++character)
			{
				// Score for this character.
				score_t score = 0;

				//render character, or null if not printable
				auto bitmap = typer->renderCharacter(character);



				//if printable char
				if(bitmap && (bitmap->pitch > 0))
				{



					// Compare this bitmap against this block in the source image.


					    // Calculate the offsets of the bitmap in the image block.
						assert(blockHeight >= bitmap->rows);
						assert(blockHeight >= bitmap->width);

						const uint16_t bitmapYOffset = (blockHeight - bitmap->rows) / 2;
						const uint16_t bitmapXOffset = 0;


						// Iterate through the pixels in the bitmap, comparing them against the corresponding
					    // image block pixels, and produce a score.
						for(int bitmapX = 0; bitmapX < bitmap->width; ++bitmapX)
						{
							for(int bitmapY = 0; bitmapY < bitmap->rows; ++bitmapY)
							{

								int imagePixelValue = 255 - sourceImage.get_pixel(bitmapX + bitmapXOffset, bitmapY + bitmapYOffset);
								int bitmapPixelValue = *(bitmap->buffer + (bitmapY * bitmap->pitch) + bitmapX);

								//std::cout << "(" << bitmapX << "," << bitmapY << "): " << typer->getApproximateGreyscaleCharacter(bitmapPixelValue) << " ";
								//std::cout << "(" << bitmapX + bitmapXOffset  << "," << bitmapY + bitmapYOffset << "): " << typer->getApproximateGreyscaleCharacter(imagePixelValue) << std::endl;

								score += computeScore(imagePixelValue, bitmapPixelValue);

							}
						}


					//std::cout << "Character \'" << (char)character << "\' matches the block with a score of " << score;

					//if score is better than our current best, set the current best to this
					if(score > currentBest.first)
					{
						//std::cout << ", a new best";
						currentBest.first = score;
						currentBest.second = character;
					}

					//std::cout << std::endl;
				}

			}
			std::cout << "The best match for the block is " << currentBest.second << std::endl;

			//use our best character to the CharImage for this block
			outputImage.add(blockStartX / blockHeight, blockStartY / blockHeight, currentBest.second);

		}
	}

	std::wcout << outputImage;


	return 0;
}
