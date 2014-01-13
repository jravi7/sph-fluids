#include "image.h"


Image::Image(std::string filename)
{
	ilInit();
	init();
	load(filename);
}

void Image::init()
{
	ilGenImages(1, &mImgID);
	ilBindImage(mImgID);
}

ILboolean Image::load(std::string filename)
{
	ILboolean success;
	try{
		success = ilLoadImage(filename.c_str());
	}
	catch (int err)
	{
		printf("there was a error loading the image");
	}
	//convert to RGBA
	if(success == IL_TRUE)
	{
		//success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		mData = ilGetData();
		mwidth = ilGetInteger(IL_IMAGE_WIDTH);
		mheight = ilGetInteger(IL_IMAGE_HEIGHT);
	}
	else{
		
	}

	return success;
}

int Image::width()
{
	return mwidth;
}

int Image::height()
{
	return mheight;
}

ILubyte* Image::data()
{
	return mData;
}
Image::~Image(void)
{
}
