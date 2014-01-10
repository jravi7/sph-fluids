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
#include "ParticleSystem.h"
#include "TrackBall.h"
#include "pipeline.h"

Shader shader_obj;
Pipeline* p; 
Camera cam;
TrackBall* trackBall;

int gwidth = 800;
int gheight = 600; 
float angle;
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
  
  


  //Keyboard variables
  bool keyStates[256];

  
  int lastTime = 0;


//Buffers
GLuint vertexBuffers;
GLuint indexBuffers;
GLuint colorBuffers;
GLuint particleBuffers;

glm::vec3 *partPos; 

ParticleSystem ps;

glm::vec3 globalPos;
float gRotateX = 0;
float gRotateY = 0;
float gRotateZ = 0;



bool isMouseDown = false;

const float cubeVerts[] = {
	  // front
    -10.0, -10.0,  10.0, 
     10.0, -10.0,  10.0, 
     10.0,  10.0,  10.0, 
    -10.0,  10.0,  10.0, 
    // back
    -10.0, -10.0, -10.0, 
     10.0, -10.0, -10.0, 
     10.0,  10.0, -10.0, 
    -10.0,  10.0, -10.0, 
};
const GLushort cubeIndices[] = {
	//front
	0, 1, 2, 
	2, 3, 0, 
	//top
	3, 2, 6, 
	6, 7, 3, 
	//back
	7, 6, 5, 
	5, 4, 7, 
	//left
	4, 0, 3,
	3, 7, 4, 
	//right
	1, 5, 6, 
	6, 2, 1,
	//bottom
	4, 5, 1, 
	1, 0, 4,
};

GLfloat cube_colors[] = {
    // front colors
    1.0, 0.0, 0.0, 1.0f,
    0.0, 1.0, 0.0, 1.0f,
    0.0, 0.0, 1.0, 1.0f,
    1.0, 1.0, 1.0, 1.0f,
    // back colors
    1.0, 0.0, 0.0, 1.0f,
    0.0, 1.0, 0.0, 1.0f,
    0.0, 0.0, 1.0, 1.0f,
    0.0, 0.0, 0.0, 1.0f,
  };


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
	projection_loc = shader_obj.getUniformLoc("projection");
	texture_loc = shader_obj.getUniformLoc("gtexture");
	texcoord_loc = shader_obj.getUniformLoc("texcoord0");
	normal_loc = shader_obj.getUniformLoc("normal0");
}

void initCamera(){
	cam.setWindowCoords(gwidth, gheight);
	cam.init();
	cam.setFov(60.0f);
	cam.setAspRatio(gwidth/gheight);
	cam.setNearFar(0.1f, 1000.f);
	cam.setPosition(glm::vec3(0, 0, 50));
	cam.lookAt(glm::vec3(0.0, 0.0, 0.0));
	cam.setVelocity(20);
	printf("%d", cam.mMouseX);
	printf("%d", cam.mMouseY);
	glutWarpPointer(cam.mMouseX, cam.mMouseY);
}
void InitializeBuffers()
{
//Vertex Buffers for the cube
	glGenBuffers(1, &vertexBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//VBO for the color
	glGenBuffers(1, &colorBuffers);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//EBO for the indices
	glGenBuffers(1, &indexBuffers);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void initOpengl()
{
	lastTime = glutGet(GLUT_ELAPSED_TIME);
	glEnable(GL_DEPTH_TEST);
	InitializeProgram();
	InitializeBuffers();
	initCamera();
	getShaderVarLoc();
	p = new Pipeline(projection_loc, cam.matrix(), glm::vec3(0, 0, 0));
	trackBall = new TrackBall(gwidth, gheight);
	glPointSize(3.f);
		
}


void initParticles(){
	ps.setBoundary( glm::vec3(-20, -20.0f, -20.0f), glm::vec3(20.0f, 20.0f, 20.0f));
	ps.setRandPos(true);
	ps.setPnum(1000);
	ps.initMemory();
	ps.initParticles();
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
	
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffers);
	glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, colorBuffers);
	glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffers);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
	glutWireSphere(10, 30, 30);
	
	/*p->MoveTo(glm::vec3(25, 0, 0));
	p->Rotate(angle, glm::vec3(0, 0, 1));
	p->Save();
	glutWireSphere(10.f, 30, 30);
	p->Rotate(angle, glm::vec3(1,0,0));
	p->MoveTo(glm::vec3(10, 0, 0));
	p->Retrieve();
	p->Pass();
	glutWireSphere(5.0f, 30, 30);*/
	
	//Displaying particles starts here
	//glGenBuffers(1, &particleBuffers); 
	//glBindBuffer(GL_ARRAY_BUFFER, particleBuffers);
	//glBufferData(GL_ARRAY_BUFFER, ps.pnum*sizeof(glm::vec3), ps.mPos, GL_STREAM_DRAW);
	//glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glDrawArrays(GL_POINTS, 0, (GLsizei)ps.pnum);			//Draw them to screen
	//
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Displaying particles ends here
	
	
	
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
	//cam.onMouseMove(x,y);	
}

void motion(int x, int y)
{
	trackBall->trackMovement(x, y);
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
	float presentTime = glutGet(GLUT_ELAPSED_TIME);
	float dt= 0.01;
	angle += 0.2;
	
	processKeyboard(dt);
	ps.simulate(dt, globalPos, glm::vec3(20,10,-5));
	lastTime = presentTime;
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
	initParticles();
	initOpengl();
	glutMainLoop();
	return 0;
}