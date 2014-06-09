#include "grid.h"


Grid::Grid(glm::vec3 p, int w, int h, int cs)
{
	m_pos = p;
	m_width = w; 
	m_height = h; 
	m_cellsize = cs;
	init();
}


void Grid::init()
{
	//init grid
	initGrid();

	//init Buffers
	initBuffers();
}

void Grid::initGrid()
{
	int xstart = (m_pos.x - (m_width / 2 )) / m_cellsize; 
	int ystart = (m_pos.y + (m_height / 2)) / m_cellsize; 
	int xend = ((m_pos.x + (m_width/2))  / m_cellsize) + 1;
	int yend = ((m_pos.y-(m_height/2)) / m_cellsize) - 1;
	int nopx = xend - xstart;
	int nopy = ystart - yend;
	for(int i = ystart; i > yend; i--)
	{
		for(int j = xstart; j < xend; j++)
		{
			glm::vec3 v  = glm::vec3(j*m_cellsize, i*m_cellsize, 0);
			glm::vec3 c = glm::vec3 (1, 1, 1);
			m_colors.push_back(c);
			m_vertices.push_back(v);

		}
	}

	//create indices for the grid
	for( int j = 0; j < nopy-1 ; j++)
	{
		for( int i = 0; i < nopx-1; i++)
		{
			int v1 = i + j*nopx;
			int v2 = (i + 1) + j*nopx;
			int v6 = i+ (j+1)*nopx;
			int v7 = (i+1)+((j+1)*nopx);

			
			m_indices.push_back(v1);
 			m_indices.push_back(v6);
			m_indices.push_back(v2);

			m_indices.push_back(v2);
			m_indices.push_back(v6);
			m_indices.push_back(v7);

		}
	}

	

	//initialize m_normals
/*	for(int i = 0 ; i < m_vertices.size();i++)
	{
		m_normals.push_back(glm::vec3(0,0,0));
	}

	for(int i = 0; i < m_indices.size();i+=3)
	{
		unsigned int idx1 = m_indices[i];
		unsigned int idx2 = m_indices[i+1];
		unsigned int idx3 = m_indices[i+2];

		glm::vec3 A = m_vertices[idx1];
		glm::vec3 B = m_vertices[idx2];
		glm::vec3 C = m_vertices[idx3];

		glm::vec3 AB = B-A; 
		glm::vec3 AC = C-A; 
		glm::vec3 cross = glm::cross(AB, AC);

		m_normals[idx1] += cross;
		m_normals[idx2] += cross;
		m_normals[idx3] += cross;

	}

	for(int i = 0 ; i < m_vertices.size();i++)
	{
		m_normals[i] = glm::normalize(m_normals[i]);
	}*/
}

void Grid::initBuffers()
{
	//Generate buffers for grid vertices
	glGenBuffers(1, &m_vb);
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(glm::vec3), &m_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate Buffers for grid Colors; 
	glGenBuffers(1, &m_cb);
	glBindBuffer(GL_ARRAY_BUFFER, m_cb);
	glBufferData(GL_ARRAY_BUFFER, m_colors.size()*sizeof(glm::vec3), &m_colors[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//generate buffers for grid indices
	glGenBuffers(1, &m_ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned short), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//generate buffers for grid normals 
	/*glGenBuffers(1, &m_nb);
	glBindBuffer(GL_ARRAY_BUFFER, m_nb);
	glBufferData(GL_ARRAY_BUFFER, m_normals.size()*sizeof(glm::vec3), &m_normals[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void Grid::render(int pos, int color, int normal)
{
	glPolygonMode( GL_BACK, GL_LINE );
	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_cb);
	glVertexAttribPointer(color, 3, GL_FLOAT, GL_FALSE, 0, 0);

	/*glBindBuffer(GL_ARRAY_BUFFER, m_nb);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_TRUE, 0, 0);*/

	glFrontFace(GL_CW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glDrawElements(GL_POINTS, m_indices.size(), GL_UNSIGNED_SHORT, (void*)0);
}

Grid::~Grid(void)
{
}
