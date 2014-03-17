#pragma once

#include<vector>
#include<glm/glm.hpp>
#include <GL/glew.h>

class SpringSystem
{
public:
	SpringSystem(int n);
	void init();
	void initbuffers();
	void render(int);
	void simulate(float dt);
	~SpringSystem(void);

private:
	std::vector<glm::vec3> p;
	std::vector<glm::vec3> v;
	std::vector<glm::vec3> a;
	GLuint vbo;
	int count;
};

