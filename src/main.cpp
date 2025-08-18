#include <iostream>
#include <thread>
#include <windows.h>

#include "Render.h"
#include "UI.h"

using namespace std;

int main () {
	thread* renderThread = new thread(Render::init,800,600);


	UI::Box myBox(0,0,400,300,glm::vec4(0.05,0.45,0.3,1));


	renderThread->join();
	return 0;
}