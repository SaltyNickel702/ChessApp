#include "Render.h"
#include "UI.h"

using namespace std;

//Important Functions
void Render::declareShaders() {
	UI::Box::shader = new Render::Shader("UI_Elements/boxVert.glsl","UI_Elements/boxFrag.glsl");
}


//Under the hood functions
namespace {
	using namespace Render;
	void windowResizeCallback(GLFWwindow* window, int w, int h) { //for when the window gets resized
		glViewport(0, 0, w, h);
		width = w;
		height = h;
	}
	void processInput () {
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

	}
}

//Render.h definitions
namespace Render {
	GLFWwindow* window = nullptr;
	unsigned int width, height;

	vector<Object*> Object::objects;
	void Object::updateBuffers () {
		if (!flagReady) return;
		cleanBuffers();

		float* v = vertices.data();
		unsigned int* i = indices.data();
		unsigned int* a = attr.data();

		totalIndices = indices.size();

		// Calculate attribute layout
		attrPerVert = 0; //Stride per Vertex
		unsigned int sums[attr.size()]; //offset per attribute
		for (int j = 0; j < attr.size(); j++) {
			sums[j] = attrPerVert;
			attrPerVert += a[j];
		}

		totalVertices = vertices.size() / attrPerVert;

		// Generate OpenGL buffers
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), v, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), i, GL_STATIC_DRAW);

		for (int j = 0; j < attr.size(); j++) {
			glVertexAttribPointer(j, a[j], GL_FLOAT, GL_FALSE, attrPerVert * sizeof(float), (void*)(sums[j] * sizeof(float)));
			glEnableVertexAttribArray(j);
		}

		glBindVertexArray(0);
		ready = true;
		flagReady = false;
	}
	void Object::cleanBuffers () {
		if (!ready) return;
		ready = false;
		if (glIsVertexArray(VAO)) glDeleteVertexArrays(1, &VAO);
		if (glIsBuffer(VBO)) glDeleteBuffers(1, &VBO);
		if (glIsBuffer(EBO)) glDeleteBuffers(1, &EBO);
	}

	void init (int w, int h) {
		//Initialize
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Set Version
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Use core version of OpenGL
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //FOR MACOS
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		//Create GLFW window
		window = glfwCreateWindow(w, h, "Chess", NULL, NULL); //Size, title, monitor, shared recourses
		if (window == NULL) {
			cout << "Failed to create GLFW window" << endl;
			glfwTerminate();
			return;// -1;
		}
		glfwMakeContextCurrent(window);


		//Initialize GLAD
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			cout << "Failed to initialize GLAD" << endl;
			return;// -1;
		}

		//Sets GL Viewport (camera)
		glViewport(0, 0, w, h);
		width = w;
		height = h;
		glfwSetFramebufferSizeCallback(window,windowResizeCallback); //assigns resize callback function

		//Loading window Icon
		/* Because I'm not gonna deal with stbi yet
		int Iwidth, Iheight, Ichannels;
		unsigned char* pixels = stbi_load("assets/textures/Chess.png", &Iwidth, &Iheight, &Ichannels, 4);

		if (pixels) {
			GLFWimage images[1];
			images[0].width = Iwidth;
			images[0].height = Iheight;
			images[0].pixels = pixels;
			glfwSetWindowIcon(window, 1, images);
			stbi_image_free(pixels);
		} else {
			printf("Failed to load icon image!\n");
		}
		//*/

		//Load GLFW assets
		declareShaders();

		loop();
	}
	void loop () {
		while (!glfwWindowShouldClose(window)) {
			processInput();

			glClearColor(0.0,0.0,0.0,1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//update pending meshes
			for (Object* o : Object::objects) {
				if (o->flagReady) o->updateBuffers();
				if (!o->hidden) o->draw();
			}

			glfwSwapBuffers(window);
		}
		glfwTerminate();
	};
}