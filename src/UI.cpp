#include "UI.h"
#include <iostream>

namespace UI {
	std::vector<Element*> Element::elements;

	Render::Shader* Box::shader = nullptr;
	Box::Box (float x1, float y1, float x2, float y2) : color(glm::vec4(0)) {
		pos = glm::vec2(x1,y1);
		dim = glm::vec2(x2,y2) - pos;

		obj.attr = {2};
		obj.indices = {
			0,1,2,
			0,2,3
		};
		setDimensions(dim.x,dim.y);

		obj.draw = [&](){
			//Apply Uniforms
			glUseProgram(Box::shader->ID);
			glUniform4fv(glGetUniformLocation(Box::shader->ID,"color"),1,glm::value_ptr(color));

			glm::mat4 translate(1);
			translate = glm::translate(translate,glm::vec3(pos,0));
			glUniformMatrix4fv(glGetUniformLocation(Box::shader->ID,"translate"), 1, GL_FALSE, glm::value_ptr(translate));

			glUniform2f(glGetUniformLocation(Box::shader->ID,"screenDim"),Render::width,Render::height);


			//Draw Triangles
			glBindVertexArray(obj.VAO);
			glDrawElements(GL_TRIANGLES,obj.totalIndices,GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
		};
	}
	Box::Box (float x1, float y1, float x2, float y2, glm::vec4 colorIn) : Box(x1,y1,x2,y2) {
		this->color = colorIn;
	}

	void Box::setDimensions (float w, float h) {
		dim = glm::vec2(w,h);
		
		obj.vertices = {
			0,0,
			w,0,
			w,h,
			0,h
		};

		obj.flagReady = true;
	}
}