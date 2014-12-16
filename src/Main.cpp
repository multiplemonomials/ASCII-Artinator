#include <iostream>
#include <memory>
#include <thread>
#include <cmath>
#include <map>

#include <Windows.h>
#include <assert.h>

#include "FreeTyper.h"
#include "CharImage.h"

#include <argtable2.h>

#include <png++/png.hpp>

//renders characters and calculates their greyscale values
//if scaling is enabled, it also scales their greyscale values
//also returns the greyscale value of the darkest char.
std::pair<std::map<BitmapImage::greyscaleType, char>, int> getCharToGreyscaleMap(std::shared_ptr<FreeTyper> typer)
{
	//greyscale value of the blackest character
	//used for optimization
	uint8_t maxGreyscaleValue = 0;

	//fill a hash map with ascii characters and their greyscale values
	//note: this loop relies on -funsigned-char
	std::map<BitmapImage::greyscaleType, char> greyscaleToCharacterMap;
	for(char character = 32; character < 255; ++character)
	{
		//render character, or null if not printable
		auto bitmap = typer->renderCharacter(character);

		//if printable char
		if(!bitmap->isEmpty())
		{
			BitmapImage::greyscaleType greyscaleValue = bitmap->greyscaleValue( Options::instance().blockSize,  Options::instance().blockSize);

			if(Options::instance().infoOut)
			{
				std::cout << "Adding char " << character << " with a greyscale value of " << std::dec << (int16_t)greyscaleValue << std::endl;
			}

			if(greyscaleValue > maxGreyscaleValue)
			{
				maxGreyscaleValue = greyscaleValue;
			}


			greyscaleToCharacterMap[greyscaleValue] = character;
		}

	}

	if(Options::instance().infoOut)
	{
		std::cout << "Darkest character is " << greyscaleToCharacterMap.at(maxGreyscaleValue) << " with a greyscale value of " << std::dec << (int16_t)maxGreyscaleValue << std::endl;
	}


	if(Options::instance().scaleCharValues)
	{
		int scalar = 255 / maxGreyscaleValue;
		//now that we have our scalar value, scale the other characters
		for(std::map<BitmapImage::greyscaleType, char>::iterator mapping = greyscaleToCharacterMap.begin(); mapping != greyscaleToCharacterMap.end(); ++mapping)
		{
			(*mapping).first *= scalar;
		}
	}
}

//function which actually does the image conversion
std::shared_ptr<CharImage> mainProcess()
{

}

