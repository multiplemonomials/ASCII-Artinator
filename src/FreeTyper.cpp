/*
 * FreeTyper.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: Jamie
 */

#include "FreeTyper.h"

#include <exception>
#include <iostream>
#include <cmath>

FreeTyper::FreeTyper()
: _library(),
_face()
{


}

FreeTyper::~FreeTyper()
{
}

std::shared_ptr<FreeTyper> FreeTyper::init(int characterHeight, std::string fontPath)
{
	auto typer = std::shared_ptr<FreeTyper>(new FreeTyper());
	if(FT_Init_FreeType(&(typer->_library)))
	{
		throw std::exception{};
	}

	int error = FT_New_Face(typer->_library, "C:\\Windows\\Fonts\\consola.ttf", 0, &(typer->_face));


	if(error)
	{
		if(error == FT_Err_Unknown_File_Format)
		{
			throw std::runtime_error("Could not read the font file: " + fontPath);
		}
		else
		{
			throw std::runtime_error("FreeType Error " + std::to_string(error) + " while loading font");
		}
	}

	if(FT_Set_Pixel_Sizes(typer->_face, 0, characterHeight))
	{
		throw std::runtime_error("Call to FT_Set_Pixel_Sizes failed!");
	}


	return typer;
}

std::shared_ptr<BitmapImage> FreeTyper::renderCharacter(char32_t character)
{
	auto glyphIndex = FT_Get_Char_Index(_face, character);

	if(glyphIndex == 0)
	{
		return std::make_shared<BitmapImage>();
	}

	if(FT_Load_Glyph(_face, glyphIndex, FT_LOAD_DEFAULT))
	{
		throw std::runtime_error("Call to FT_Load_Glyph failed!");
	}

	if(FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL))
	{
		throw std::runtime_error("Call to FT_Render_Glyph");
	}

	return std::make_shared<BitmapImage>(_face->glyph->bitmap);
}

