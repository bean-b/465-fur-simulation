#include "ExampleApp.h"

#define FONTSTASH_IMPLEMENTATION
#include <fontstash.h>
#define GLFONTSTASH_IMPLEMENTATION
#include <glfontstash.h>

#include <config/VRDataIndex.h>

#include <glm/gtx/orthonormalize.hpp>
#include <stdlib.h>  

using namespace basicgraphics;
using namespace std;
using namespace glm;





ExampleApp::ExampleApp(int argc, char** argv) : VRApp(argc, argv)
{
	_turntable.reset(new TurntableManipulator(3, 0.3, 0.5));

	drawingModel = true;

	if (drawingModel) {
		_turntable->setCenterPosition(vec3(-0.3, 0.8, 0));
	}
	else {
		_turntable->setCenterPosition(vec3(0, 0, 0));
	}

	_lastTime = 0.0;
    _curFrameTime = 0.0;
	rotation = mat4(1.0);

	
	
	
	
	maxHairLength = 0.1f;
	furCoverage = 4.0f;
	gravPower = -0.12f;
	shadowIntensity = 0.6f;





	modelName = "bunny.obj";
	numLayers = 400;
}

ExampleApp::~ExampleApp()
{
	shutdown();
}


void ExampleApp::onAnalogChange(const VRAnalogEvent &event) {
	if (event.getName() == "FrameStart") {
		_lastTime = _curFrameTime;
		_curFrameTime = event.getValue();
	}
}

void ExampleApp::onButtonDown(const VRButtonEvent &event) {
	string name = event.getName();
	_turntable->onButtonDown(event);
}

void ExampleApp::onButtonUp(const VRButtonEvent &event) {
	_turntable->onButtonUp(event);
}

void ExampleApp::onCursorMove(const VRCursorEvent &event) {
	_turntable->onCursorMove(event);
}


void ExampleApp::onRenderGraphicsContext(const VRGraphicsState &renderState) {
    // This routine is called once per graphics context at the start of the
    // rendering process.  So, this is the place to initialize textures,
    // load models, or do other operations that you only want to do once per
    // frame.
    
	// Is this the first frame that we are rendering after starting the app?
    if (renderState.isInitialRenderCall()) {

		//For windows, we need to initialize a few more things for it to recognize all of the
		// opengl calls.
		#ifndef __APPLE__
			glewExperimental = GL_TRUE;
			GLenum err = glewInit();
			if (GLEW_OK != err)
			{
				std::cout << "Error initializing GLEW." << std::endl;
			}
		#endif     


        glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();

		_shader.use();

		_shader.setUniform("baseColor", vec3(0.124, 0.104, 0.07));
		_shader.setUniform("MaxHairLength", maxHairLength);
		_shader.setUniform("lightPos", vec3(1, 1, 1));
		_shader.setUniform("gravPower", gravPower);
		_shader.setUniform("shadowIntensity", shadowIntensity);

		if (drawingModel) {
			vec4 mat = vec4(0, 0, 0, 0);
			_modelMesh.reset(new Model(modelName, 1.5, mat));
			pushFurTex();
		}
		else {
			setUpSphere();
		}
		
    }
}

void ExampleApp::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
    //glm::vec3 eye_world = glm::vec3(0, 1.5, 1.5+maxHairLength);
	vec3 eyePosition = vec3(_turntable->getPos().x, _turntable->getPos().y, _turntable->getPos().z + 3 + maxHairLength);
	glm::mat4 view = _turntable->frame();

	// Setup the projection matrix so that things are rendered in perspective
	GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
	GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
	
	// Setup the model matrix
	glm::mat4 model = rotation;
    
	// Tell opengl we want to use this specific shader.
	_shader.use();
	
	_shader.setUniform("view_mat", view);
	_shader.setUniform("projection_mat", projection);
	_shader.setUniform("model_mat", model);

	_shader.setUniform("normal_mat", mat3(transpose(inverse(model))));
	
	//used for without the turn table
	//_shader.setUniform("eye_world", eye_world);
	_shader.setUniform("eye_world", eyePosition);
	

	//_modelMesh->draw(_shader);


	if (drawingModel) {
		furLengthLoop();
	}
	else {
		furLengthLoopSphere();
	}
}




