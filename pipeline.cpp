#include "pipeline.h"


Pipeline::Pipeline()
{
	model = glm::mat4(1.f);
}

Pipeline::Pipeline(int uni, glm::mat4 cam, glm::vec3 moveTo)
{
	uniform = uni;
	camera = cam;
	LoadIdentity();
	MoveTo(moveTo);
	
}


void Pipeline::LoadIdentity()
{
	model = glm::mat4(1.f);
}


void Pipeline::MoveTo(glm::vec3 v)
{
	model =  glm::translate(glm::mat4(1), v) * model;
}

void Pipeline::Rotate(float angle, glm::vec3 v)
{
	model = glm::rotate(glm::mat4(1), angle, v) * model;
}

/*
Matrix transformations are evaluated from right to left; 
hence we save current transformations
and add new to the left of the expression
*/
void Pipeline::LoadMatrix(glm::mat4 m)
{
	glm::mat4 t = model;				//save current matrix stack
	model = m;							//put new mat on top
	model*=t;						//multiply saved matrix with new matrix; 

}

void Pipeline::Save()
{
	Pass();
	temp = model; 
	model = glm::mat4(1);
}

void Pipeline::Retrieve()
{
	model*=temp;
	temp = glm::mat4(1);
}

void Pipeline::LoadView(glm::mat4 m)
{
	camera = m; 
}

void Pipeline::Pass()
{
	glUniformMatrix4fv(uniform, 1, false, glm::value_ptr(Matrix()));	
}

glm::mat4 Pipeline::Matrix()
{
	matrix = glm::mat4(1);
	matrix = camera * model;
	return matrix;
}
Pipeline::~Pipeline(void)
{
}


