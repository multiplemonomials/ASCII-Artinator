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
_face(),
_whiteSpaceBitmap(new FT_Bitmap())
{

	// TODO Auto-generated constructor stub

}

FreeTyper::~FreeTyper() {
	// TODO Auto-generated destructor stub
}

std::shared_ptr<FreeTyper> FreeTyper::init(int characterHeight)
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
			std::cerr << "Could not read the font file." << std::endl;
		}
		else
		{
			std::cerr << "FreeType Error " << error << " while loading font" << std::endl;
		}
		throw std::exception{};
	}

	if(FT_Set_Pixel_Sizes(typer->_face, 0, characterHeight))
	{
		throw std::exception{};
	}

	//set up white space bitmap
	typer->_whiteSpaceBitmap->buffer = new unsigned char[characterHeight*characterHeight];
	memset(typer->_whiteSpaceBitmap->buffer, 0, characterHeight*characterHeight);

	typer->_whiteSpaceBitmap->pitch = characterHeight;
	typer->_whiteSpaceBitmap->rows = characterHeight;
	typer->_whiteSpaceBitmap->width = characterHeight;

	return typer;
}

std::shared_ptr<FT_Bitmap> FreeTyper::renderCharacter(char32_t character)
{
	auto glyphIndex = FT_Get_Char_Index(_face, character);

	if(glyphIndex == 0)
	{
		return std::shared_ptr<FT_Bitmap>();
	}

	if(FT_Load_Glyph(_face, glyphIndex, FT_LOAD_DEFAULT))
	{
		throw std::exception();
	}

	if(FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL))
	{
		throw std::exception();
	}

	auto bitmap = std::make_shared<FT_Bitmap>();

	if(FT_Bitmap_Copy(_library, &(_face->glyph->bitmap), bitmap.get()))
	{
		throw std::exception();
	}

	//a bitmap with no pitch is returned for whitespace (but still valid) characters
	//if(bitmap->pitch == 0)
	//{
	//	return _whiteSpaceBitmap;
	//}

	//printBitmap(_face->glyph->bitmap);
	return bitmap;
}

