#include "UI.h"
#include <iostream>

namespace UI {
	std::vector<Element*> Element::elements;

	Render::Shader* Box::shader = nullptr;
	Box::Box (float x1, float y1, float x2, float y2) : color(glm::vec4(0)) {
		pos = glm::vec2(x1,y1);
		dim = glm::vec2(x2,y2) - pos;

		obj.attr = {2}; //pos
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


	Render::Shader* Image::shader = nullptr;
	Image::Image (float x, float y, float w, float h, unsigned int* imgID) {
		pos = glm::vec2(x,y);
		dim = glm::vec2(w,h);
		imageID = imgID;

		obj.attr = {2,2}; //pos, UV
		obj.indices = {
			0,1,2,
			0,2,3
		};
		setDimensions(dim.x,dim.y);

		obj.draw = [&](){
			//Apply Uniforms
			glUseProgram(Image::shader->ID);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, *imageID);
			glUniform1i(glGetUniformLocation(Image::shader->ID,"tex0"),0);

			glm::mat4 translate(1);
			translate = glm::translate(translate,glm::vec3(pos,0));
			glUniformMatrix4fv(glGetUniformLocation(Image::shader->ID,"translate"), 1, GL_FALSE, glm::value_ptr(translate));

			glUniform2f(glGetUniformLocation(Image::shader->ID,"screenDim"),Render::width,Render::height);


			//Draw Triangles
			glBindVertexArray(obj.VAO);
			glDrawElements(GL_TRIANGLES,obj.totalIndices,GL_UNSIGNED_INT, 0);

			glBindVertexArray(0);
		};
	}
	void Image::setDimensions (float w, float h) {
		dim = glm::vec2(w,h);
		
		obj.vertices = {
			0,0, 0,0,
			w,0, 1,0,
			w,h, 1,1,
			0,h, 0,1
		};

		obj.flagReady = true;
	}
}