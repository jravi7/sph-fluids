#include <string>
#include <stdio.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <math.h>
#include "Camera.h"
#include "TrackBall.h"
#include "pipeline.h"
#include "cube.h"
#include "image.h"
#include "mesh.h"
#include "particlesystem.h"
#include "springSystem.h"
#include "grid.h"

Shader shader_obj;
Pipeline* p; 
Camera cam;
TrackBall* trackBall;
ParticleSystem* ps;
Grid* grid;


int gwidth = 800;
int gheight = 600; 
//Keyboard variables
bool keyStates[256];


//Shader var locations
int position_loc = -1;			// Initialize location to -1 
int color_loc= -1;			
int projection_loc = -1;
int texture_loc = -1;
int texcoord_loc = -1;
int normal_loc = -1;

//Transformation Matrices
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
glm::mat4 rot = glm::rotate(model, 45.f, glm::vec3(1, 0, 0));

//Buffers
GLuint vertexBuffers;
GLuint indexBuffers;
GLuint colorBuffers;
GLuint particleBuffers;
GLuint textureId; 
GLuint texcoord; 
glm::vec3 *partPos; 





void InitializeProgram()
{
	shader_obj.add(GL_VERTEX_SHADER, "vs.glsl");
	shader_obj.add(GL_FRAGMENT_SHADER, "fs.glsl");
	shader_obj.CompileProgram();
	shader_obj.deleteShaders();
}

void getShaderVarLoc()
{
	position_loc = shader_obj.getAttributeLocation("position");
	color_loc = shader_obj.getAttributeLocation("color");
	texcoord_loc = shader_obj.getAttributeLocation("texcoord");
	projection_loc = shader_obj.getUniformLoc("projection");
	texture_loc = shader_obj.getUniformLoc("gtexture");
	normal_loc = shader_obj.getUniformLoc("normal");
}

void initCamera(){
	cam.setWindowCoords(gwidth, gheight);
	cam.init();
	cam.setFov(60.0f);
	cam.setAspRatio(gwidth/gheight);
	cam.setNearFar(0.1f, 5000.f);
	cam.setPosition(glm::vec3(0, 0, 50));
	cam.lookAt(glm::vec3(0.0, 0.0, 0.0));
	cam.setVelocity(100);
	printf("%d", cam.mMouseX);
	printf("%d", cam.mMouseY);
	glutWarpPointer(cam.mMouseX, cam.mMouseY);
}


void initOpengl()
{
	glEnable(GL_DEPTH_TEST);
	InitializeProgram();
	initCamera();
	getShaderVarLoc();
	p = new Pipeline(projection_loc, cam.matrix(), glm::vec3(0, 0, 0));
	trackBall = new TrackBall(gwidth, gheight);	
	grid = new Grid(glm::vec3(50,50,0),100, 100, 10);
	ps = new ParticleSystem(glm::vec3(0,0,0), glm::vec3(100,100,0), 1000);
		
}


void processKeyboard(float dt)
{


	if(keyStates['a'] || keyStates['A'])
	{
		cam.offsetPosition(cam.velocity()*dt*-cam.right());
	}

	if(keyStates['s'] || keyStates['S'])
	{
		cam.offsetPosition(cam.velocity()*dt*-cam.forward());
	}

	if(keyStates['w'] || keyStates['W'])
	{
		cam.offsetPosition(cam.velocity()*dt*cam.forward());
	}

	if(keyStates['d'] || keyStates['D'])
	{
		cam.offsetPosition(cam.velocity()*dt*cam.right());
	}

	if(keyStates['j'] || keyStates['J'])
	{
		rot = glm::rotate(rot,-1.f, glm::vec3(0.0, 1.0, 0.0));
	}

	if(keyStates['l'] || keyStates['L'])
	{
		rot = glm::rotate(rot ,1.f, glm::vec3(0.0, 1.0, 0.0));
	}
	if(keyStates['k'] || keyStates['k'])
	{
		rot = glm::rotate(rot,-1.f, glm::vec3(1.0, 0.0, 0.0));
	}

	if(keyStates['i'] || keyStates['I'])
	{
		rot = glm::rotate(rot ,1.f, glm::vec3(1.0, 0.0, 0.0));
	}

}




void idle()
{
	static int last_time = 0;
	int time = glutGet(GLUT_ELAPSED_TIME);
	int elapsed = time-last_time;
	float dt= 0.001f * elapsed;
	last_time = time;
	processKeyboard(dt);
	ps->simulate(dt, glm::vec3(50,50,0));
} 

void display()
{
	glPolygonMode( GL_BACK, GL_LINE );
	//clear screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glAccum(GL_RETURN, 0.95f);

	glClear(GL_ACCUM_BUFFER_BIT);

	//use shader program for subsequent calls
	shader_obj.useProgram();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
	
	glm::mat4 m = cam.matrix() *trackBall->transform()*model;
	
	//Pass uniforms to shader
	if(projection_loc != -1)
		glUniformMatrix4fv(projection_loc, 1, false, glm::value_ptr(m));

	//p->LoadView(cam.matrix());	
	//p->LoadIdentity();
	

	

	//Enable the position location in the shader
	glEnableVertexAttribArray(position_loc);
	glEnableVertexAttribArray(color_loc);
	glEnableVertexAttribArray(texcoord_loc);
	
	
	/*glBindBuffer(GL_ARRAY_BUFFER,vertexBuffers);
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, texcoord);
	glVertexAttribPointer(texcoord_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers);*/
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (void*)0);
	//ps->render(position_loc, color_loc);
	
	
	//ps->render(position_loc, color_loc);
	glPointSize(3.0);
	grid->render(position_loc, color_loc, normal_loc);
	ps->render(position_loc, color_loc);

	
	
	glUseProgram(0);

	glDisableVertexAttribArray(position_loc);
	glDisableVertexAttribArray(color_loc);
	glutPostRedisplay();
	glutSwapBuffers();
	glAccum(GL_ACCUM, 0.9f);
}

void reshape (int w, int h)
{
	gwidth = w;
	gheight = h; 

	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	  case 27:
		  glutLeaveMainLoop();
		  return;
	}
	keyStates[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
	keyStates[key] = false;
}

void mwheel(int wheel, int direction, int x, int y)
{
	
}

void mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		trackBall->setBtnState(true);
		trackBall->trackPoint(x, y);
		cout<<"onMOuseDown"<<endl;
		cout<<trackBall->getIsDown();
	}
	if(state == GLUT_UP)
	{
		trackBall->setBtnState(false);
		cout<<"onMouseUp"<<endl;
		cout<<trackBall->getIsDown();
	}
}

void passiveMotion(int x, int y)
{		
	cam.onMouseMove(x,y);	
}

void motion(int x, int y)
{
	//trackBall->trackMovement(x, y);
}





void createGlutCallBacks()
{
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutReshapeFunc(reshape);
	glutMouseWheelFunc(mwheel);
	glutMouseFunc(mouse);
	glutPassiveMotionFunc(passiveMotion);
	glutMotionFunc(motion);
	glutIdleFunc(idle);
	
	
}
int main(int argc, char** argv)
{
	glutInit(&argc, argv); 
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM);
	glutInitWindowSize(gwidth,gheight);
	glutInitWindowPosition(100, 200);
	glutCreateWindow("Physika - Particle Systems");
	createGlutCallBacks();
	GLenum res = glewInit();
	if(res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	initOpengl();
	glutMainLoop();
	return 0;
}