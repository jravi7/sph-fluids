#pragma once

#include <GL/glew.h>
#include <vector>
#include <glm\glm.hpp>
class Grid
{
public:
	Grid(glm::vec3 pos, int w, int h, int cs);
	void init();
	void initGrid();
	void initBuffers();
	void render(int pos, int color, int normal);
	~Grid(void);
private:
	std::vector<glm::vec3> m_vertices; 
	std::vector<glm::vec3> m_colors;
	std::vector<glm::vec3> m_normals;
	std::vector<unsigned short> m_indices; 

	GLuint m_vb;				//Vertex Buffers; 
	GLuint m_cb;				//Color Buffers; 
	GLuint m_ib;				//Indices Buffers; 
	GLuint m_nb;				//Normal buffers;

	int m_width;				//total width along x-axis
	int m_height;				//total height along y-axis
	int m_cellsize;				//side of a cell (square)
	glm::vec3 m_pos;			//centre of the grid
};

