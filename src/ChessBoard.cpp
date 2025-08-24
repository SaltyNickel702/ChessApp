#include "ChessBoard.h"

#include <iostream>
#include <cstdint>

using namespace ChessBoard;
using namespace std;

Board::Piece::Piece(unsigned int typeInt) : type(typeInt) {}
string Board::Piece::typeToText (unsigned int t) {
	switch (t) {
		case 1:
			return "Pawn";
			break;
		case 2:
			return "Rook";
			break;
		case 3:
			return "Knight";
			break;
		case 4:
			return "Bishop";
			break;
		case 5:
			return "Queen";
			break;
		case 6:
			return "King";
			break;
		default:
			return "";
	}
}


int Board::pieceTypesAtStart[8][8] = {
	{2,3,4,5,6,4,3,2},
	{1,1,1,1,1,1,1,1},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0},
	{1,1,1,1,1,1,1,1},
	{2,3,4,5,6,4,3,2}
};
Board::Board () {
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece*& ps = pieces[x][y]; //piece slot
			ps = new Piece(pieceTypesAtStart[y][x]); // rows are listed in pieceTypesAtStart, not columns
			if (y <= 1) { //white
				ps->team = 1;
			} else if (y >= 6) {//black
				ps->team = 0;
			} else ps->team = -1;
		}
	}
}
void Board::movePiece (int fx, int fy, int tx, int ty, bool swap) {
	if (fx < 0 || fy < 0 || fx > 7 || fy > 7) return;
	if (tx < 0 || ty < 0 || tx > 7 || ty > 7) return;

	Piece* fP = pieces[fx][fy];
	Piece* tP = pieces[tx][ty];

	fP->moved = true;
	tP->moved = true;

	pieces[tx][ty] = fP;
	pieces[fx][fy] = swap ? tP : new Piece(0);
	if (!swap) {
		if (tP->type != 0 && tP->team != -1) {
			takenPieces[tP->team].push_back(tP);
		} else delete tP;
	}
}
void Board::movePiece (int fx, int fy, int tx, int ty) {movePiece(fx,fy,tx,ty,false);}
bool Board::makeMove (int fx, int fy, int tx, int ty, bool test) {
	if (fx < 0 || fy < 0 || fx > 7 || fy > 7) return false; // out of bounds
	if (tx < 0 || ty < 0 || tx > 7 || ty > 7) return false;

	if (glm::vec2(fx,fy) == glm::vec2(tx,ty)) return false; // same piece

	Piece* p1 = pieces[fx][fy];
	Piece* p2 = pieces[tx][ty];

	if (p1->team == p2->team) return false; // same team
	if (p1->team != turn) return false; // not the correct turn

	switch (p1->type) { // needs to pass individual tests | Pass means the switch is broken
		case 1: { // Pawn
			if (fx == tx && fy-ty == ((p1->team == 1) ? -1 : 1) && p2->type == 0) goto TestPassed; // straight forward relative to team; no capture
			if (abs(fx-tx) == 1 && fy-ty == ((p1->team == 1) ? -1 : 1) && p2->type != 0) goto TestPassed; // diagonal move; capture
			if (!p1->moved && fx == tx && fy-ty == ((p1->team == 1) ? -2 : 2) && p2->type == 0) goto TestPassed; // first pawn move; no capture
			return false;
		}
		case 2: { // Rook
			if (fx == tx || fy == ty) {
				//check for collisions
				glm::vec2 dir = glm::vec2(tx,ty) - glm::vec2(fx,fy);
				dir = glm::normalize(dir); //to only move 1 unit per loop
				
				glm::vec2 pos = glm::vec2(fx,fy) + dir;
				while ((pos.x >= 0 && pos.y >= 0 && pos.x <= 7 && pos.y <= 7)) {
					if (pos == glm::vec2(tx,ty)) goto TestPassed; // Arrived at correct position
					
					Piece* np = pieces[(int)pos.x][(int)pos.y];
					if (np->type != 0) return false; // arrived at another piece, but isn't at position
					
					pos+=dir;
				}
			}
			return false;
		}
		case 3: { // Knight
			glm::vec2 dir = glm::vec2(tx,ty) - glm::vec2(fx,fy);
			dir = glm::abs(dir);
			if ((dir.x == 1 && dir.y == 2) || (dir.x == 2 && dir.y == 1)) goto TestPassed;
			return false;
		}
		case 4: { // Bishop
			glm::vec2 dir = glm::vec2(tx,ty) - glm::vec2(fx,fy);
			if (abs(dir.x/dir.y) == 1) { //diagonals
				dir = glm::normalize(dir);
				dir.x = ceil(abs(dir.x)) * (signbit(dir.x) ? -1 : 1); //1 or 0 | +-
				dir.y = ceil(abs(dir.y)) * (signbit(dir.y) ? -1 : 1); //1 or 0 | +-

				glm::vec2 pos = glm::vec2(fx,fy) + dir;
				while ((pos.x >= 0 && pos.y >= 0 && pos.x <= 7 && pos.y <= 7)) {
					if (pos == glm::vec2(tx,ty)) goto TestPassed; // Arrived at correct position
					
					Piece* np = pieces[(int)pos.x][(int)pos.y];
					if (np->type != 0) return false; // arrived at another piece, but isn't at position
					
					pos+=dir;
				}
			}
			return false;
		}
		case 5: { // Queen
			glm::vec2 dir = glm::vec2(tx,ty) - glm::vec2(fx,fy);
			if ((fx == tx || fy == ty) || (abs(dir.x/dir.y) == 1)) { //Rook + Bishop Rules
				dir = glm::normalize(dir);
				dir.x = ceil(abs(dir.x)) * (signbit(dir.x) ? -1 : 1); //1 or 0 | +-
				dir.y = ceil(abs(dir.y)) * (signbit(dir.y) ? -1 : 1); //1 or 0 | +-

				glm::vec2 pos = glm::vec2(fx,fy) + dir;
				while ((pos.x >= 0 && pos.y >= 0 && pos.x <= 7 && pos.y <= 7)) {				
					if (pos == glm::vec2(tx,ty)) goto TestPassed; // Arrived at correct position

					Piece* np = pieces[(int)pos.x][(int)pos.y];
					if (np->type != 0) return false; // arrived at another piece, but isn't at position

					pos+=dir;
				}
			}
			return false;
		}
		case 6: { // King
			glm::vec2 dir = glm::vec2(tx,ty) - glm::vec2(fx,fy);
			if (abs(dir.x) <= 1 && abs(dir).y <= 1) {
				for (int dx = -1; dx <= 1; dx++) { // Check for the other king
					if (tx + dx < 0 || tx + dx > 7) continue; //Out of x bounds
					for (int dy = -1; dy <= 1; dy++) {
						if (tx + dx == fx && ty + dy == fy) continue; // Don't check self
						if (tx + dy < 0 || ty + dy > 7) continue; //out of y bounds

						Piece* check = pieces[tx + dx][ty + dy];
						if (check->type == 6) return false; // King is next to the position
					}
				}
				// I'm gonna allow checkmate moves because that's your own fault
				goto TestPassed;
			}
			
			return false;
		}
	}
	TestPassed:;


	if (!test) Board::movePiece (fx, fy, tx, ty, false);
	return true;
}
bool Board::makeMove (int fx, int fy, int tx, int ty) {return Board::makeMove(fx,fy,tx,ty,false);}
void Board::displayText () {
	cout << endl;
	for (int y = 7; y >= 0; y--) {
		for (int x = 0; x < 8; x++) {
			cout << pieces[x][y]->type << " ";
		}
		cout << endl;
	}
	cout << endl;
}



