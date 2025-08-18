#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <functional>

#include "Render.h"

#ifndef UI_H
#define UI_H

namespace UI {
	class Element {
		public:
			static std::vector<Element*> elements;

			Element () : zIndex(0) {
				elements.push_back(this);
			}

			std::function<void()> onClick;
			std::function<void()> onHover;
			std::function<void()> onLeave;
			
			glm::vec2 pos; //Top left corner
			float rot; //Rotation around z axis; only in render, not for interaction
			glm::vec2 dim; //Width and Height
			float zIndex; //Distance from camera

			virtual void setPosition (glm::vec2 nPos) {pos = nPos;};
			virtual void setCenter (glm::vec2 nPos) {pos = nPos - dim*0.5f;};
			virtual glm::vec2 getCenter () {return pos + dim*0.5f;};

			virtual ~Element () {
				//clear from elements vector
				auto pointer = std::find(elements.begin(), elements.end(), this);
				if (pointer != elements.end()) {
					elements.erase(pointer);
				}
			}
	};

	class Box : public Element { //most basic element, can have a color and bounds
		public:
			static Render::Shader* shader;

			Box () = delete;
			Box (float x1, float y1, float x2, float y2);
			Box (float x1, float y1, float x2, float y2, glm::vec4 color);

			void setDimensions (float w, float h);

			glm::vec4 color;
		private:
			Render::Object obj;
	};

	class Image : public Element {
		public:
			static Render::Shader* shader;

			Image () = delete;
			Image (float x, float y, float w, float h, unsigned int* imageID);

			void setDimensions (float w, float h);

			unsigned int* imageID;
		private:
			Render::Object obj;
	};
}

#endif