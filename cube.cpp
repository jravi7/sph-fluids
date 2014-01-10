#include "cube.h"


Cube::Cube(unsigned int w, unsigned int h, unsigned int d, glm::vec3 p)
{
	width = w; 
	height = h; 
	depth = d; 
	pos = p; 
	computeVerts();
	computeIndices();

}

void Cube::computeVerts()
{
	verts[0] = -width/2;
	verts[1] = -height/2;
	verts[2] = depth/2;

	verts[3] = width/2;
	verts[4] = -height/2;
	verts[5] = depth/2;

	verts[6] = width/2;
	verts[7] = height/2;
	verts[8] = depth/2;

	verts[9] = -width/2;
	verts[10] = height/2;
	verts[11] = depth/2;

	verts[12] = -width/2;
	verts[13] = -height/2;
	verts[14] = -depth/2;

	verts[15] = width/2;
	verts[16] = -height/2;
	verts[17] = -depth/2;

	verts[18] = width/2;
	verts[19] = height/2;
	verts[20] = -depth/2;

	verts[21] = -width/2;
	verts[22] = height/2;
	verts[23] = -depth/2;
}

void Cube::computeIndices()
{
	//front
	indices[0]  = 0;
	indices[1]  = 1;
	indices[2]  = 2;
	indices[3]  = 2;
	indices[4]  = 3;
	indices[5]  = 0;
	//top
	indices[6]  = 3;
	indices[7]  = 2;
	indices[8]  = 6;
	indices[9]  = 6;
	indices[10]  = 7;
	indices[11]  = 3;
	//back
	indices[12]  = 7;
	indices[13]  = 6;
	indices[14]  = 5;
	indices[15]  = 5;
	indices[16]  = 4;
	indices[17]  = 7;
	//left
	indices[18]  = 4;
	indices[19]  = 0;
	indices[20]  = 3;
	indices[21]  = 3;
	indices[22]  = 7;
	indices[23]  = 4;
	//right
	indices[24]  = 1;
	indices[25]  = 5;
	indices[26]  = 6;
	indices[27]  = 6;
	indices[28]  = 2;
	indices[29]  = 1;
	//bottom
	indices[30]  = 4;
	indices[31]  = 5;
	indices[32]  = 1;
	indices[33]  = 1;
	indices[34]  = 0;
	indices[35]  = 4;
}


Cube::~Cube(void)
{
}