void ExampleApp::reloadShaders()
{
	_shader.compileShader("ColorShader.vert", GLSLShader::VERTEX);
	_shader.compileShader("ColorShader.frag", GLSLShader::FRAGMENT);

	_shader.link();
	_shader.use();
}



void ExampleApp::pushFurTex() {
	//an array to hold our pixels

	int width = 500;
	int height = 500;
	int totalPixels = width * height;

	unsigned char colors[1000000];

	for (int i = 0; i < 1000000; i+=4) {
		fillByteInByteArray(colors, i, 0, 0, 0, 0);
	}

	////compute the number of opaque pixels = nr of hair strands
	int nrStrands = (int)(furCoverage * totalPixels);

	int nrOfLayers = numLayers;

	int strandsPerLayer = nrStrands / nrOfLayers;


	////fill texture with opaque pixels
	for (int i = 0; i < nrStrands; i++)
	{
		int x, y;

		x = rand() % height;
		y = rand() % width;

		//compute max layer
		int max_layer = i / strandsPerLayer;
		//normalize into [0..1] range
		float max_layer_n = (float)max_layer / (float)nrOfLayers;

		if (checkNeighbors(colors, x, y, width)) {
			fillByteInByteArray(colors, (x * width + y) * 4, 95, 80, 54, (max_layer_n * 255));
		}		
	}


	//coulumn major order
	tex = Texture::createFromMemory("testName", colors, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8, GL_TEXTURE_2D, width, height, 1);
	
	// Added Jonas' texture path
	//tex->save2D("D:\\comp465\\code\\465-fur-simulation\\resources\\grey2.png");
	// Aurum's tex path
	//tex->save2D("C:\\Users\\mykun\\Documents\\comp465\\code\\465-fur-simulation\\resources\\grey2.png");
	//beans tex path
	//tex->save2D("D:\\Code\\465\\465-fur-simulation\\resources\\grey2.png");


	tex->bind(1);
	_shader.setUniform("furTex", 1);
}

glm::vec3 ExampleApp::getPosition(double latitude, double longitude) {

	// Latitude and longitude should already be in radians
	return vec3(cos(latitude) * cos(longitude), -sin(latitude), cos(latitude) * sin(longitude));
}

