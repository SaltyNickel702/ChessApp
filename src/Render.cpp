#include "Render.h"
#include "Scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

using namespace std;

//Important Functions
void Render::declareShaders() {
	Scene::UI::Box::shader = new Render::Shader("UI_Elements/boxVert.glsl","UI_Elements/boxFrag.glsl");
	Scene::UI::Image::shader = new Render::Shader("UI_Elements/imgVert.glsl","UI_Elements/imgFrag.glsl");
}


//Under the hood functions
namespace {
	using namespace Render;
	void windowResizeCallback(GLFWwindow* window, int w, int h) { //for when the window gets resized
		glViewport(0, 0, w, h);
		width = w;
		height = h;
	}

	map<string,unsigned int*> imageQueue;
	void loadImageQueue () {
		for (auto& [src, ptr] : imageQueue) {
			unsigned int texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);

			// Set wrapping and filtering
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// Load image
			string ImgRel = "./assets/textures/" + src;
			int width, height, nrChannels;
			unsigned char* data = stbi_load(ImgRel.c_str(), &width, &height, &nrChannels, 0);
			if (data) {
				GLenum format = (nrChannels == 4) ? GL_RGBA :
								(nrChannels == 3) ? GL_RGB :
								(nrChannels == 1) ? GL_RED : GL_RGB;

				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			} else {
				std::cout << "Failed to load texture: " << src << std::endl;
			}
			stbi_image_free(data);
			glBindTexture(GL_TEXTURE_2D, 0);

			*ptr = texture;
		}
		imageQueue.clear();
	};


	atomic<bool> keysDownArr[GLFW_KEY_LAST + 1];
	atomic<bool> keysPressArr[GLFW_KEY_LAST + 1];
	atomic<bool> keysUpArr[GLFW_KEY_LAST + 1];
	void processInput () {
		glfwPollEvents();
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

		bool ticked = scriptTicked.load(); //If scripting ticked in the last frame
		if (ticked) {
			//reset things that are only for one tick
			for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
				keysPressArr[i].store(false);
			}
		}
		for (int i = GLFW_KEY_SPACE; i <= GLFW_KEY_LAST; i++) {
			bool keyDownBefore = keysDownArr[i].load();
			if (glfwGetKey(Render::window, i) == GLFW_PRESS) {
				if (!keyDownBefore) {
					keysPressArr[i].store(true);
				}
				keysDownArr[i].store(true);
			} else { 
				if (ticked) //Key down events are guaranteed to be present for atleast one tick | If script is running slowly, and a key has been pressed and released between ticks, keysDown should still be true for that tick
					keysDownArr[i].store(false);
			}
		}

		scriptTicked.store(false);
	};
}

//Render.h definitions
namespace Render {
	GLFWwindow* window = nullptr;
	unsigned int width, height;
	std::map<std::string,unsigned int*> imageCache;

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

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CW);

		loop();
	}
	void loop () {
		while (!glfwWindowShouldClose(window)) {
			processInput();

			if (imageQueue.size() > 0) loadImageQueue();

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

	unsigned int* generateImageCache (string imageSrc) {
		unsigned int* ptr = new unsigned int(0);
		imageCache[imageSrc] = ptr; //it shouldn't happen that the image has already been generated;
		imageQueue[imageSrc] = ptr;
		return ptr;
	}

	atomic<bool> scriptTicked{false};
	bool isKeyDown (int GLFW_key) {
		return keysDownArr[GLFW_key].load();
	}
	bool isKeyPressed (int GLFW_key) {
		return keysPressArr[GLFW_key].load();
	}
}