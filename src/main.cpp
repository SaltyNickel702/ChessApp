#include <iostream>
#include <thread>
#include <windows.h>

#include "Render.h"
#include "UI.h"

using namespace std;

int main () {
	thread* renderThread = new thread(Render::init,800,600);


	// UI::Box myBox(0,0,300,300,glm::vec4(0.05,0.45,0.3,1));
	// myBox.setCenter(glm::vec2(400,300));
	
	unsigned int* imgID = Render::generateImageCache("F-22.png"); //JPEGs don't work quite right
	UI::Image myImg(0,0,300,300, imgID);
	myImg.setCenter(glm::vec2(400,300));

	renderThread->join();
	return 0;
}