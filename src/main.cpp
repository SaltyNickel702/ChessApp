#include <iostream>
#include <thread>

#include "Render.h"
#include "Scene.h"
#include "ChessBoard.h"

using namespace std;

thread* renderThread = nullptr;
thread* scriptThread = nullptr;

int main () {
	ChessBoard::Themes::Default board(600);
	board.setCenter(glm::vec2(550,450));
	board.displayText();


	renderThread = new thread(Render::init,1100,900);
	scriptThread = new thread(Scene::Script::init);

	renderThread->join(); //This controls when the program should shut down

	Scene::Script::runLoop.store(false);
	return 0;
}