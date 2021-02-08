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
#include <random>

using namespace std;

// Scene toggles
bool togglePlayClip = false;
bool toggleIK = false;
bool toggleFocusModel = false;


/* ---------- GUI & RENDERING ---------- */

void PlaceAxis();
void DrawMesh();

void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength);
void DrawLine(float x1, float y1, float z1, float x2, float y2, float z2, GLfloat edgeLength);

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1280
const double PI = 3.141592653589793238463;    //value of pi
std::vector<float> HierarchicalOffset(int jIndex);


// Initial camera rotation
GLfloat rotationX = 20.0f;
GLfloat rotationY = -45.0f;

// Screen constants
GLfloat halfScreenWidth = SCREEN_WIDTH / 2;
GLfloat halfScreenHeight = SCREEN_HEIGHT / 2;
int scale = 100;                            // Factor to scale the OBJ model up by

float sceneX = 0;
float sceneY = 0;
float sceneZ = 0;

// When a new model is loaded or a change in the heirarchy is made this switch will flip to ensue that the entire heirarchy is updated.  
bool loadSwitch = false;
bool enableAxis = true;

// Vertex Color
std::vector<float> lineColour
{
    0, 255, 0,
};

std::vector<float> cubeColour
{
    0, 255, 0,
};


int currentFrame = 0; // Stores current frame index
int lastFrame = -1;   // Set forceFullRefresh to true to refresh the entire hierarchy despite the frame staying the same

std::vector<float> rootPos = { 0,0,0 };





/* ---------- FILE SYSTEM ---------- */

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
int GetJointIndex(std::string name);
int LoadOBJ(std::string objFile);
int SaveOBJ(std::string objFile);
void Clear();
int CreateOBJ(std::string objFile);
void OffsetOBJ(float x, float y, float z);

struct vec3 {
    float x = 0;
    float y = 0;
    float z = 0;
};

struct vElement {
    vec3 position;

    vec3 velocity; // velocity
    vec3 force; // force
    float mass = .25;

    bool fixed = false;
    std::set<int> fIndex;       // index of all connecting faces to this point
    std::list<float> fDistance;    // base distance the vertex is from the connecting vertex
};

// f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
struct fElement {
    float v1 = 0;
    float vt1 = 0;
    float vn1 = 0;

    float v2 = 0;
    float vt2 = 0;
    float vn2 = 0;

    float v3 = 0;
    float vt3 = 0;
    float vn3 = 0;
};

std::list<fElement> fList;
std::list<vElement> vList;
std::list<vElement> vnList;
std::list<vElement> vtList;

int keyCooldown = 0;
int keyCooldownMax = 5;
bool fileLoaded = false;

std::string fFile; // Mesh file
std::string fFilename = "Untitled_Mesh"; // Mesh Name






/* ---------- SIMULATION SCENES ---------- */

// Creates a horizontal piece of cloth
void CreateHorizontalSheet(std::string objFile, int x, int y);

void SS1(std::string objFile);  // SS1 (cloth falls onto a sphere)
void SS2(std::string objFile);  // SS2 (cloth with fixed corners)
void SS1R(std::string objFile); // SS1 /w rotation
void SS2W(std::string objFile); // SS2 /w wind

void SimFixCorners(std::string objFile);
void SimFixSides(std::string objFile, int sides);




/* ---------- PHYSICS CALCULATION ---------- */

void SimulateNextFrame();           // Simulates physics for the next frame
void PhysicsSpring(int pInde);
vElement PhysicsPoint(vElement v);  // Calculates physical forces for a single vertex
vElement PhysicsApply(vElement v);  // Applies physical forces to each vertex

vElement PhysicsBounds(vElement v); // calculate particle vector if particle hits a boundary - return new vector
vec3 LogVertexDistances(vec3 v);    // Find and store the initial distance between every point 
float VertexDistance(vec3 p1, vec3 p2); // Distance between two vertices in 3D space
vec3 Normalize(vec3 v);             // Normalizes a vector

float damp = 0.001;
float timeStep = 0.001;
float groundHeight = -0.5;
int clothH = 11;
int clothW = 11;

// k is a very important constant; if too low, the cloth will sag unrealistically:
float k = 40; // srping constant

struct Gravity {
    float x = 0;
    float y = -9.81;
    float z = 0;
    
    bool enable  = true;
};

struct Wind {
    float x = -10;
    float y = 0;
    float z = -15;

    bool enable  = false;   
    bool realism = false;   // randomizes wind force on vertices to simulate more realistic wind
};

struct Ground {
    float x = 0;
    float y = -1;
    float z = 0;

    float friction = 0.005;
    bool enable  = true;
};

Gravity gravity;
Ground ground;
Wind wind;

// Sphere constants
struct Sphere {
    vec3 position;      // position of the sphere
    vec3 rotation;      // speed at which the sphere rotates
    bool rotate = true;
    bool enable = true;
    float radius = 2;

    // Initializers
    float posX = 0;     // position
    float posY = 0;
    float posZ = 0;
    
    float rotX = 0;     // rotation
    float rotY = 20;  // (0.02 - 0.04 for "sane" speeds)
    float rotZ = 0; 

    // Divide the ball horizontally and vertically into a 50*50 grid
    const int ySeg = 10;
    const int xSeg = 10;
};
Sphere sphere;

// Sphere rotation matrix
vector<float> mY = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

// Stores a list of faces on the sphere
struct SphereVertices {
    vec3 position;
    int i1;
    int i2;
    int i3;
    int i4;
    int i5;
    int i6;
};
std::list<SphereVertices> sphereVertices;






/* ---------- MATRIX MANIPULATION ---------- */

void RotateSphere();
std::vector<float> MultMatrix(std::vector<float> m0, std::vector<float> m1);
vec3 RotateVertex(vec3 v);