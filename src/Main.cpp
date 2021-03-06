#include <iostream>
#include <memory>
#include <cmath>
#include <map>
#include <sstream>

#include <Windows.h>
#include <assert.h>

#include "FreeTyper.h"
#include "CharImage.h"
#include "Options.h"

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
	for(char character = 32; character < Options::instance().maxCharValue; ++character)
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
		int scalar = Options::instance().maxCharValue / maxGreyscaleValue;

		std::map<BitmapImage::greyscaleType, char> scaledCharacterMap;

		//now that we have our scalar value, scale the other characters
		for(std::map<BitmapImage::greyscaleType, char>::iterator mapping = greyscaleToCharacterMap.begin(); mapping != greyscaleToCharacterMap.end(); ++mapping)
		{
			scaledCharacterMap[(*mapping).first * scalar] = (*mapping).second ;
		}

		maxGreyscaleValue *= scalar;

		return std::make_pair(scaledCharacterMap, maxGreyscaleValue);
	}

	return std::make_pair(greyscaleToCharacterMap, maxGreyscaleValue);
}

//function which actually does the image conversion
std::shared_ptr<CharImage> mainProcess(png::image<png::gray_pixel> & sourceImage, std::string fontFile)
{
	//obtain rasterizer from factory
	auto typer = FreeTyper::init( Options::instance().blockSize, fontFile);

	std::cerr << "Building greyscale to character mapping..." << std::endl;

	auto mapPair = getCharToGreyscaleMap(typer);

	std::cerr << "Translating image..." << std::endl;

	//init variable to hold the object
	std::shared_ptr<CharImage> outputImage = std::make_shared<CharImage>(sourceImage.get_height() /  Options::instance().blockSize, sourceImage.get_width() /  Options::instance().blockSize);

	// for each row in the source image...
	for(unsigned int blockStartY = 0; blockStartY < sourceImage.get_height(); blockStartY +=  Options::instance().blockSize)
	{
		//and each column in each row in the source image...
		for(unsigned int blockStartX = 0; blockStartX < sourceImage.get_width(); blockStartX +=  Options::instance().blockSize)
		{
			if(Options::instance().infoOut)
			{
				std::cout << "Processing block (" << std::dec << blockStartX /  Options::instance().blockSize << ", " << blockStartY /  Options::instance().blockSize << ")" << std::endl;
			}


			// read the block's pixels into an object
			BitmapImage blockPixels(sourceImage, blockStartX, blockStartY,  Options::instance().blockSize,  Options::instance().blockSize);

			BitmapImage::greyscaleType imageGreyscaleValue = blockPixels.greyscaleValue();

			if(Options::instance().infoOut)
			{
				std::cout << "block greyscale value:" << std::dec << (int16_t)imageGreyscaleValue << std::endl;
			}

			//if there is an exact match, our job is easy
			if(mapPair.first.find(imageGreyscaleValue) != mapPair.first.end())
			{
				if(Options::instance().debugOut)
				{
					std::cout << "Found an exact match in char " << '\'' << mapPair.first.at(imageGreyscaleValue) << '\'' << std::endl;
				}
				//use our best character in the CharImage for this block
				outputImage->add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, mapPair.first.at(imageGreyscaleValue));
			}
			//if we are over the max value, we can only go down and don't need to search
			else if(imageGreyscaleValue > mapPair.second)
			{
				outputImage->add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, mapPair.first.at(mapPair.second));
			}
			else
			{
				//try to find a close match by going around the actual value
				int offsetFromIdeal = 1;
				while(mapPair.first.find(imageGreyscaleValue + offsetFromIdeal) == mapPair.first.end())
				{
					if(Options::instance().debugOut)
					{
						std::cout << "Looking for a match offset from the ideal greyscale value by " << offsetFromIdeal << std::endl;
					}
					offsetFromIdeal = offsetFromIdeal > 0 ? -offsetFromIdeal : -offsetFromIdeal + 1;

					assert(imageGreyscaleValue + offsetFromIdeal < 255);
				}

				if(Options::instance().debugOut)
				{
					std::cout << "Found a rough match in char " << std::hex << mapPair.first.at(imageGreyscaleValue + offsetFromIdeal) << std::endl;
				}

				outputImage->add(blockStartX /  Options::instance().blockSize, blockStartY /  Options::instance().blockSize, mapPair.first.at(imageGreyscaleValue + offsetFromIdeal));

			}

		}
	}

	return outputImage;
}