void ExampleApp::setUpSphere() {
	const int STACKS = 20;
	const int SLICES = 40;

	std::vector<Mesh::Vertex> cpuVertexArray;
	std::vector<int> cpuIndexArray;
	std::vector<std::shared_ptr<Texture>> textures;

	float radius = 1.0;
	float pi = glm::pi<float>();


	float singleSliceAngle = 2 * pi / SLICES;
	float singleStackAngle = pi / STACKS;
	float currSectorAngle, currStackAngle;
	int count = 0;

	for (int i = 0; i <= STACKS; i++) {
		currStackAngle = pi / 2 - i * singleStackAngle;


		for (int j = 0; j <= SLICES; j++) {
			currSectorAngle = j * singleSliceAngle;
			Mesh::Vertex vert;
			vert.position = getPosition(currStackAngle, currSectorAngle);
			vert.normal = vert.position / radius;

			//thanks to 
			//https://math.stackexchange.com/questions/1006177/compensating-for-distortion-when-projecting-a-2d-texture-onto-a-sphere

			double lat = abs(asin(vert.position.z));
			double dist = 1 - lat / pi * 2;
			double xProj = cos(lat);
			float ratio = (xProj == 0) ? 1 : (float)(dist / xProj);
			float u = (vert.position.x * ratio + 1) / 2;
			float v = (vert.position.y * ratio + 1) / 2;

			vert.texCoord0 = glm::vec2(u, v);

			cpuVertexArray.push_back(vert);
		}
	}

	int k1, k2;
	for (int i = 0; i < STACKS; i++) {
		// Coordinates for each corner of a sector in a sphere
		k1 = i * (SLICES + 1);
		k2 = k1 + SLICES + 1;

		for (int j = 0; j < SLICES; j++, k1++, k2++) {

			// If not the first sector
			if (i != 0) {
				cpuIndexArray.push_back(k1);
				cpuIndexArray.push_back(k2);
				cpuIndexArray.push_back(k1 + 1);
			}

			// If not the last sector
			if (i != (STACKS)) {
				cpuIndexArray.push_back(k1 + 1);
				cpuIndexArray.push_back(k2);
				cpuIndexArray.push_back(k2 + 1);
			}

		}
	}

	const int numVertices = cpuVertexArray.size();
	const int cpuVertexByteSize = sizeof(Mesh::Vertex) * numVertices;
	const int cpuIndexByteSize = sizeof(int) * cpuIndexArray.size();

	//an array to hold our pixels

	int width = 500;
	int height = 500;
	int totalPixels = width * height;

	unsigned char colors[1000000];

	for (int i = 0; i < 1000000; i += 4) {
		fillByteInByteArray(colors, i, 0, 0, 0, 0);
	}

	////compute the number of opaque pixels = nr of hair strands
	int nrStrands = (int)(furCoverage * totalPixels);

	int nrOfLayers = 400;

	int strandsPerLayer = nrStrands / nrOfLayers;


	////fill texture with opaque pixels
	for (int i = 0; i < nrStrands; i++)
	{
		int x, y;

		x = rand() % height;
		y = rand() % width;

		//compute max layer
		int max_layer = i / strandsPerLayer;
		//normalize into [0..1] range
		float max_layer_n = (float)max_layer / (float)nrOfLayers;

		if (checkNeighbors(colors, x, y, width)) {
			fillByteInByteArray(colors, (x * width + y) * 4, 95, 80, 54, (max_layer_n * 255));
		}



	}


	//coulumn major order
	std::shared_ptr<Texture> tex = Texture::createFromMemory("testName", colors, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8, GL_TEXTURE_2D, width, height, 1);

	// Added Jonas' texture path
	//tex->save2D("D:\\comp465\\code\\465-fur-simulation\\resources\\grey2.png");
	// Aurum's tex path
	//tex->save2D("C:\\Users\\mykun\\Documents\\comp465\\code\\465-fur-simulation\\resources\\grey2.png");
	//beans tex path
	//tex->save2D("D:\\Code\\465\\465-fur-simulation\\resources\\grey2.png");

	textures.push_back(tex);
	tex->bind(1);
	_shader.setUniform("furTex", 1);

	sphere_mesh.reset(new Mesh(textures, GL_TRIANGLES, GL_STATIC_DRAW,
		cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray,
		cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
}

void ExampleApp::furLengthLoop() {
	for (int i = 0; i < numLayers; i++) {
		_shader.setUniform("CurrentLayer", ((float)i) / ((float)numLayers));
		_modelMesh->draw(_shader);
	}
}

void ExampleApp::furLengthLoopSphere() {
	for (int i = 0; i < numLayers; i++) {
		_shader.setUniform("CurrentLayer", ((float)i) / ((float)numLayers));
		sphere_mesh->draw(_shader);
	}
}

void ExampleApp::fillByteInByteArray(unsigned char* bytes, int index, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	*(bytes + index) = r;
	*(bytes + 1 + index) = g;
	*(bytes + 2 + index) = b;
	*(bytes + 3 + index) = a;
}

bool ExampleApp::checkNeighbors(unsigned char* bytes, int x, int y, int width) {
	unsigned char mod = 1;
		if (   *(bytes + 3 * mod + ((x - 1) * width + (y - 1)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x)*width + (y - 1)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x + 1) * width + (y-1)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x-1)*width + (y)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x+1)*width + (y)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x - 1) * width + (y + 1)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x)*width + (y + 1)) * 4 * mod) > (unsigned char) 0
			|| *(bytes + 3 * mod + ((x + 1) * width + (y + 1)) * 4 * mod) > (unsigned char) 0) {
			return false;
		}
	return true; 

}
