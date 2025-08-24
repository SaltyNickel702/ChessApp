# **Chess App**

#### Current Features
- Chess board with alternating turns
	- No win condition yet
- Current Themes
	- Standard chess.com
- Still working on more

#### Planned Features
- Local Multiplayer
- Single Player against a bot
- Hosting a game on your own PC
	- Share your IP address with a friends to play online
- Different themes for the chess board
	- 3d pieces with a high fov to differentiate pieces from above
	- etc.

### Game Engine Details
- Scenes
	- Each scene can currently be given UI and scripts
	- Scenes are swapped with `Scene::setActiveScene`, which takes the target scene's string ID as an argument
	- UI
		- All UI elements fall under `Scene::UI::Element` for easy rendering and standardized functions across elements
		- Current elements are only colored boxed and images
		- Elements have interaction functions built in, allowing any elements to be made into buttons
	- Scripts
		- Scripts are attached to a scene, and are run when the scene is active
		- Scripts have access to things like time and deltaTime for constant rates
		- A maximum tps can be specified
		- Key events must be accessed through render due to glfw window i/o not being thread safe
			- `Render::isKeyDown` returns if the key has been held down sometime in the last tick
			- `Render::isKeyPressed` return if the key was pressed down in the last tick. This can be used for interactions and toggles
		- Scripts are designed to be run on a separate thread at initialization (`Scene::Script::init`)
- Render Thread
	- Also designed to be run on a separate thread, with the entry point being `Render::init`
	- Rendering is done through the `Render::Object` class
		- Each instance has vectors for vertex data, indices, and the vertex attribute structure
			- The attribute variable lists how long each piece of data is in a vertex (e.g. `{3,2}` for a 3d position and a UV coordinate per vertex)
		- To buffer the vertex data, fill the vectors and set the `flagReady` variable to true, which flags the render loop to buffer the data in between frames. This can be done from any thread.
		- Render objects are intended to be used with a container class. The class constructor should generate the draw function for the object, which applies draw settings, shaders, and uniforms
		- If a render object is not set to hidden, the draw function will automatically be called every frame
	- The `Render::Shader` class compiles shaders and stores their ID
		- New shaders need to be generated on the Render thread, and therefore must be defined in a function that is currently on the top of `Render.cpp`
	- Images can be buffered using `Render::generateImageCache`. The function takes in the location of the image relative to `/assets/textures` and returns a pointer to the ID of the texture on the GPU. This ID can also be found in `Render::imageCache` using the image name as the map key. There is no safety for buffering the same image twice, and will cause a minor memory leak