//function which parses the command line, makes sure the arguments are valid, and invokes mainProcess().
int main(int argc, char** argv)
{
	//setup argtable
	struct arg_lit *info = arg_lit0("v", "verbose", "enable verbose (basically progress) output");
	struct arg_lit *debug = arg_lit0("d", "debug", "enable debug output for development");
	struct arg_lit *scaling = arg_lit0("n", "noScaling", "disable scaling the darkest character to near-black");
	struct arg_lit *invert = arg_lit0("i", "invert", "invert lights and darks in output");
	struct arg_lit *version = arg_lit0("e", "version", "print version and exit");
	struct arg_lit *asciiOnly = arg_lit0("a", "asciiOnly", "use ASCII only, not extended ASCII. This allows you to use a utf-8 editor to view the output.");
	struct arg_lit *force = arg_lit0("f", "force", "overwrite output file if it exists");
	struct arg_lit *help = arg_lit0("h", "help", "list usage options (this)");
	struct arg_int *blockSize = arg_int0("b","blockSize", "<n>", "set number of pixels (in either axis) of the original image per character.  Default 16.");
	struct arg_file *font = arg_file0("c","font","[font]", "Full path to font file, or just its name if if is in the system default folder. (e.g. consola.ttf).");
	struct arg_file *input = arg_file1(nullptr, nullptr,"<input>", "filename to read png image from");
	struct arg_file *output = arg_file1(nullptr,nullptr,"<output>", "filename to write ASCII image to");
	struct arg_end *end = arg_end(20);

	void * argtable[] = {info, debug, scaling, invert, version, asciiOnly, force, help, blockSize, font, input, output, end};

	if (arg_nullcheck(argtable) != 0)
	{
		std::cerr << "error: insufficient memory" << std::endl;
		return 1;
	}

	//init default(s)
	blockSize->ival[0] = 16;
	font->filename[0] = "C:\\Windows\\Fonts\\consola.ttf";

	int nerrors = arg_parse(argc,argv,argtable);

	if(help->count > 0)
	{
		arg_print_syntaxv(stdout, argtable, "\n");
		arg_print_glossary(stdout, argtable, "%-25s %s\n");

		return 0;
	}

	if(version->count > 0)
	{
		std::cout << "aart version " << Options::instance().version << std::endl;
		return 0;
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
	Options::instance().scaleCharValues = scaling->count == 0;
	Options::instance().invert = invert->count > 0;
	if(asciiOnly->count > 0)
	{
		Options::instance().maxCharValue = 127;
	}
	else
	{
		Options::instance().maxCharValue = 255;

	}
	Options::instance().blockSize = blockSize->ival[0];

	//deal with font argument

	std::stringstream triedLocations;
	std::string fontFile(font->filename[0]);
	triedLocations << fontFile << std::endl;

	boolean cannotReadFont = false;

	if(access(font->filename[0], R_OK) != 0)
	{
		if(strcmp(font->basename[0], font->filename[0]) == 0) //if there was no full path provided...
		{
			//try appending the default fonts folder path
			std::stringstream stream;
			stream << "C:\\Windows\\Fonts\\" << font->filename[0];

			fontFile = stream.str();
			triedLocations << fontFile << std::endl;

			if(access(fontFile.c_str(), R_OK) != 0)
			{
				cannotReadFont = true;
			}
		}
		else
		{
			cannotReadFont = true;
		}
	}

	if(cannotReadFont)
	{
		std::cerr << "Error: cannot read font file." << std::endl << "Tried locations: " << triedLocations.str();
		return 1;
	}


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
		std::cerr << "Error: the width of the input image is not divisible by " << std::dec <<  Options::instance().blockSize << std::endl;
		return 1;
	}

	if(sourceImage.get_height() %  Options::instance().blockSize)
	{
		std::cerr << "Error: the height of the input image is not divisible by " << std::dec <<  Options::instance().blockSize << std::endl;
		return 1;
	}

	//ensure space for output file
	if(access(output->filename[0], F_OK) == 0)
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

	std::shared_ptr<CharImage> charImage = mainProcess(sourceImage, fontFile);

	std::ofstream outputStream;

	outputStream.open(output->filename[0]);

	outputStream << *charImage;

	outputStream.close();

	std::cerr << "Done!" << std::endl << "Make sure to use code page 1252 when viewing the output!" << std::endl;

	arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));


	return 0;
}
