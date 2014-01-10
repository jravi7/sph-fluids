#pragma once
#ifndef CUBE_H
#define CUBE_H

#include <glm\glm.hpp>
#include <GL\glew.h>

class Cube
{
public:
	Cube(unsigned int w, unsigned int h, unsigned int d, glm::vec3 p);
	void computeVerts();
	void computeIndices();
	void initBuffers(); 
	~Cube(void);
private:
	glm::vec3 pos; 
	GLuint width; 
	GLuint height; 
	GLuint depth; 
	GLfloat verts[24];
	GLushort indices[36];

	GLuint VB;			//Vertex buffers
	GLuint IB;			//Element Buffers or Indices buffer

};


#endif 