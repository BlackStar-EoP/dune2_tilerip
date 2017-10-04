// Dune2TileRip.cpp : Defines the entry point for the console application.
//

// beautiful piece of crap code that anyone may use for whatever purpose
//
//

#include <stdio.h>
#include <vector>
#define _CRT_SECURE_NO_WARNINGS
#include "../SDL2/SDL_image.h"
#undef main

#pragma comment(lib, "../SDL2/sdl2.lib")
#pragma comment(lib, "../SDL2/sdl2_image.lib")

struct tile
{
	std::vector<unsigned char> pixeldata;
	int width = 16;
	int height = 16;
};

struct col
{
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 255;
};

struct tilepalette
{
	col tilecolors[16];
};

void add_pixel_to_vector(const col& c, std::vector<unsigned char>& pixelvector)
{
	pixelvector.push_back(c.r);
	pixelvector.push_back(c.g);
	pixelvector.push_back(c.b);
	pixelvector.push_back(c.a);
}

// Unfinished, the idea was to save the tiles as a more rectangular png
// void save_tiles(std::vector<tile>& tiles, const std::string& filename)
// {
// 	int NUM_X_TILES = 16;
// 	int NUM_Y_TILES = tiles.size() / NUM_X_TILES;
// 	if (tiles.size % NUM_X_TILES != 0)
// 		NUM_Y_TILES++;

// 	std::vector<unsigned char> pixeldata;

// 	for (int y = 0; y < NUM_Y_TILES; ++y)
// 	{
// 		for (int x = 0; x < NUM_X_TILES; ++x)
// 		{
// 			int tileindex = (y * NUM_X_TILES) + x;
// 			const tile& tile = y * 
// 		}
// 	}

// 	int imagewidth = NUM_X_TILES * 16;
// }

void save_pixeldata(std::vector<unsigned char>& pixeldata, const std::string& filename)
{
	// fill remainder
	int remainder = pixeldata.size() % 16;
	for (int i = 0; i < remainder; ++i)
	{
		for (int j = 0; j < 4; ++j)
			pixeldata.push_back(0xFF);
	}
	int width = 16;
	int height = pixeldata.size() / 16 / 4;
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(&pixeldata[0], width, height, 32, 16 * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	IMG_SavePNG(surface, filename.c_str());
}

int main(int argc, char* argv[])
{
	FILE* pal = fopen("ibm.pal", "rb");
	char palbuf[768];
	fread(palbuf, 1, 768, pal);
	fclose(pal);


	std::vector<col> palette;
	for (int i = 0; i < 768; i += 3)
	{
		col c;
		if (i == 0)
		{
			c.r = 0xFF;
			c.b = 0xFF;
		}
		else
		{
			c.r = palbuf[i] * 4;
			c.g = palbuf[i + 1] * 4;
			c.b = palbuf[i + 2] * 4;
			if (i == 0)
				c.a = 0;
		}
		palette.push_back(c);
	}


	std::vector<unsigned char> palettedata;
	for (int i = 0; i < 256; ++i)
	{
		col& c = palette[i];
		palettedata.push_back(c.r);
		palettedata.push_back(c.g);
		palettedata.push_back(c.b);
		palettedata.push_back(c.a);
	}
	SDL_Surface* palsurface = SDL_CreateRGBSurfaceFrom(&palettedata[0], 256, 1, 32, 256 * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	//IMG_SavePNG(palsurface, "pal.png");



	std::vector<unsigned char> pixeldata;
	FILE* tiles = fopen("icon.icn", "rb");
	const int TILE_FSIZE = 54231;
	unsigned char tilebuf[TILE_FSIZE];
	fread(tilebuf, 1, TILE_FSIZE, tiles);
	fclose(tiles);

	// read tile palettes
	const int RPALSTART = 51120;
	std::vector<unsigned char> tilepalettes;
	std::vector<tilepalette> tile_palettes;
	int colorindex = 0;
	tilepalette current_tile_palette;
	for (int i = RPALSTART; i < TILE_FSIZE; ++i)
	{
		unsigned int index = tilebuf[i];
		col& c = palette[index];
		tilepalettes.push_back(c.r);
		tilepalettes.push_back(c.g);
		tilepalettes.push_back(c.b);
		tilepalettes.push_back(c.a);

		current_tile_palette.tilecolors[colorindex++] = c;
		if (colorindex == 16)
		{
			tile_palettes.push_back(current_tile_palette);
			colorindex = 0;
		}
		
	}
//	save_pixeldata(tilepalettes, "tilepalettes.png");

	// Attempt to read and dump all the tiles :)
	//std::vector<unsigned char> tiledata;
	std::vector<tile> tiles;
	const int TILE_COUNT = 399;
	int byte_index = 40; // tiles start at 40
	const int RTBL_start = 53832;
	for (int tilenum = 0; tilenum < TILE_COUNT; ++tilenum)
	{
		int palnum = tilebuf[RTBL_start + tilenum];
		const tilepalette& pal = tile_palettes[palnum];
		tile t;
		// each tile is 128 bytes
		for (int tilebyte = 0; tilebyte < 128; ++tilebyte)
		{
			unsigned char packedpixel = tilebuf[byte_index++];
			unsigned char leftpixel = packedpixel >> 4;
			unsigned char rightpixel = packedpixel & 0x0F;

			add_pixel_to_vector(pal.tilecolors[leftpixel], t.pixeldata);
			add_pixel_to_vector(pal.tilecolors[rightpixel], t.pixeldata);
		}
	}
	//save_tiles(tiles, "tileset.png");
	//save_pixeldata(tiledata, "tiledata.png");
	printf("");

	//for (int i = 0; i < TILE_FSIZE; ++i)
	//{
	//	unsigned int index = tilebuf[i];
	//	col& c = palette[index];
	//	pixeldata.push_back(c.r);
	//	pixeldata.push_back(c.g);
	//	pixeldata.push_back(c.b);
	//	pixeldata.push_back(c.a);
	//}

	//// fill remainder
	//int remainder = pixeldata.size() % 16;
	//for (int i = 0; i < remainder; ++i)
	//{
	//	for (int j = 0; j < 4; ++j)
	//		pixeldata.push_back(0xFF);
	//}
	//int width = 16;
	//int height = pixeldata.size() / 16 / 4;
	//SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(&pixeldata[0], width, height, 32, 16 * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	////IMG_SavePNG(surface, "bras.png");
	return 0;

}


