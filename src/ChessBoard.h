#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include "glm/glm.hpp"
#include <vector>

#include "Scene.h"

namespace ChessBoard {
	class Board {
		public:
			class Piece {
				public:
					static std::string typeToText (unsigned int type); //Takes in the type value and returns piece name

					Piece () : Piece(0) {};
					Piece (unsigned int type);
					unsigned int type = 0; //0: nothing | 1: Pawn | 2: Rook | 3: Knight | 4: Bishop | 5: Queen | 6: King
					int team = -1; //1: white | 0: false | -1: neither
					bool moved = false;
			};

			static int pieceTypesAtStart[8][8];
			Board (); //constructor fills in and manages board data, each subclass is responsible for creating the displays

			Piece* pieces[8][8];
			std::vector<Piece*> takenPieces[2];

			void movePiece (int fx, int fy, int tx, int ty, bool swap);
			void movePiece (int fx, int fy, int tx, int ty);
			bool makeMove (int fx, int fy, int tx, int ty, int testType); // testType 0: Move request | 1: See if available | 2: See if available ignoring check
			bool makeMove (int fx, int fy, int tx, int ty);
			bool isCheck (int fx, int fy, int tx, int ty, int team); // when moving fx fy to tx ty
			bool isCheck (int team);

			void displayText();

			int turn = 1;
			bool makingMove = false;
			glm::vec2 selectedPiece = glm::vec2(-1);

			~Board () {
				for (int x = 0; x < 8; x++) {
					for (int y = 0; y < 8; y++) {
						delete pieces[x][y]; //no coord should have a danging pointer or nullptr
					}
				}
				for (Piece* p : takenPieces[0]) delete p;
				for (Piece* p : takenPieces[1]) delete p;
			}
	};
	namespace Themes {
		class Default : public Board {
			public:
				static bool loaded;
				static Default* currentBoard;

				static glm::vec2 getXY (std::string eID);

				Default () = delete;
				Default (float width);

				Scene scene;
				Scene::UI boardUI;
				Scene::UI pieceUI;
				
				glm::vec2 pos; //use method to move
				float width; //use method to resize

				void setPosition (glm::vec2 nPos);
				void setCenter (glm::vec2 nPos);
				void setWidth (float width);
				void setWidth(float width, bool centered);

				void resetTileColor (int x, int y);
		};
		class dim3D : public Board {
			public:
				dim3D () = delete;
				dim3D (float w, float h);
		};
		class Debug : public Board {
			public:
				Debug ();
		};
	}
}

#endif