//function which parses the command line, makes sure the arguments are valid, and invokes mainProcess().
int main(int argc, char** argv)
{
	//setup argtable
	struct arg_lit *info = arg_lit0("v", "verbose", "enable verbose (basically progress) output");
	struct arg_lit *debug = arg_lit0("d", "debug", "enable debug output for development");
	struct arg_lit *scaling = arg_lit0("s", "scaling", "enable scaling the darkest character to near-black");
	struct arg_lit *force = arg_lit0("f", "force", "overwrite output file if it exists");
	struct arg_lit *help = arg_lit0("h", "help", "list usage options (this)");
	struct arg_int *blockSize = arg_int0("s","blockSize", "<n>", "set number of pixels (in either axis) of the original image per character");
	struct arg_file *input = arg_file1(nullptr, nullptr,"<file>", "filename to read png image from");
	struct arg_file *output = arg_file1(nullptr,nullptr,"<file>", "filename to write ASCII image to");
	struct arg_end *end = arg_end(20);

	void * argtable[] = {info, debug, scaling, force, help, blockSize, input, output, end};

	if (arg_nullcheck(argtable) != 0)
	{
		std::cerr << "error: insufficient memory" << std::endl;
	}

	//init default(s)
	blockSize->ival[0] = 16;

	int nerrors = arg_parse(argc,argv,argtable);

	if(help->count > 0)
	{
		arg_print_syntaxv(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "%-25s %s\n");

		return 1;
	}

	if(nerrors != 0)
	{
		arg_print_errors(stdout, end, "aart");

		std::cout << std::endl << "------------------------------------------------------" << std::endl << std::endl;

		arg_print_syntaxv(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "%-25s %s\n");

		return 1;

	}

	//copy options to Options
	Options::instance().infoOut = info->count > 0;
	Options::instance().debugOut = debug->count > 0;
	Options::instance().scaleCharValues = scaling->count > 0;
	Options::instance().blockSize = blockSize->ival[0];


	//load and convert image
	png::image<png::gray_pixel> sourceImage;
	try
	{
		sourceImage = png::image<png::gray_pixel>(input->filename[0]);
	}
	catch(png::std_error & error)
	{
		std::cerr << "error loading input image: " << error.what() << std::endl;
		return 1;
	}


	//verify image - make sure dimensions are multiples of  Options::instance().blockSize
	if(sourceImage.get_width() % Options::instance().blockSize)
	{
		std::cerr << "error: the width of the input image is not divisible by " << std::dec <<  Options::instance().blockSize << std::endl;
		return 1;
	}

	if(sourceImage.get_height() %  Options::instance().blockSize)
	{
		std::cerr << "error: the height of the input image is not divisible by " << std::dec <<  Options::instance().blockSize << std::endl;
		return 1;
	}

	//ensure space for output file
	if(access(output->filename[0], F_OK))
	{
		if(force->count > 0)
		{
			DeleteFile(output->filename[0]);
		}
		else
		{
			std::cerr << "Error: output file exists. You can use the \"-f\" option to overwrite the file." << std::endl;
			return 1;
		}
	}


	//obtain rasterizer from factory
	auto typer = FreeTyper::init( Options::instance().blockSize);

	std::cerr << "Building greyscale to character mapping..." << std::endl;

	std::cerr << "Translating image..." << std::endl;

	//init variable to hold the object
	CharImage outputImage(sourceImage.get_height() /  Options::instance().blockSize, sourceImage.get_width() /  Options::instance().blockSize);

	// for each row in the source image...
	for(unsigned int blockStartY = 0; blockStartY < sourceImage.get_height(); blockStartY +=  Options::instance().blockSize)
	{
		//and each column in each row in the source image...
		for(unsigned int blockStartX = 0; blockStartX < sourceImage.get_width(); blockStartX +=  Options::instance().blockSize)
		{
#ifdef ENABLE_INFO_OUT
			std::cout << "Processing block (" << std::dec << blockStartX /  Options::instance().blockSize << ", " << blockStartY /  Options::instance().blockSize << ")" << std::endl;
#endif


			// read the block's pixels into an object
			BitmapImage blockPixels(sourceImage, blockStartX, blockStartY,  Options::instance().blockSize,  Options::instance().blockSize);

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
				//use our best character in the CharImage for this block
				outputImage.add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, greyscaleToCharacterMap.at(imageGreyscaleValue));
			}
			//if we are over the max value, we can only go down and don't need to search
			else if(imageGreyscaleValue > maxGreyscaleValue)
			{
				outputImage.add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, greyscaleToCharacterMap.at(maxGreyscaleValue));
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

					assert(imageGreyscaleValue + offsetFromIdeal < 255);
				}

#ifdef ENABLE_DEBUG_OUT
				std::cout << "Found a rough match in char " << std::hex << greyscaleToCharacterMap.at(imageGreyscaleValue + offsetFromIdeal) << std::endl;
#endif

				outputImage.add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, greyscaleToCharacterMap.at(imageGreyscaleValue + offsetFromIdeal));

			}

		}
	}

	std::ofstream outputStream;

	outputStream.open("output.txt");

	outputStream << outputImage;

	outputStream.close();

	std::cerr << "Done!" << std::endl << "Make sure to use code page 1252 when viewing the output!" << std::endl;

	arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));


	return 0;
}
