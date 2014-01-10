#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H

#include <GL/glew.h>
#include<glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>			//remove later
class Pipeline
{
public:
	Pipeline();
	Pipeline::Pipeline(int uni, glm::mat4 cam, glm::vec3 moveTo);
	~Pipeline(void);
	void LoadIdentity();				//reset transformations
	void LoadMatrix(glm::mat4 m);
	void MoveTo(glm::vec3 v);			//places an entire scene at some x, y, z location
	void Rotate(float angle, glm::vec3 v);
	void LoadView(glm::mat4);			//sets the camera matrix; 
	void Save();
	void Retrieve();
	void Pass();						//pass matrix stack to shader
	glm::mat4 Matrix();
private:
	glm::mat4 model;
	glm::mat4 camera;
	glm::mat4 matrix;
	glm::mat4 temp;
	int uniform;			//uniform's location in the shader
	
};


#endif