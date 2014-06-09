#include "image.h"


Image::Image(std::string filename)
{
	load(filename);
}



void Image::load(std::string filename)
{
	image = SOIL_load_image(filename.c_str(), &mwidth, &mheight, 0, SOIL_LOAD_RGB);
}

int Image::width()
{
	return mwidth;
}

int Image::height()
{
	return mheight;
}

unsigned char* Image::data()
{
	return image;
}
Image::~Image(void)
{
	SOIL_free_image_data(image);
}
