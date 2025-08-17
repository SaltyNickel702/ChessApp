#include "UI.h"

namespace UI {
	std::vector<Element*> Element::elements;

	Box::Box (float x1, float y1, float x2, float y2) : color(glm::vec4(0)) {
		pos = glm::vec2(x1,y1);
		dim = glm::vec2(x2,y2) - pos;

		obj.attr = {3};
		obj.indices = {
			0,1,2,
			0,2,3
		};
	}
}