#include "Misc.h"

void enableFixedFunction(Camera &cam)
{
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(glm::value_ptr(cam.projection()));

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(glm::value_ptr(cam.view()));

}

void disableFixedFunction()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}