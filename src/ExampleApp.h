#ifndef EXAMPLEAPP_H
#define EXAMPLEAPP_H

#include <api/MinVR.h>
using namespace MinVR;

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "TurntableManipulator.h"

#ifdef _WIN32
#include "GL/glew.h"
#include "GL/wglew.h"
#elif (!defined(__APPLE__))
#include "GL/glxew.h"
#endif

// OpenGL Headers
#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#define GL_GLEXT_PROTOTYPES
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#endif

#include <BasicGraphics.h>
using namespace basicgraphics;

class ExampleApp : public VRApp {
public:
    
    /** The constructor passes argc, argv, and a MinVR config file on to VRApp.
     */
	ExampleApp(int argc, char** argv);
    virtual ~ExampleApp();

    
    /** USER INTERFACE CALLBACKS **/
    virtual void onAnalogChange(const VRAnalogEvent &state);
    virtual void onButtonDown(const VRButtonEvent &state);
    virtual void onButtonUp(const VRButtonEvent &state);
	virtual void onCursorMove(const VRCursorEvent &state);
    
    
    /** RENDERING CALLBACKS **/
    virtual void onRenderGraphicsScene(const VRGraphicsState& state);
    virtual void onRenderGraphicsContext(const VRGraphicsState& state);
    
private:

	double _lastTime;
	double _curFrameTime;

    
    float furCoverage;
    float maxHairLength;

    glm::vec2 lastMousePos;
    
    virtual void reloadShaders();
    basicgraphics::GLSLProgram _shader;

    std::unique_ptr<Model> _modelMesh; // for the bunny
    std::shared_ptr<TurntableManipulator> _turntable;

    std::shared_ptr<Texture> tex;


    bool drawingModel;

    glm::mat4 rotation;


    void pushFurTex();

    // Given latitude and longitude, calculate 3D position
    glm::vec3 getPosition(double latitude, double longitude);
    
   
    void fillByteInByteArray(unsigned char* bytes, int index, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void furLengthLoop();

    bool checkNeighbors(unsigned char* bytes, int x, int y, int width);
    //basicgraphics::Texture FillFurTexture(basicgraphics::Texture furTexture, float density);
};


#endif //EXAMPLEAPP_H
