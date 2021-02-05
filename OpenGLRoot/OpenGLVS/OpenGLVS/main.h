#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#include <set>
#include <vector>
#include <conio.h>
#include <sstream>
#include <algorithm>
#include <string>
#include <map>

using namespace std;

// Scene toggles
bool togglePlayClip = false;
bool toggleIK = false;
bool toggleFocusModel = false;


/* ---------- GUI & RENDERING ---------- */

void PlaceAxis();
void DrawMesh();

void DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength );
void DrawLine(float x1, float y1, float z1, float x2, float y2, float z2, GLfloat edgeLength );

#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 1280
const double PI = 3.141592653589793238463;    //value of pi

std::vector<float> HierarchicalOffset(int jIndex);

// Initial camera rotation
GLfloat rotationX = 20.0f;
GLfloat rotationY = -45.0f;

// Screen constants
GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;
int scale = 100;                         // Factor to scale the OBJ model up by
int scalePosFactor = .25;
int scalePos = scale*scalePosFactor;    // Factor to scale movements in the OBJ by

float sceneX = 0;
float sceneY = 0;
float sceneZ = 0;
bool autoIK = true;


// When a new model is loaded or a change in the heirarchy is made
// this switch will flip to ensue that the entire heirarchy is updated.  
bool loadSwitch = false;

// Vertex Color

std::vector<float> lineColour
{
    0, 255, 0,
};

std::vector<float> cubeColour
{
    0, 255, 0,
};


int currentFrame=0;             // Stores current frame index
int lastFrame=-1;               // Set forceFullRefresh to true to refresh the entire hierarchy despite the frame staying the same

std::vector<float> rootPos = {0,0,0};





/* ---------- FILE SYSTEM ---------- */

void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods );
int GetJointIndex(std::string name);
int LoadOBJ(std::string objFile);
int SaveOBJ(std::string objFile);
void Clear();
int CreateOBJ(std::string objFile);

struct velocity {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct force {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct vElement {
    float x = 0;
    float y = 0;
    float z = 0;

    velocity v; // velocity
    velocity f; // force
    float mass = 1;

    bool fixed = false;
    std::set<int> fIndex;       // index of all connecting faces to this point
    std::list<float> fDistance;    // base distance the vertex is from the connecting vertex
};

// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
struct fElement{
    float v1  = 0;
    float vt1 = 0;
    float vn1 = 0;

    float v2  = 0;
    float vt2 = 0;
    float vn2 = 0;

    float v3  = 0;
    float vt3 = 0;
    float vn3 = 0;
};

std::list<fElement> fList;
std::list<vElement> vList;
std::list<vElement> vListCopy;
std::list<vElement> vnList;
std::list<vElement> vtList;

int keyCooldown = 0;
int keyCooldownMax = 5;
bool fileLoaded = false;

std::string fFile; // Mesh file
std::string fFilename = "Untitled_Mesh"; // Mesh Name




/* ---------- PHYSICS SIMULATION ---------- */

void SimulateNextFrame();
vElement PhysicsPoint(vElement v);
void PhysicsSpring(int pInde);
vElement PhysicsApply(vElement v);
vElement PhysicsBounds(vElement v);
vElement LogVertexDistances(vElement v);

float friction     = 0.2;
bool enableGravity = true;
float damp         = 0.00099;
float timeStep     = 0.0001f;

// k is a very important constant; if too low, the cloth will sag unrealistically:
float k = 50; // srping constant


struct Gravity {
    float x = 0;
    float y = -9.81;
    float z = 0;
};

Gravity gravity;


// float springTension = 0.05;
// float springStrength = 0.0001;
// float gravity = -0.000981;
// float gravity = -0.000981 *.5;