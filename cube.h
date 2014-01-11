#pragma once
#ifndef CUBE_H
#define CUBE_H

#include <glm\glm.hpp>
#include <GL\glew.h>

class Cube
{
public:
	Cube(glm::vec3 d, glm::vec3 p);
	void render(int pos_loc, int color_loc);
	~Cube(void);
private:
	void computeVerts();
	void computeIndices();
	void initColors();
	void initBuffers(); 

	glm::vec3 pos; 
	glm::vec3 dim;			//dimensions of cube x->width , y-> depth
	GLfloat verts[24];
	GLushort indices[36];
	GLfloat colors[32];

	GLuint VB;			//Vertex buffers
	GLuint IB;			//Element Buffers or Indices buffer
	GLuint CB;			//Color Buffers

};


#endif 