#include <iostream>
#include <thread>

#include "Render.h"
#include "Scene.h"

using namespace std;

thread* renderThread = nullptr;
thread* scriptThread = nullptr;

int main () {
	unsigned int* imgID = Render::generateImageCache("F-22.png"); //JPEGs don't work quite right

	Scene s("New Scene");
	
	Scene::UI u("UI",s);
	
	Scene::UI::Image i("f-22",u, 0,0,120,167, imgID);
	i.setCenter(glm::vec2(400,300));
	i.zIndex = 1;

	Scene::UI::Box b("b", u, 0,0,100,100, glm::vec4(.05,.045,0.3,1));


	Scene::UI u3("UI2",s);
	u3.setUIHiddenState(true);
	Scene::UI::Box b3("b3",u3, 0,0,200,200, glm::vec4(1));
	b3.setCenter(glm::vec2(400,300));

	bool u1Used = true;

	Scene::Script scr("Script", s);
	scr.tick = [&]() {
		glm::vec2 nPos = i.pos;
		glm::vec2 dta(0); //delta
		float speed = 400;
		if (Render::isKeyDown(GLFW_KEY_A)) dta += glm::vec2(-speed,0);
		if (Render::isKeyDown(GLFW_KEY_D)) dta += glm::vec2(+speed,0);
		if (Render::isKeyDown(GLFW_KEY_W)) dta += glm::vec2(0,-speed);
		if (Render::isKeyDown(GLFW_KEY_S)) dta += glm::vec2(0,+speed);
		
		nPos+= dta * (float)Scene::Script::deltaTick;

		i.setPosition(nPos);

		// if (Render::isKeyPressed(GLFW_KEY_Q)) Scene::Script::tickQueue.push_back([&](){Scene::setActiveScene("Blank");});
		if (Render::isKeyPressed(GLFW_KEY_Q)) {
			// u.setUIHiddenState(!u.uiHiddenState);
			u3.setUIHiddenState(!u3.uiHiddenState);
		}
		if (Render::isKeyPressed(GLFW_KEY_E)) b.setObjHiddenState(!b.objHiddenState);
	};


	Scene s2("Blank");
	Scene::UI u2("UI2",s2);
	Scene::UI::Box b2("b2",u2, 0,0,200,200, glm::vec4(1));
	b2.setCenter(glm::vec2(400,300));
	Scene::Script scr2("Script", s2);
	scr2.tick = [&]() {
		// if (Render::isKeyPressed(GLFW_KEY_Q)) Scene::Script::tickQueue.push_back([&](){Scene::setActiveScene("New Scene");});
	};


	renderThread = new thread(Render::init,800,600);
	scriptThread = new thread(Scene::Script::init);

	renderThread->join(); //This controls when the program should shut down

	Scene::Script::runLoop.store(false);
	return 0;
}