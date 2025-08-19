#include "Scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

using namespace std;

map<string, Scene*> Scene::scenes;
Scene* Scene::activeScene = nullptr;
void Scene::setActiveScene (string scene) {
	if (scenes.find(scene) == scenes.end()) return; //don't want to switch to non existant scene

	//go through all objects in the scene and set to hidden
	for (auto& [uName, ui] : activeScene->UIs) {
		for (auto& [eName, e] : ui->elements) {
			e->obj.hidden = true;
		}
	}
	activeScene->tickedActive = false;

	//do opposite for the new scene
	Scene* newScene = scenes[scene];
	for (auto& [uName, ui] : newScene->UIs) {
		for (auto& [eName, e] : ui->elements) {
			e->obj.hidden = e->objHiddenState; //preserves previous states
		}
	}

	activeScene = newScene;
}

Scene::Scene (string name) : hasBeenActive(false), tickedActive(false) {
	scenes[name] = this;
	if (!activeScene) activeScene = this;
}


#pragma region UI
using UI = Scene::UI;
UI::UI (string nameIn, Scene &scene) : cScene(&scene), name(nameIn) {
	cScene->UIs[name] = this;
}

void UI::Element::setObjHiddenState (bool state) {
	objHiddenState = state;
	obj.hidden = state;
}

Render::Shader* UI::Box::shader = nullptr;
UI::Box::Box (string name, UI &ui, float x1, float y1, float x2, float y2) : Element(name, ui), color(glm::vec4(0)) {
	pos = glm::vec2(x1,y1);
	dim = glm::vec2(x2,y2) - pos;

	obj.attr = {2}; //pos
	obj.indices = {
		0,1,2,
		0,2,3
	};
	setDimensions(dim.x,dim.y);

	obj.draw = [&](){
		//Apply Uniforms
		glUseProgram(Box::shader->ID);
        glEnable(GL_DEPTH_TEST);


		glUniform4fv(glGetUniformLocation(Box::shader->ID,"color"),1,glm::value_ptr(color));

		glm::mat4 translate(1);
		translate = glm::translate(translate,glm::vec3(pos,-zIndex));
		glUniformMatrix4fv(glGetUniformLocation(Box::shader->ID,"translate"), 1, GL_FALSE, glm::value_ptr(translate));

		glUniform2f(glGetUniformLocation(Box::shader->ID,"screenDim"),Render::width,Render::height);


		//Draw Triangles
		glBindVertexArray(obj.VAO);
		glDrawElements(GL_TRIANGLES,obj.totalIndices,GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	};
}
UI::Box::Box (string name, UI &ui, float x1, float y1, float x2, float y2, glm::vec4 colorIn) : Box(name,ui,x1,y1,x2,y2) {
	this->color = colorIn;
}

void UI::Box::setDimensions (float w, float h) {
	dim = glm::vec2(w,h);
	
	obj.vertices = {
		0,0,
		w,0,
		w,h,
		0,h
	};

	obj.flagReady = true;
}


Render::Shader* UI::Image::shader = nullptr;
UI::Image::Image (string name, UI &ui, float x, float y, float w, float h, unsigned int* imgID) : Element(name, ui) {
	pos = glm::vec2(x,y);
	dim = glm::vec2(w,h);
	imageID = imgID;

	obj.attr = {2,2}; //pos, UV
	obj.indices = {
		0,1,2,
		0,2,3
	};
	setDimensions(dim.x,dim.y);

	obj.draw = [&](){
		//Apply Uniforms
		glUseProgram(Image::shader->ID);

        glEnable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *imageID);
		glUniform1i(glGetUniformLocation(Image::shader->ID,"tex0"),0);

		glm::mat4 translate(1);
		translate = glm::translate(translate,glm::vec3(pos,-zIndex));
		glUniformMatrix4fv(glGetUniformLocation(Image::shader->ID,"translate"), 1, GL_FALSE, glm::value_ptr(translate));

		glUniform2f(glGetUniformLocation(Image::shader->ID,"screenDim"),Render::width,Render::height);


		//Draw Triangles
		glBindVertexArray(obj.VAO);
		glDrawElements(GL_TRIANGLES,obj.totalIndices,GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	};
}
void UI::Image::setDimensions (float w, float h) {
	dim = glm::vec2(w,h);
	
	obj.vertices = {
		0,0, 0,0,
		w,0, 1,0,
		w,h, 1,1,
		0,h, 0,1
	};

	obj.flagReady = true;
}
#pragma endregion


#pragma region Scripting
using Script = Scene::Script;

double Script::maxTPS = 120;
double Script::time = 0;
double Script::deltaTick = 0;
double Script::TPS = 0;
double lastTick = 0;

std::vector<function<void()>> Script::tickQueue;

atomic<bool> Script::runLoop{true};
void Scene::Script::init () { //In case I need to do init things
	Script::loop();
}
void Script::loop () {
	while (runLoop.load()) {
		Render::scriptTicked.store(true);

		Scene* scene = Scene::activeScene;

		lastTick = time;
		time = glfwGetTime();
		deltaTick = time - lastTick;
		double targetTInt = 1.0f/maxTPS;
		if (deltaTick < targetTInt) {
			double dif = targetTInt - deltaTick;
			int us = dif * 1e+6; //microseconds
			this_thread::sleep_for(chrono::microseconds(us));
			
			//perform the same thing again for an accurate measurement
			time = glfwGetTime();
			deltaTick = time - lastTick;
		}
		TPS = 1.0f / deltaTick;


		//run tick queue
		for (function<void()> f : tickQueue) f();
		tickQueue.clear();

		//Run scripts
		for (auto& [scriptName, s] : scene->scripts) {
			if (!scene->hasBeenActive) s->onStart();
			if (!scene->tickedActive) s->onSceneLoad();
			s->tick();
		}

		if (!scene->hasBeenActive) scene->hasBeenActive = true;
		if (!scene->tickedActive) scene->tickedActive = true;
	}
}

#pragma endregion