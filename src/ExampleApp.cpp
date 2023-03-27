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
	_lastTime = 0.0;
    _curFrameTime = 0.0;
}

ExampleApp::~ExampleApp()
{
	glfonsDelete(fs);
	shutdown();
}

void ExampleApp::onAnalogChange(const VRAnalogEvent &event) {
    // This routine is called for all Analog_Change events.  Check event->getName()
    // to see exactly which analog input has been changed, and then access the
    // new value with event->getValue().
    
	if (event.getName() == "FrameStart") {
		_lastTime = _curFrameTime;
		_curFrameTime = event.getValue();
	}


}

void ExampleApp::onButtonDown(const VRButtonEvent &event) {
    // This routine is called for all Button_Down events.  Check event->getName()
    // to see exactly which button has been pressed down.
	
	//std::cout << "ButtonDown: " << event.getName() << std::endl;
}

void ExampleApp::onButtonUp(const VRButtonEvent &event) {
    // This routine is called for all Button_Up events.  Check event->getName()
    // to see exactly which button has been released.

	//std::cout << "ButtonUp: " << event.getName() << std::endl;
}

void ExampleApp::onCursorMove(const VRCursorEvent &event) {
	// This routine is called for all mouse move events. You can get the absolute position
	// or the relative position within the window scaled 0--1.
	
	//std::cout << "MouseMove: "<< event.getName() << " " << event.getPos()[0] << " " << event.getPos()[1] << std::endl;
}

void ExampleApp::onTrackerMove(const VRTrackerEvent &event) {
    // This routine is called for all Tracker_Move events.  Check event->getName()
    // to see exactly which tracker has moved, and then access the tracker's new
    // 4x4 transformation matrix with event->getTransform().

	// We will use trackers when we do a virtual reality assignment. For now, you can ignore this input type.
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
        glClearDepth(1.0f);
        glDepthFunc(GL_LEQUAL);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		glEnable(GL_MULTISAMPLE);

		// This sets the background color that is used to clear the canvas
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		// This load shaders from disk, we do it once when the program starts up.
		reloadShaders();

		setupGeometry(sphere_mesh);
    }
}

void ExampleApp::onRenderGraphicsScene(const VRGraphicsState &renderState) {
    // This routine is called once per eye/camera.  This is the place to actually
    // draw the scene.
    
	// clear the canvas and other buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Setup the view matrix to set where the camera is located in the scene
    glm::vec3 eye_world = glm::vec3(0, 1.5, 6);
    glm::mat4 view = glm::lookAt(eye_world, glm::vec3(0,0,0), glm::vec3(0,1,0));

	// Setup the projection matrix so that things are rendered in perspective
	GLfloat windowHeight = renderState.index().getValue("FramebufferHeight");
	GLfloat windowWidth = renderState.index().getValue("FramebufferWidth");
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), windowWidth / windowHeight, 0.01f, 100.0f);
	
	// Setup the model matrix
	glm::mat4 model = glm::mat4(1.0);
    
	// Tell opengl we want to use this specific shader.
	_shader.use();
	
	_shader.setUniform("view_mat", view);
	_shader.setUniform("projection_mat", projection);
	
	_shader.setUniform("model_mat", model);
	_shader.setUniform("normal_mat", mat3(transpose(inverse(model))));
	_shader.setUniform("eye_world", eye_world);
    
	sphere_mesh->draw(_shader);



}




void ExampleApp::reloadShaders()
{
	_shader.compileShader("texture.vert", GLSLShader::VERTEX);
	_shader.compileShader("texture.frag", GLSLShader::FRAGMENT);
	_shader.link();
	_shader.use();
}


void ExampleApp::setupGeometry(std::shared_ptr<basicgraphics::Mesh>& _mesh) {

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
			vert.texCoord0 = glm::vec2(-(float)j / (float)SLICES + 0.5, -(float)i / (float)STACKS);
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

	int width = 256;
	int height = 256;
	int totalPixels = width * height;
	//TODOmake this an array of bytes
	unsigned char colors[262144];

	for (int i = 0; i < 262144; i+=4) {
		fillByteInByteArray(colors, i, 60, 60, 60, 255);
	}

	////compute the number of opaque pixels = nr of hair strands
	int nrStrands = (int)(0.5f * totalPixels);

	////fill texture with opaque pixels
	for (int i = 0; i < nrStrands; i++)
	{
		int x, y;

		x = rand() % height;
		y = rand() % width;

		fillByteInByteArray(colors, (x * width + y)*4, 218, 165, 32, 255);
	}


	////set the pixels on the texture.

	
	//coulumn major order
	std::shared_ptr<Texture> tex = Texture::createFromMemory("testName", colors, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8, GL_TEXTURE_2D, width, height, 1);
	//
	//
	tex->save2D("D:\\Code\\465\\465-fur-simulation\\resources\\grey2.png");
	textures.push_back(tex);


	


	_mesh.reset(new Mesh(textures, GL_TRIANGLE_STRIP, GL_STATIC_DRAW,
		cpuVertexByteSize, cpuIndexByteSize, 0, cpuVertexArray,
		cpuIndexArray.size(), cpuIndexByteSize, &cpuIndexArray[0]));
}

glm::vec3 ExampleApp::getPosition(double latitude, double longitude) {

	// Latitude and longitude should already be in radians
	return vec3(cos(latitude) * cos(longitude), -sin(latitude), cos(latitude) * sin(longitude));
}

void ExampleApp::fillByteInByteArray(unsigned char* bytes, int index, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
	unsigned char mod = 1;
	
	*(bytes + mod * index) = r;
	*(bytes + mod + mod*index) = g;
	*(bytes + 2*mod + mod*index) = b;
	*(bytes + 3*mod + mod*index) = a;
}



//basicgraphics::Texture ExampleApp::FillFurTexture(basicgraphics::Texture furTexture, float density)
//{
//
//	//read the width and height of the texture
//	int width = furTexture.getWidth();
//	int height = furTexture.getHeight();
//	const int totalPixels = width * height;
//
//	//an array to hold our pixels
//	std::vector<vec4> colors;
//	for (int i=0; i < totalPixels; i++) {
//		colors.push_back(vec4(0,0,0,0));
//	}
//
//	////compute the number of opaque pixels = nr of hair strands
//	int nrStrands = (int)(density * totalPixels);
//
//	////fill texture with opaque pixels
//	for (int i = 0; i < nrStrands; i++)
//	{
//		int x, y;
//		//random position on the texture
//		x = rand() % height;
//		y = rand() % width;
//		//put color (which has an alpha value of 255, i.e. opaque)
//		colors[x * width + y] = vec4(0.75f, 0.75f, 0, 1);
//	}
//
//	////set the pixels on the texture.
//	furTexture.update(&colors, GL_SRGB8_ALPHA8, GL_SRGB8_ALPHA8);
//	return furTexture;
//}