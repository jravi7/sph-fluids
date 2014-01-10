#include<vector>
#include<gl/glew.h>
/*
Shader Class
	-> Load shaders from file
	-> Attach shader to program
	-> Compile program and link
	-> Check for error messages in the shader

*/

using namespace std;

class Shader{

private:
	vector<GLuint> shaderList;
	GLuint Program;

	char* file_read(const char* filename);
	GLuint CreateShader(GLenum shaderType, const char* strShaderFileData);
	

public:
	Shader();
	void add(GLenum shaderType, const char* filename);
	void deleteShaders();
	void CompileProgram();
	void useProgram();
	GLint getUniformLoc(GLchar* varName);
	GLint getAttributeLocation(std::string varName);


};