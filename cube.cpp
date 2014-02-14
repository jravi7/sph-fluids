#include "cube.h"


Cube::Cube(glm::vec3 d, glm::vec3 p)
{
	dim = d; 
	pos = p; 
	tex = new Texture(GL_TEXTURE_2D, "textures/blue.png");
	computeVerts();
	computeIndices();
	computeTexCoords();
	initColors();
	initBuffers();
}

void Cube::initBuffers()
{
	glGenBuffers(1, &VB);
	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//buffers for color array
	glGenBuffers(1, &CB);
	glBindBuffer(GL_ARRAY_BUFFER, CB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Buffers for Texture Coordinates
	glGenBuffers(1, &TB);
	glBindBuffer(GL_ARRAY_BUFFER, TB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//EBO for the indices
	glGenBuffers(1, &IB);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cube::initColors()
{
	colors[0]=1.0;
	colors[1]=0.0;
	colors[2]=0.0;
	colors[3]=1.0;

	colors[4]=0.0;
	colors[5]=1.0;
	colors[6]=0.0;
	colors[7]=1.0;

	colors[8]=0.0;
	colors[9]=0.0;
	colors[10]=1.0;
	colors[11]=1.0;

	colors[12]=1.0;
	colors[13]=1.0;
	colors[14]=1.0;
	colors[15]=1.0;

	colors[16]=1.0;
	colors[17]=0.0;
	colors[18]=0.0;
	colors[19]=1.0;

	colors[20]=0.0;
	colors[21]=1.0;
	colors[22]=0.0;
	colors[23]=1.0;

	colors[24]=0.0;
	colors[25]=0.0;
	colors[26]=1.0;
	colors[27]=1.0;

	colors[28]=0.0;
	colors[29]=0.0;
	colors[30]=0.0;
	colors[31]=1.0;
}

void Cube::computeTexCoords()
{	//front
	texcoords[0] = 0.0f; 
	texcoords[1] = 0.0f;
	texcoords[2] = 1.0f; 
	texcoords[3] = 0.0f; 
	texcoords[4] = 1.0f; 
	texcoords[5] = 1.0f;
	texcoords[6] = 0.0f; 
	texcoords[7] = 1.0f; 

    //top
    texcoords[8] = 0.f;
    texcoords[9] = 1.f;
    texcoords[10] = 0.f;
    texcoords[11] = 0.f;
    texcoords[12] = 1.f;
    texcoords[13] = 0.f;
    texcoords[14] = 1.f;
    texcoords[15] = 1.f;
    //back
    texcoords[16] = 1.f;
    texcoords[17] = 0.f;
    texcoords[18] = 1.f;
    texcoords[19] = 1.f;
    texcoords[20] = 0.f;
    texcoords[21] = 1.f;
    texcoords[22] = 0.f;
    texcoords[23] = 0.f;
    //left
    texcoords[24] = 0.f;
    texcoords[25] = 0.f;
    texcoords[26] = 1.f;
    texcoords[27] = 0.f;
    texcoords[28] = 1.f;
    texcoords[29] = 1.f;
    texcoords[30] = 0.f;
    texcoords[31] = 1.f;
    //right
    texcoords[32] = 1.f;
    texcoords[33] = 0.f;
    texcoords[34] = 1.f;
    texcoords[35] = 1.f;
    texcoords[36] = 0.f;
    texcoords[37] = 1.f;
    texcoords[38] = 0.f;
    texcoords[39] = 0.f;
    //bottom
    texcoords[40] = 1.f;
    texcoords[41] = 1.f;
    texcoords[42] = 0.f;
    texcoords[43] = 1.f;
    texcoords[44] = 0.f;
    texcoords[45] = 0.f;
    texcoords[46] = 1.f;
    texcoords[47] = 0.f;
	

	
}

void Cube::computeVerts()
{
	verts[0] = -dim.x/2 + pos.x;
	verts[1] = -dim.y/2 + pos.y;
	verts[2] = dim.z/2 + pos.z;

	verts[3] = dim.x/2 + pos.x;
	verts[4] = -dim.y/2 + pos.y;
	verts[5] = dim.z/2 + pos.z;

	verts[6] = dim.x/2 + pos.x;
	verts[7] = dim.y/2 + pos.y;
	verts[8] = dim.z/2 + pos.z;

	verts[9] = -dim.x/2 + pos.x;
	verts[10] = dim.y/2 + pos.y;
	verts[11] = dim.z/2 + pos.z;

	verts[12] = -dim.x/2 + pos.x;
	verts[13] = -dim.y/2 + pos.y;
	verts[14] = -dim.z/2 + pos.z;

	verts[15] = dim.x/2 + pos.x;
	verts[16] = -dim.y/2 + pos.y;
	verts[17] = -dim.z/2 + pos.z;

	verts[18] = dim.x/2 + pos.x;
	verts[19] = dim.y/2 + pos.y;
	verts[20] = -dim.z/2 + pos.z;

	verts[21] = -dim.x/2 + pos.x;
	verts[22] = dim.y/2 + pos.y;
	verts[23] = -dim.z/2 + pos.z;
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

void Cube::render(int pos_loc, int color_loc, int tex_loc, int sample)
{
	tex->activate(sample, GL_TEXTURE0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VB);
	glVertexAttribPointer(pos_loc, 3, GL_FLOAT,GL_FALSE, 0, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, CB);
	//glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, TB); 
	glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);			//render triangles
}


Cube::~Cube(void)
{

}