Themes::Debug::Debug () {
	// cout << "created Console Board" << endl;
}


bool Themes::Default::loaded = false;
Themes::Default* Themes::Default::currentBoard = nullptr;
glm::vec2 Themes::Default::getXY (std::string eID) {
	return glm::vec2(eID[0] - '0',eID[2] - '0');
}
Themes::Default::Default (float widthIn /* square, no reason for w + h */) : scene("DefaultBoard"), boardUI("board",scene), pieceUI("pieces",scene), pos(glm::vec2(0)), width(widthIn) {
	if (!loaded) {
		//load textures
		loaded = true;

		Render::generateImageCache("PawnWhite.png","defaultPawnW");
		Render::generateImageCache("RookWhite.png","defaultRookW");
		Render::generateImageCache("KnightWhite.png","defaultKnightW");
		Render::generateImageCache("BishopWhite.png","defaultBishopW");
		Render::generateImageCache("QueenWhite.png","defaultQueenW");
		Render::generateImageCache("KingWhite.png","defaultKingW");

		Render::generateImageCache("PawnBlack.png","defaultPawnB");
		Render::generateImageCache("RookBlack.png","defaultRookB");
		Render::generateImageCache("KnightBlack.png","defaultKnightB");
		Render::generateImageCache("BishopBlack.png","defaultBishopB");
		Render::generateImageCache("QueenBlack.png","defaultQueenB");
		Render::generateImageCache("KingBlack.png","defaultKingB");

	}
	if (currentBoard != nullptr) delete currentBoard;
	currentBoard = this;


	float tW = width/8;
	//Generate Tile Backgrounds
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			string tName = to_string(x) + "." + to_string(y);
			Scene::UI::Box* b = new Scene::UI::Box(tName,boardUI, x*tW, (7-y)*tW, x*tW+tW, (7-y)*tW+tW);
			bool pat = x % 2 ^ y % 2;
			b->color = (!pat ? glm::vec4(118, 150, 86, 255) : glm::vec4(238, 238, 210, 255)) * (1.0f/255);
		}
	}

	//Generate Pieces
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Piece* p = pieces[x][y];

			string imageName = "default" + Piece::typeToText(p->type) + ((p->team == 1) ? "W" : "B");
			string tName = to_string(x) + "." + to_string(y);
			Scene::UI::Image* i = new Scene::UI::Image(tName,pieceUI, x*tW, (7-y)*tW, tW,tW, (p->type == 0) ? 0 : Render::imageCache[imageName]);
			i->zIndex = 1;
		}
	}


	Scene::Script* script = new Scene::Script("Script",scene);
	script->tick = [&]() {
		// Get Mouse Clicks
		// Easier to manage from one script

		//Define Variables
		glm::vec2 mp = Render::getMousePos();
		Scene* scene = Scene::scenes["DefaultBoard"];
		Scene::UI* boardUI = scene->UIs["board"];
		Scene::UI* pieceUI = scene->UIs["pieces"];

		Default* b = currentBoard;

		glm::vec2 relXY = mp - b->pos;
		glm::vec2 pXY = relXY * (1.0f / b->width) * 8.0f;
		pXY.x = floor(pXY.x);
		pXY.y = 7 - floor(pXY.y);

		//Get selected tile coords
		if (pXY.x >= 0 && pXY.x < 8 && pXY.y >= 0 && pXY.y < 8) {
			if (Render::isMouseBtnPressed(GLFW_MOUSE_BUTTON_LEFT)) {
				string tName1 = to_string((int)pXY.x) + "." + to_string((int)pXY.y);
				Piece* p1 = pieces[(int)pXY.x][(int)pXY.y];
				Scene::UI::Image* p1Img = dynamic_cast<Scene::UI::Image*>(pieceUI->elements[tName1]);
				Scene::UI::Box* p1Box = dynamic_cast<Scene::UI::Box*>(boardUI->elements[tName1]);

				if (makingMove) {
					string tName0 = to_string((int)selectedPiece.x) + "." + to_string((int)selectedPiece.y);
					Piece* p0 = pieces[(int)selectedPiece.x][(int)selectedPiece.y];
					Scene::UI::Image* p0Img = dynamic_cast<Scene::UI::Image*>(pieceUI->elements[tName0]);
					Scene::UI::Box* p0Box = dynamic_cast<Scene::UI::Box*>(boardUI->elements[tName0]);

					bool moveSuccess = makeMove(selectedPiece.x,selectedPiece.y, pXY.x,pXY.y);

					if (moveSuccess) {
						turn = !(bool)turn;
						displayText();
					}

					makingMove = false;
					selectedPiece = glm::vec2(-1);
				} else {
					//First selection
					if (p1->type != 0 && p1->team == turn) {
						selectedPiece = pXY;
						makingMove = true;

						p1Box->color*= glm::vec4(1.1,1.3,1.2,1);
					}
				}
			}
		}



		for (auto& [id,e] : pieceUI->elements) { // Update Pieces
			Scene::UI::Image* i = dynamic_cast<Scene::UI::Image*>(e);
			glm::vec2 pos = getXY(id);
			Piece* p = pieces[(int)pos.x][(int)pos.y];
			string imageName = "default" + Piece::typeToText(p->type) + ((p->team == 1) ? "W" : "B");
			i->imageID = (p->type == 0) ? 0 : Render::imageCache[imageName];
		}
		for (auto& [id,e] : boardUI->elements) {
			Scene::UI::Box* b = dynamic_cast<Scene::UI::Box*>(e);
			glm::vec2 pos = getXY(id);
			resetTileColor(pos.x,pos.y);

			if (makingMove && makeMove(selectedPiece.x, selectedPiece.y, pos.x, pos.y, true)) {
				b->color = (glm::vec4(1,0.8,0.8,1) + b->color) * 0.5f;
			}
		}

		// Highlight Selected Piece
		if (makingMove) dynamic_cast<Scene::UI::Box*>(boardUI->elements[to_string((int)selectedPiece.x) + "." + to_string((int)selectedPiece.y)])->color *= glm::vec4(1.1,1.3,1.2,1);
	};
}
void Themes::Default::setPosition (glm::vec2 nPos) {
	glm::vec2 dp = nPos - pos;
	
	for (auto& [name,e] : boardUI.elements) {
		e->pos += dp;
	}
	for (auto& [name,e] : pieceUI.elements) {
		e->pos += dp;
	}

	pos = nPos;
}
void Themes::Default::setCenter (glm::vec2 nPos) {setPosition(nPos - glm::vec2(width/2));}
void Themes::Default::setWidth (float widthIn, bool centered) {
	int oldW = width;

	width = widthIn;

	float tW = width/8;

	for (auto& [name,e] : boardUI.elements) {
		Scene::UI::Box* b = dynamic_cast<Scene::UI::Box*>(e);
		glm::vec2 xy = getXY(name);
		b->setDimensions(tW,tW);
		b->setPosition(pos + xy*tW);
	}

	if (centered) {
		setCenter(pos + glm::vec2(oldW * 0.5f));
	}
}
void Themes::Default::setWidth (float widthIn) {setWidth(widthIn,true);}
void Themes::Default::resetTileColor (int x, int y) {
	string tName = to_string(x) + "." + to_string(y);
	Scene::UI::Box* t = dynamic_cast<Scene::UI::Box*>(boardUI.elements[tName]);
	t->color = (!(x % 2 ^ y % 2) ? glm::vec4(118, 150, 86, 255) : glm::vec4(238, 238, 210, 255)) * (1.0f/255);
}