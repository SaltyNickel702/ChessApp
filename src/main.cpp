#include <iostream>
#include <thread>
#include <windows.h>

#include "Render.h"
#include "UI.h"

using namespace std;

int main () {
	thread* renderThread = new thread(Render::init,800,600);


	

	renderThread->join();
	return 0;
}