#pragma once
#ifndef IMAGE_H
#define IMAGE_H
#include <iostream>
#include <IL\il.h>


class Image
{
private:
	ILuint mImgID; 
	ILubyte* mData; 
	unsigned int mwidth; 
	unsigned int mheight; 
public:

	Image(std::string filename);
	int width();
	int height();
	void init();
	ILboolean load(std::string filename); 
	ILubyte* data();
	~Image(void);
};


#endif