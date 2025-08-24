#include <glm/glm.hpp>
#include <map>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <atomic>
#include <mutex>

#include "Render.h"

#ifndef SCENE_H
#define SCENE_H

class Scene {
	public:
		static std::map<std::string, Scene*>	scenes;
		static Scene* activeScene;
		static void setActiveScene (std::string scene);

		Scene () = delete;
		Scene (std::string name);
		const std::string name;

		bool hasBeenActive; //stays true if scene becomes inactive
		bool tickedActive; //resets every time scene is inactive

		//UI
		class UI {
			public:
				UI () = delete;
				UI (std::string name, Scene &scene);
				const std::string name;

				bool uiHiddenState = false;
				void setUIHiddenState (bool state); //Control if the element is visible when scene is active


				class Element { //container function that has standard functions
					public:
						Element () = delete;
						Element (std::string nameIn, UI &ui) : cUI(&ui), name(nameIn), zIndex(0) {
							cUI->elements[name] = this;
							if (cUI->cScene != Scene::activeScene || cUI->uiHiddenState) obj.hidden = true; //adhere to hidden states of UI and scene
						};
						const std::string name;

						std::function<void(Element* e)> onClick;
						std::function<void(Element* e)> onHover;
						std::function<void(Element* e)> onLeave;
						
						Render::Object obj;
						void setObjHiddenState (bool state); //Control if the element is visible when scene is active
						bool objHiddenState = false; //The state to which the object will be set to when scene becomes active | Use function to modify

						glm::vec2 pos; //Top left corner
						float rot; //Rotation around z axis; only in render, not for interaction
						glm::vec2 dim; //Width and Height
						float zIndex; //Distance from camera

						virtual void setPosition (glm::vec2 nPos) {pos = nPos;};
						virtual void setCenter (glm::vec2 nPos) {pos = nPos - dim*0.5f;};
						virtual glm::vec2 getCenter () {return pos + dim*0.5f;};

						virtual ~Element () {
							cUI->elements.erase(name);
						}

						UI* cUI; //UI the element is attached to | Container UI | Don't edit
				};
				std::map<std::string, Element*> elements;


				class Box : public Element { //most basic element, can have a color and bounds
					public:
						static Render::Shader* shader;

						Box () = delete;
						Box (std::string name, UI &ui, float x1, float y1, float x2, float y2);
						Box (std::string name, UI &ui, float x1, float y1, float x2, float y2, glm::vec4 color);

						void setDimensions (float w, float h);

						glm::vec4 color;
				};

				class Image : public Element { //Box, but with a texture instead of color
					public:
						static Render::Shader* shader;

						Image () = delete;
						Image (std::string name, UI &ui, float x, float y, float w, float h, unsigned int* imageID);

						void setDimensions (float w, float h);

						unsigned int* imageID;
				};
			protected:
				Scene* cScene; //Scene the UI is attached to | Container Scene
		};
		std::map<std::string, UI*> UIs;
		

		class Script {
			public:
				static void init ();
				static void loop ();
				static std::atomic<bool> runLoop;

				static std::vector<std::function<void()>> tickQueue;
				
				static double maxTPS;
				static double time;
				static double deltaTick;
				static double TPS;

				const std::string name;

				Script () = delete;
				Script (std::string nameIn, Scene &scene) : cScene(&scene), name(nameIn), onStart([](){}), onSceneLoad([](){}), tick([](){}) {
					cScene->scripts[name] = this;
				};

				std::function<void()> onStart; //runs the first time the scene loads
				std::function<void()> onSceneLoad; //runs every time the scene is loaded
				std::function<void()> tick; //runs every tick;

				~Script () {
					cScene->scripts.erase(name);
				}
			protected:
				Scene* cScene;
		};
		std::map<std::string, Script*> scripts;
		Script sceneManager;
};

#endif