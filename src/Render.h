#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <functional>
#include <vector>
#include <algorithm>


#ifndef RENDER_H
#define RENDER_H

/*

Anything in the render loop should be independant from the rest of the engine
To pause rendering, don't update information in the Render::Object

*/
namespace Render {
	//Class Definitions
	class Shader {
		public:
			unsigned int ID;

			Shader (const std::string vertexPath, const std::string fragmentPath) {
				using namespace std;
				#include <iostream>

				string vertexCode;
				string fragmentCode;

				ifstream vShader;
				ifstream fShader;

				vShader.exceptions(ifstream::failbit | ifstream::badbit);
				fShader.exceptions(ifstream::failbit | ifstream::badbit);

				//Read Files
				try {
					vShader.open("./assets/shaders/" + vertexPath);
					fShader.open("./assets/shaders/" + fragmentPath);

					stringstream vShaderStream, fShaderStream;
					vShaderStream << vShader.rdbuf();
					fShaderStream << fShader.rdbuf();

					vShader.close();
					fShader.close();

					vertexCode = vShaderStream.str();
					fragmentCode = fShaderStream.str();
				} catch (ifstream::failure e) {
					cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" <<endl;
				}
				const char* vertexCString = vertexCode.c_str();
				const char* fragmentCString = fragmentCode.c_str();


				//Compile Shaders
				unsigned int vertex, fragment;
				int success;
				char infoLog[512];

				//Vertex
				vertex = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vertex,1,&vertexCString,NULL);
				glCompileShader(vertex);

				glGetShaderiv(vertex,GL_COMPILE_STATUS,&success);
				if (!success) {
					glGetShaderInfoLog(vertex,512,NULL,infoLog);
					cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
				}

				//fragment
				fragment = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fragment,1,&fragmentCString,NULL);
				glCompileShader(fragment);

				glGetShaderiv(fragment,GL_COMPILE_STATUS,&success);
				if (!success) {
					glGetShaderInfoLog(fragment,512,NULL,infoLog);
					cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
				}

				
				//Program Creation
				ID = glCreateProgram();
				glAttachShader(ID,vertex);
				glAttachShader(ID,fragment);
				glLinkProgram(ID);

				glGetProgramiv(ID,GL_LINK_STATUS,&success);
				if (!success) {
					glGetShaderInfoLog(fragment,512,NULL,infoLog);
					cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
				}

				glDeleteShader(vertex);
				glDeleteShader(fragment);
			};

			void use() {
				glUseProgram(ID);
			}

			~Shader () {
				glDeleteProgram(ID);
			}
	};
	class Object { //Contains mesh information
		public:
			Object () : ready(false), flagReady(false), hidden(false), draw([](){}) {
				objects.push_back(this);
			};
			
			static std::vector<Object*> objects;

			bool hidden;

			bool flagReady; //Ready to be pushed to buffer
			std::vector<float> vertices;
			std::vector<unsigned int> indices;
			std::vector<unsigned int> attr;


			bool ready; //Ready to be drawn
			unsigned int VAO, EBO, VBO;

			unsigned int totalIndices;
			unsigned int totalVertices;
        	unsigned int attrPerVert;

			void updateBuffers (); //must be called on render thread
			void cleanBuffers ();

			std::function<void()> draw;

			~Object () {
				cleanBuffers();
				
				//clear from objects vector
				auto pointer = find(objects.begin(), objects.end(), this);
				if (pointer != objects.end()) {
					objects.erase(pointer);
				}
			}
	};


	//Variable Defs
	extern GLFWwindow* window;
	extern unsigned int width, height;


	//Function Defs
	void init(int w, int h); //initializes glfw and starts loop
	void loop (); //Starts the render loop

	void declareShaders (); //Only function that should be messed with; used to create shaders bc all glfw stuff is done on the same thread
}

#endif