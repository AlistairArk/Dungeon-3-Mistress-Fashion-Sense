#define _CRT_SECURE_NO_WARNINGS
#include "Cloth.h"
bool debug = false;
bool demoMode = true;


int main(void)
{
    GLFWwindow* window;


    // ========== WINDOW INITIALIZATION ==========
    if (!glfwInit())
    {
        return -1;
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon 3", NULL, NULL);

    glfwSetKeyCallback(window, keyCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glViewport(0.0f, 0.0f, screenWidth, screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1000, 10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    // ========== PARAMETER INITIALIZATION ==========

    // Sphere
    sphere.position.x = sphere.posX;
    sphere.position.y = sphere.posY;
    sphere.position.z = sphere.posZ;

    sphere.rotation.x = sphere.rotX;
    sphere.rotation.y = sphere.rotY;
    sphere.rotation.z = sphere.rotZ;

    // Generate sphere of size
    for (int y=0;y<=sphere.ySeg;y++)
    {
        for (int x=0;x<=sphere.xSeg;x++)
        {
            // generate sphere verticies
            SphereVertices s;
            float xSegment = (float)x / (float)sphere.xSeg;
            float ySegment = (float)y / (float)sphere.ySeg;
            float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = std::cos(ySegment * PI);
            float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            s.position.x = xPos * scale * sphere.radius;
            s.position.y = yPos * scale * sphere.radius;
            s.position.z = zPos * scale * sphere.radius;

            // calculate sphere faces
            s.i1 = (y * (sphere.xSeg + 1) + x);
            s.i2 = ((y + 1) * (sphere.xSeg + 1) + x);
            s.i3 = ((y + 1) * (sphere.xSeg + 1) + x+1);
            s.i4 = (y* (sphere.xSeg + 1) + x);
            s.i5 = ((y + 1) * (sphere.xSeg + 1) + x + 1);
            s.i6 = (y * (sphere.xSeg + 1) + x + 1);
            sphereVertices.push_back(s);
        }
    }

    // Start with scene playing
    togglePlayClip = true;
    loadSwitch = true;


    // ========== MAIN LOOP ==========

    while (!glfwWindowShouldClose(window))
    {

        if (keyCooldown > 0) keyCooldown--;

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1.0, 1.0, 1.0, 1.0);

        // RENDER GL          
        glPushMatrix();

            // Control the position of the scene itself
            glTranslatef(halfScreenWidth, halfScreenHeight, -500);
            glRotatef(rotationX, 1, 0, 0);
            glRotatef(rotationY, 0, 1, 0);
            glTranslatef(-halfScreenWidth, -halfScreenHeight, 500);

            if (toggleFocusModel) glTranslatef(-(rootPos[0] * scale), -(rootPos[1] * scale), -(rootPos[2] * scale));

            // Simulate physics for the next frame
            if (togglePlayClip) SimulateNextFrame();

            // Draw Axis to the screen
            PlaceAxis();

            // Draw sphere to the screen
            if (sphere.enable){

                // Creating a iterator pointing to start of set
                std::list<SphereVertices>::iterator it = sphereVertices.begin();

                lineColour = { 0.7,0.7, 0.7 };
                while (it != sphereVertices.end()) { // Iterate till the end of set

                    vec3 i1 = (*std::next(sphereVertices.begin(), (it->i1))).position;
                    vec3 i2 = (*std::next(sphereVertices.begin(), (it->i2))).position;
                    vec3 i3 = (*std::next(sphereVertices.begin(), (it->i3))).position;
                    vec3 i4 = (*std::next(sphereVertices.begin(), (it->i4))).position;
                    vec3 i5 = (*std::next(sphereVertices.begin(), (it->i5))).position;
                    vec3 i6 = (*std::next(sphereVertices.begin(), (it->i6))).position;

                    if (!ground.enable || i1.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i1.x, i1.y, i1.z, 50);
                    if (!ground.enable || i2.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i2.x, i2.y, i2.z, 50);
                    if (!ground.enable || i3.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i3.x, i3.y, i3.z, 50);
                    if (!ground.enable || i4.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i4.x, i4.y, i4.z, 50);
                    if (!ground.enable || i5.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i5.x, i5.y, i5.z, 50);
                    if (!ground.enable || i6.y > ground.y * scale) DrawLine(it->position.x, it->position.y, it->position.z, i6.x, i6.y, i6.z, 50);

                    it++; //Increment the iterator
                }
            }

            // Draw cloth mesh to the screen
            DrawMesh();


        glPopMatrix();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    const GLfloat rotationSpeed = 1;

    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {

        if (loadSwitch)
        {

            switch (key)
            {

                // ----- Rotate Scene -----
            case GLFW_KEY_UP:
                rotationX -= rotationSpeed;
                break;
            case GLFW_KEY_DOWN:
                rotationX += rotationSpeed;
                break;
            case GLFW_KEY_RIGHT:
                rotationY += rotationSpeed;
                break;
            case GLFW_KEY_LEFT:
                rotationY -= rotationSpeed;
                break;

            case GLFW_KEY_0:
                std::cout<< "Empty Simulation Environment" << std::endl;
                Clear();
                sphere.enable=false;
                wind.enable=false;
                break;

            case GLFW_KEY_1:
                std::cout<< "Simulation Scenario 1" << std::endl;
                SS1("cloth");
                break;

            case GLFW_KEY_2:
                std::cout<< "Simulation Scenario 1+" << std::endl;
                SS1R("cloth");
                break;

            case GLFW_KEY_3:
                std::cout<< "Simulation Scenario 2" << std::endl;
                SS2("cloth");
                break;

            case GLFW_KEY_4:
                std::cout<< "Simulation Scenario 2+" << std::endl;
                SS2W("cloth");
                break;

            case GLFW_KEY_5:
                std::cout<< "Simulation Scenario Corner Test" << std::endl;
                SimFixCorners("cloth");
                break;

            case GLFW_KEY_6:
                std::cout<< "Simulation Scenario Side Test - 1" << std::endl;
                SimFixSides("cloth", 1);
                break;
            case GLFW_KEY_7:
                std::cout<< "Simulation Scenario Side Test - 2" << std::endl;
                SimFixSides("cloth", 2);
                break;
            case GLFW_KEY_8:
                std::cout<< "Simulation Scenario Side Test - 3" << std::endl;
                SimFixSides("cloth", 3);
                break;
            case GLFW_KEY_9:
                std::cout<< "Simulation Scenario Side Test - 4" << std::endl;
                SimFixSides("cloth", 4);
                break;

            case GLFW_KEY_G:    // TOGGLE Ground
                ground.enable = !ground.enable;
                break;

            case GLFW_KEY_A:    // TOGGLE Axis
                enableAxis = !enableAxis;
                break;

            case GLFW_KEY_P:    // PAUSE Simulation
                if (togglePlayClip) {
                    std::cout<< "Play Simulation: " << togglePlayClip << std::endl;
                    togglePlayClip = !togglePlayClip;
                }else{
                    std::cout<< "Play Simulation: " << togglePlayClip << std::endl;
                    togglePlayClip = !togglePlayClip;
                }
                break;

                // Toggle sphere rotation.
            case GLFW_KEY_X:
                if (sphere.rotate) {
                    std::cout<< "Sphere Rotation: " << sphere.rotate << std::endl;
                    sphere.rotate = !sphere.rotate;
                }else{
                    std::cout<< "Sphere Rotation: " << sphere.rotate << std::endl;
                    sphere.rotate = !sphere.rotate;
                }
                break;

                // Toggle sphere.
            case GLFW_KEY_C:
                if (sphere.enable) {
                    std::cout<< "Sphere: " << sphere.enable << std::endl;
                    sphere.enable = !sphere.enable;
                }else{
                    std::cout<< "Sphere: " << sphere.enable << std::endl;
                    sphere.enable = !sphere.enable;
                }
                break;

                // Toggle wind         (modelled as a steady force in a fixed direction).
            case GLFW_KEY_W:
                if (wind.enable) {
                    std::cout<< "Wind: " << wind.enable << std::endl;
                    wind.enable = !wind.enable;
                }else{
                    std::cout<< "Wind: " << wind.enable << std::endl;
                    wind.enable = !wind.enable;
                }
                break;

                // Toggle wind realism (introduces some variation to the wind strength on the cloth mesh).
            case GLFW_KEY_R:
                if (wind.realism) {
                    std::cout<< "Wind Realism: " << wind.realism << std::endl;
                    wind.realism = !wind.realism;
                }else{
                    std::cout<< "Wind Realism: " << wind.realism << std::endl;
                    wind.realism = !wind.realism;
                }
                break;

                // Increase/Decrease cloth width
            case GLFW_KEY_KP_DIVIDE:
                clothW++;
                std::cout<< "Cloth Dimensions: (" << clothW << "," << clothH << ")" << std::endl;
                break;

            case GLFW_KEY_KP_MULTIPLY:
                if (clothW>1) clothW--;
                std::cout<< "Cloth Dimensions: (" << clothW << "," << clothH << ")" << std::endl;
                break;

                // Increase/Decrease cloth length
            case GLFW_KEY_KP_SUBTRACT:
                if (clothH>1) clothH--;
                std::cout<< "Cloth Dimensions: (" << clothW << "," << clothH << ")" << std::endl;
                break;

            case GLFW_KEY_KP_ADD:
                clothH++;
                std::cout<< "Cloth Dimensions: (" << clothW << "," << clothH << ")" << std::endl;
                break;


                // Slow down / Speed up sphere
            case GLFW_KEY_COMMA:
                if (sphere.rotation.y>1) sphere.rotation.y-=1;
                std::cout<< "Sphere Speed: " << sphere.rotation.y << std::endl;
                break;
            case GLFW_KEY_PERIOD:
                if (sphere.rotation.y<40) sphere.rotation.y+=1;
                std::cout<< "Sphere Speed: " << sphere.rotation.y << std::endl;
                break; 

                // Slow down / Speed up simulation.
            case GLFW_KEY_RIGHT_BRACKET:
                if (damp < 0.001) damp+=0.00005;
                std::cout<< "Simulation Speed: " << damp << std::endl;
                break;
            case GLFW_KEY_LEFT_BRACKET:
                if (damp > 0.00005) damp-=0.00005;
                std::cout<< "Simulation Speed: " << damp << std::endl;
                break;  

            }
        }
    }



    if (keyCooldown == 0) {
        // GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT

        if (key == 76 && action == GLFW_PRESS) {        // L key press
            while (_kbhit()) _getch(); // Clear key buffer
            std::cout << "\n==== LOAD FILE ====" << std::endl;

            string filename;
            cout << "Enter the name of a .obj file: ";
            cin >> filename;
            filename += ".obj";

            if (FILE* file = fopen(filename.c_str(), "r")) {
                fclose(file);
                std::cout << "Loading File... " << filename << std::endl;
                LoadOBJ(filename);
            }
            else {
                std::cout << "Error: File not found" << std::endl;
                std::cout << "Do not include '.obj' when entering a file name..." << std::endl;
            }
            keyCooldown = keyCooldownMax;
            while (_kbhit()) _getch(); // Clear key buffer

        }
        else if (key == 83 && action == GLFW_PRESS) {  // S key press
            while (_kbhit()) _getch(); // Clear key buffer
            std::cout << "\n==== SAVE FILE ====" << std::endl;

            string filename;
            cout << "Enter name to save the file under: ";
            cin >> filename;
            std::cout << "Saving..." << std::endl;

            fFilename = filename;
            filename += ".obj";
            SaveOBJ(filename);

            while (_kbhit()) _getch(); // Clear key buffer
            keyCooldown = keyCooldownMax;

        }
    }
}











//--------------------------------//
//                                //
//  PHYSICS CALCULATION ROUTEINS  //
//                                //
//--------------------------------//

// calculate particle vector if particle hits a boundary - return new vector
vElement PhysicsBounds(vElement v) {

    // Check for ground colission
    if (v.position.y < ground.y && ground.enable) {
        v.position.y = ground.y;
    }

    if (sphere.enable) {
        float sphereDistance = VertexDistance(sphere.position, v.position);

        // if vertex is inside sphere 
        if (sphereDistance <= sphere.radius){

            // move any vertex inside the sphere to the closest point on the surface of the sphere.
            if (sphere.rotate) v.position = RotateVertex(v.position);
            
            // shunt point to exterior of sphere
            vec3 sphereToPoint;
            sphereToPoint.x = v.position.x - sphere.position.x;
            sphereToPoint.y = v.position.y - sphere.position.y;
            sphereToPoint.z = v.position.z - sphere.position.z;

            // get the sphere to point normal 
            sphereToPoint = Normalize(sphereToPoint);

            // calculate new position on spehere exterior 
            v.position.x = sphereToPoint.x * sphere.radius;
            v.position.y = sphereToPoint.y * sphere.radius;
            v.position.z = sphereToPoint.z * sphere.radius;

            // if sphere is rotating, impart some momentum onto it
            // {
            //     vec3 forceTarget = RotateVertex(v.position);
            //     v.velocity.x -= (v.position.x - forceTarget.x)/damp;
            //     v.velocity.y -= (v.position.y - forceTarget.y)/damp;
            //     v.velocity.z -= (v.position.z - forceTarget.z)/damp;
            // }
        }

    }

    return v;
}

// Distance between two vertices in 3D space
float VertexDistance(vec3 p1, vec3 p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}

// Normalizes a vector
vec3 Normalize(vec3 v) {

    float length = sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));

    v.x = v.x / length;
    v.y = v.y / length;
    v.z = v.z / length;

    return v;
}

// Calculates physical forces for a single vertex
void PhysicsSpring(int pIndex) {

    vec3 springVector;
    vec3 force;

    // Creating a iterator pointing to start of set
    std::set<int>::iterator it = (*std::next(vList.begin(), pIndex)).fIndex.begin();
    int i = -1;
    while (it != (*std::next(vList.begin(), pIndex)).fIndex.end()) { // Iterate till the end of set
        i++;

        vElement p1 = (*std::next(vList.begin(), pIndex));
        vElement p2 = (*std::next(vList.begin(), (*it)));

        springVector.x = p1.position.x - p2.position.x;
        springVector.y = p1.position.y - p2.position.y;
        springVector.z = p1.position.z - p2.position.z;

        float xCurrent = VertexDistance(p1.position, p2.position);
        float xRest = (*std::next(p1.fDistance.begin(), i));
        float distanceFromRest = xCurrent - xRest;

        float hookesValue = -k * distanceFromRest;

        springVector = Normalize(springVector);

        // calculate force
        force.x = (springVector.x * hookesValue);
        force.y = (springVector.y * hookesValue);
        force.z = (springVector.z * hookesValue);

        p1.force.x = (p1.force.x + force.x);
        p1.force.y = (p1.force.y + force.y);
        p1.force.z = (p1.force.z + force.z);

        (*std::next(vList.begin(), pIndex)).force = p1.force;

        (*std::next(vList.begin(), (*it))).force = p2.force;

        it++; //Increment the iterator
    }
}

// Applies physical forces to each vertex
vElement PhysicsApply(vElement p1)
{

    if (!p1.fixed) {

        // if(!particles[i].isStationary()) {
        if (gravity.enable) {
            p1.force.x = (p1.force.x + gravity.x * p1.mass);
            p1.force.y = (p1.force.y + gravity.y * p1.mass);
            p1.force.z = (p1.force.z + gravity.z * p1.mass);
        }

        if (wind.enable) {
            if (wind.realism) {     // Model wind more realistically with random variation
                std::mt19937_64 gen{ std::random_device()() };
                std::uniform_real_distribution<double> dis{ 0.5, 1.0 };

                p1.force.x += wind.x * dis(gen);
                p1.force.y += wind.y * dis(gen);
                p1.force.z += wind.z * dis(gen);
            
            } else {                // Model wind as a steady force in a fixed direction
                p1.force.x += wind.x;
                p1.force.y += wind.y;
                p1.force.z += wind.z;
            }
        }

        p1.velocity.x = p1.velocity.x + (p1.force.x / (p1.mass * timeStep));
        p1.velocity.y = p1.velocity.y + (p1.force.y / (p1.mass * timeStep));
        p1.velocity.z = p1.velocity.z + (p1.force.z / (p1.mass * timeStep));

        p1.velocity.x = p1.velocity.x * damp;
        p1.velocity.y = p1.velocity.y * damp;
        p1.velocity.z = p1.velocity.z * damp;

        p1.position.x = p1.position.x + (p1.velocity.x * timeStep);
        p1.position.y = p1.position.y + (p1.velocity.y * timeStep);
        p1.position.z = p1.position.z + (p1.velocity.z * timeStep);

        p1 = PhysicsBounds(p1);

        p1.force.x = 0;
        p1.force.y = 0;
        p1.force.z = 0;

    }

    return p1;
}

// Simulates physics for the next frame
void SimulateNextFrame()
{

    // Rotate sphere if rotation and sphere are enabled
    if (sphere.enable && sphere.rotate) RotateSphere();

    int i;

    std::list<vElement>::iterator it;
    i = -1;
    for (it = vList.begin(); it != vList.end(); it++) {
        i++;

        PhysicsSpring(i);
    }


    i = -1;
    for (it = vList.begin(); it != vList.end(); it++) {
        i++;

        (*std::next(vList.begin(), i)) = PhysicsApply((*std::next(vList.begin(), i)));
    }
}

// Find and store the initial distance between every point 
vElement LogVertexDistances(vElement v)
{

    std::set<int>::iterator it = v.fIndex.begin();  // Creating a iterator pointing to start of set
    while (it != v.fIndex.end()) {                  // Iterate till the end of set

        // log the distance between the two vertices
        v.fDistance.push_back(VertexDistance(v.position, (*std::next(vList.begin(), (*it))).position));

        it++;                                       //Increment the iterator
    }

    return v;
}


// Generate a linked list of adjacent nodes for each vertex in the mesh
void GenerateLinkList() {

    int i = -1;
    std::list<vElement>::iterator it;

    // Iterate over each vertex
    for (it = vList.begin(); it != vList.end(); it++) {

        i++;

        // Find all elements this vertex links to
        std::list<fElement>::iterator it2;

        for (it2 = fList.begin(); it2 != fList.end(); it2++) {

            if (it2->v1 == i) {
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v2);
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v3);
            }

            if (it2->v2 == i) {
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v1);
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v3);
            }

            if (it2->v3 == i) {
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v1);
                (*std::next(vList.begin(), i)).fIndex.insert(it2->v2);
            }
        }


        // log vertex distances
        (*std::next(vList.begin(), i)) = LogVertexDistances((*std::next(vList.begin(), i)));
    }
}


// Draws a line between two vertices 
void DrawLine(float x1, float y1, float z1, float x2, float y2, float z2, GLfloat edgeLength)
{
    // GLfloat halfSideLength = edgeLength * 0.5f;
    GLfloat vertices[] =
    {
        (x1 + halfScreenWidth), 
        (y1 + halfScreenHeight),
        (z1 + -500),

        (x2 + halfScreenWidth),
        (y2 + halfScreenHeight),
        (z2 + -500)
    };

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(lineColour[0], lineColour[1], lineColour[2]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_LINES, 0, 2);

    glDisableClientState(GL_VERTEX_ARRAY);
}



void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength)
{

    centerPosX = centerPosX + halfScreenWidth;
    centerPosY = centerPosY + halfScreenHeight;
    centerPosZ = centerPosZ + -500;

    GLfloat halfSideLength = edgeLength * 0.5f;

    GLfloat vertices[] =
    {
        // front face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // back face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom left

        // left face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // right face
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // bottom left

        // top face
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // bottom left

        // top face
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // top left
        centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // top right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // bottom right
        centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // bottom left
    };




    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(cubeColour[0], cubeColour[1], cubeColour[2]);
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glDrawArrays(GL_QUADS, 0, 24);

    glDisableClientState(GL_VERTEX_ARRAY);

}











//----------------------------//
//                            //
//  FILE MANAGEMENT ROUTEINS  //
//                            //
//----------------------------//

// clears the current lists before re-loading
void Clear()
{
    // Reset variables to defaults before loading new obj
    fList.clear();
    vList.clear();
    vnList.clear();
    vtList.clear();

}

// loads an object to the scene
int LoadOBJ(std::string file)
{

    Clear();

    ifstream inputFile(file.c_str(), ios::in);
    if (!inputFile.is_open()) {
        return -1;
    }
    fFile = file;
    char line[256];

    int textureCounter = 0;
    int normalCounter = 0;

    while (inputFile.getline(line, 256)) {
        // the first letter of every line tells us what to do with the rest 
        // of the line

        vElement v;
        fElement f;

        switch (line[0]) {
            char dummy[10];

        case 'o':
            char name[256];
            sscanf(line, "%s %s", dummy, name);

            fFile = string(name);
            break;

        case 'v': // new vertex of some type

            // fill the values with the data in the line
            sscanf(line, "%s %f %f %f", dummy, &(v.position.x), &(v.position.y), &(v.position.z));

            switch (line[1]) { // switch on second char to determine which kind

            case ' ': // plain vertex
                vList.push_back(v);
                break;

            case 't': // texture vertex
                vtList.push_back(v);
                break;

            case 'n': // vertex normal
                vnList.push_back(v);
                break;
            }
            break;

        case 'f': // a new face

            int lPos, rPos;
            string strLine = string(line);

            vector<int> vertInd;
            lPos = strLine.find(" ");
            while (lPos != string::npos) {
                string curWord;

                lPos++; // get the lPos past the space

                rPos = strLine.find(" ", lPos);
                if (rPos != string::npos)
                    curWord = strLine.substr(lPos, rPos - (lPos));
                else
                    curWord = strLine.substr(lPos, strLine.length());

                // ignore any extra spaces
                if (curWord != " " && !curWord.empty()) {
                    int vertexIndex;
                    int posSlash1, posSlash2;
                    posSlash1 = curWord.find("/");
                    posSlash2 = curWord.find("/", posSlash1 + 1);
                    sscanf(curWord.substr(0, posSlash1).c_str(), "%d", &vertexIndex);
                    vertInd.push_back(vertexIndex);
                }
                lPos = strLine.find(" ", lPos);
            }

            f.v1 = vertInd[0] - 1;
            f.v2 = vertInd[1] - 1;
            f.v3 = vertInd[2] - 1;

            fList.push_back(f);
            break;
        }
    }
    inputFile.close();

    loadSwitch = true;

    GenerateLinkList();

    return 0;
}

// save the current frame to an object file. 
int SaveOBJ(std::string objFile)
{

    ofstream output(objFile);
    if (!output.is_open())
        return -1;

    output << "o " << fFilename << std::endl;

    while (vList.size() > 0)
    {
        vElement v = vList.front();
        vList.pop_front();
        output << "v " << v.position.x << " " << v.position.y << " " << v.position.z << std::endl;
    }

    while (vnList.size() > 0)
    {
        vElement v = vnList.front();
        vnList.pop_front();
        output << "vn " << v.position.x << " " << v.position.y << " " << v.position.z << std::endl;
    }

    while (vtList.size() > 0)
    {
        vElement v = vtList.front();
        vtList.pop_front();
        output << "vt " << v.position.x << " " << v.position.y << " " << v.position.z << std::endl;
    }

    while (fList.size() > 0)
    {
        fElement f = fList.front();
        fList.pop_front();
        f.v1 += 1;
        f.v2 += 1;
        f.v3 += 1;
        output << "f " << f.v1 << "/" << f.vt1 << "/" << f.vn1 << " "
            << f.v2 << "/" << f.vt2 << "/" << f.vn2 << " "
            << f.v3 << "/" << f.vt3 << "/" << f.vn3 << std::endl;
    }

    output.close();

    // Re-load file to fill lists again
    LoadOBJ(objFile);

    return 0;
}

// Set the initial offset of the object in the scene
void OffsetOBJ(float x, float y, float z) {

    int i = -1;
    std::list<vElement>::iterator it;

    for (it = vList.begin(); it != vList.end(); it++) {

        i++;
        (*std::next(vList.begin(), i)).position.x += x;
        (*std::next(vList.begin(), i)).position.y += y;
        (*std::next(vList.begin(), i)).position.z += z;

    }
}










//--------------------------------//
//                                //
//  SIMULATION CREATION ROUTEINS  //
//                                //
//--------------------------------//

void SS1(std::string objFile)
{
    /*
        Simulation Scenario 1 (SS1): a square piece of cloth floating horizontally in the air at first, which
        then free-falls onto a sphere on the ground.
    */

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    wind.enable = false;
    sphere.enable = true;
    sphere.rotate = false;
    ground.y = -2;

}

void SS2(std::string objFile)
{
    /*
        Simulation Scenario 2 (SS2): a square piece of cloth floating horizontally in the air first, then
        free-falling with two adjacent corners fixed in the space.
    */

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    // fix two adjacent corners
    (*std::next(vList.begin(), 0)).fixed = true;
    (*std::next(vList.begin(), clothW-1)).fixed = true;


    wind.enable = false;
    sphere.enable = false;
    ground.y = -2;


}

void SS1R(std::string objFile)
{
    /*
        Simulation Scenario 1 (SS1): a square piece of cloth floating horizontally in the air at first, which
        then free-falls onto a sphere on the ground.

        SS1 with the sphere rotating in-place round the Y axis (up-axis) and friction between the sphere
        and the cloth.
    */

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    wind.enable = false;
    sphere.enable = true;
    sphere.rotate = true;
    ground.y = -2;


}

void SS2W(std::string objFile)
{
    /*
        Simulation Scenario 2 (SS2): a square piece of cloth floating horizontally in the air first, then
        free-falling with two adjacent corners fixed in the space.

        SS2 with wind blowing.
    */

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    // fix two adjacent corners
    (*std::next(vList.begin(), 0)).fixed = true;
    (*std::next(vList.begin(), clothW-1)).fixed = true;


    wind.enable = true;
    wind.x = -5;
    wind.y = 0;
    wind.z = -5;
    sphere.enable = false;
    ground.y = -2;
}

void SimFixCorners(std::string objFile)
{
    // A square piece of cloth floating horizontally in the air first, then free-falling with
    // four corners fixed in the space.

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    // fix four adjacent corners
    (*std::next(vList.begin(), 0)).fixed = true;
    (*std::next(vList.begin(), clothW-1)).fixed = true;

    (*std::next(vList.begin(), (clothH*clothW)-clothW)).fixed = true;
    (*std::next(vList.begin(), (clothH*clothW)-1)).fixed = true;

    wind.enable = false;
    sphere.enable = false;
    ground.y = -2;

}

void SimFixSides(std::string objFile, int sides)
{
    // A square piece of cloth floating horizontally in the air first, then free-falling with
    // all edges fixed in the space.

    CreateHorizontalSheet(objFile, clothW, clothH);
    SaveOBJ(objFile + ".obj");
    LoadOBJ(objFile + ".obj");

    int i = 0;
    while (i != clothW) {
        if (sides==4 || sides==3 || sides==2 || sides==1) (*std::next(vList.begin(), i)).fixed = true;
        if (sides==4 || sides==3) (*std::next(vList.begin(), i+(clothH*clothW)-clothW)).fixed = true;
        i++;
    }

    i = 0;
    while (i != clothH) {
        if (sides==4 || sides==3 || sides==2) (*std::next(vList.begin(), (i*clothW))).fixed = true;
        if (sides==4) (*std::next(vList.begin(), (i*clothW)+clothW-1)).fixed = true;
        i++;
    }
}

// Creates a horizontal piece of cloth
void CreateHorizontalSheet(std::string objFile, int sizeX, int sizeY)
{
    Clear();

    /* ---------- CREATE CLOTH ---------- */
    int offsetX = -(sizeX-1)*.5;
    int offsetY = -(sizeY-1)*.5;

    float scale = 0.5;

    // Define the verticies of the cloth
    int y = 0;
    while (y != sizeY) {

        int x = 0;

        while (x != sizeX) {

            vElement v;
            v.position.x = (x + offsetX) * scale;
            v.position.y = 0;
            v.position.z = (y + offsetY)*scale;
            vList.push_back(v);

            x++;
        }
        y++;
    }


    // Define the faces between the verticies
    y = 0;
    while (y != sizeY - 1) {

        int x = 0;
        // std::cout<< "" <<std::endl;
        while (x != sizeX - 1) {

            int i = (sizeX * y) + x + 1;

            i -= 1;

            fElement f;
            f.v1 = i;
            f.v2 = i + 1;
            f.v3 = i + sizeX;
            fList.push_back(f);
            f.v1 = i + 1;
            f.v2 = i + sizeX;
            f.v3 = i + sizeX + 1;
            fList.push_back(f);
            f.v1 = i + sizeX;
            f.v2 = i + sizeX + 1;
            f.v3 = i;
            fList.push_back(f);
            f.v1 = i + sizeX + 1;
            f.v2 = i;
            f.v3 = i + 1;
            fList.push_back(f);

            x++;
        }
        y++;
    }
    OffsetOBJ(0, 5, 0);      // offset cloth
}











//--------------------------------//
//                                //
//  MATRIX MANIPULATION ROUTEINS  //
//                                //
//--------------------------------//

// Multiply 2 matrices together and return the result
std::vector<float> MultMatrix(std::vector<float> m0, std::vector<float> m1)
{
    std::vector<float> M
    {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };


    M =
    {
        (m0[0] * m1[0] + m0[1] * m1[4] + m0[2] * m1[8] + m0[3] * m1[12]), 
        (m0[0] * m1[1] + m0[1] * m1[5] + m0[2] * m1[9] + m0[3] * m1[13]),
        (m0[0] * m1[2] + m0[1] * m1[6] + m0[2] * m1[10] + m0[3] * m1[14]),
        (m0[0] * m1[3] + m0[1] * m1[7] + m0[2] * m1[11] + m0[3] * m1[15]),

        (m0[4] * m1[0] + m0[5] * m1[4] + m0[6] * m1[8] + m0[7] * m1[12]), 
        (m0[4] * m1[1] + m0[5] * m1[5] + m0[6] * m1[9] + m0[7] * m1[13]),
        (m0[4] * m1[2] + m0[5] * m1[6] + m0[6] * m1[10] + m0[7] * m1[14]),
        (m0[4] * m1[3] + m0[5] * m1[7] + m0[6] * m1[11] + m0[7] * m1[15]),

        (m0[8] * m1[0] + m0[9] * m1[4] + m0[10] * m1[8] + m0[11] * m1[12]), 
        (m0[8] * m1[1] + m0[9] * m1[5] + m0[10] * m1[9] + m0[11] * m1[13]),
        (m0[8] * m1[2] + m0[9] * m1[6] + m0[10] * m1[10] + m0[11] * m1[14]),
        (m0[8] * m1[3] + m0[9] * m1[7] + m0[10] * m1[11] + m0[11] * m1[15]),

        (m0[12] * m1[0] + m0[13] * m1[4] + m0[14] * m1[8] + m0[15] * m1[12]), 
        (m0[12] * m1[1] + m0[13] * m1[5] + m0[14] * m1[9] + m0[15] * m1[13]),
        (m0[12] * m1[2] + m0[13] * m1[6] + m0[14] * m1[10] + m0[15] * m1[14]),
        (m0[12] * m1[3] + m0[13] * m1[7] + m0[14] * m1[11] + m0[15] * m1[15])
    };

    return M;
}



vec3 RotateVertex(vec3 v)
{

    vec3 newV;

    // Calculate Rotation Matrix
    newV.x = v.x*mY[0]  + v.y*mY[1] + v.z*mY[2];
    newV.y = v.x*mY[4]  + v.y*mY[5] + v.z*mY[6];
    newV.z = v.x*mY[8]  + v.y*mY[9] + v.z*mY[10];

    return newV;
}

void RotateSphere()
{
    mY = // Matrix RotateY
    {
      cos(sphere.rotation.y * damp), 0, sin(sphere.rotation.y * damp), 0, 
                0, 1,           0, 0, 
     -sin(sphere.rotation.y * damp), 0, cos(sphere.rotation.y * damp), 0, 
                0, 0,           0, 1, 
    };

    // Creating a iterator pointing to start of set
    std::list<SphereVertices>::iterator it = sphereVertices.begin();

    int i = -1;
    while (it != sphereVertices.end()) { // Iterate till the end of set
        i++;
        (*std::next(sphereVertices.begin(), i)).position = RotateVertex((*std::next(sphereVertices.begin(), i)).position);

        it++; //Increment the iterator
    }
}





//--------------------//
//                    //
//  DISPLAY ROUTEINS  //
//                    //
//--------------------//

// Draws the mesh lists to the scene
void DrawMesh()
{

    // Place Verticies
    for (int i = 0; i < vList.size(); i++)
    {
        vector<float> xyz = { 0, 0, 0 };


        vElement v = *std::next(vList.begin(), i);


        // Scale Up - Add Offset
        xyz[0] = (v.position.x * scale);
        xyz[1] = (v.position.y * scale);
        xyz[2] = (v.position.z * scale);

        cubeColour =
        {
            0,0,255,
            0,0,255,
            0,0,255
        };

        // Draw cube around fixed points
        if (v.fixed) DrawCube(xyz[0], xyz[1], xyz[2], 5);

    }


    // Draw Faces as Wireframe
    for (int i = 0; i < fList.size(); i++)
    {

        fElement f = *std::next(fList.begin(), i);

        lineColour =
        {
            0,0,255,
            0,0,255,
            0,0,255
        };

        vElement v1 = *std::next(vList.begin(), f.v1);
        vElement v2 = *std::next(vList.begin(), f.v2);
        vElement v3 = *std::next(vList.begin(), f.v3);

        // Scale Up - Add Offset
        v1.position.x = (v1.position.x * scale);
        v1.position.y = (v1.position.y * scale);
        v1.position.z = (v1.position.z * scale);

        v2.position.x = (v2.position.x * scale);
        v2.position.y = (v2.position.y * scale);
        v2.position.z = (v2.position.z * scale);

        v3.position.x = (v3.position.x * scale);
        v3.position.y = (v3.position.y * scale);
        v3.position.z = (v3.position.z * scale);

        DrawLine(v1.position.x, v1.position.y, v1.position.z, v2.position.x, v2.position.y, v2.position.z, 50);
        DrawLine(v3.position.x, v3.position.y, v3.position.z, v2.position.x, v2.position.y, v2.position.z, 50);
        DrawLine(v3.position.x, v3.position.y, v3.position.z, v1.position.x, v1.position.y, v1.position.z, 50);

    }
}


// Draws the axis to the scene 
void PlaceAxis()
{
    if (enableAxis){

        // ----- ORIGIN -----
        // Red Line
        lineColour[0] = 255;
        lineColour[1] = 0;
        lineColour[2] = 0;
        DrawLine(0, 0, 0, 100, 0, 0, 50);

        // Green Line
        lineColour[0] = 0;
        lineColour[1] = 255;
        lineColour[2] = 0;
        DrawLine(0, 0, 0, 0, 100, 0, 50);

        // Blue Line
        lineColour[0] = 0;
        lineColour[1] = 0;
        lineColour[2] = 255;
        DrawLine(0, 0, 0, 0, 0, 100, 50);
    }


    // Draw Line from axis in wind direction
    if (wind.enable){

        lineColour = { 0.8, 0.0, 0.8 }; // Light grey color
        DrawLine(   0, 0, 0,
                    wind.x*scale, wind.y*scale,  wind.z*scale, 0);
    }

    if (ground.enable){

        // ----- GRID -----
        lineColour = { 0.7, 0.7, 0.7 }; // Light grey color

        float gridWidth = 1000;
        float cubeSpace = 200;
        for (int x = -5; x < 6; x++)
        {
            DrawLine(   (x * cubeSpace) + ground.x, ground.y*scale, -gridWidth + ground.z,
                        (x * cubeSpace) + ground.x, ground.y*scale,  gridWidth + ground.z, 0);

            DrawLine(  -gridWidth + ground.x, ground.y*scale, (x * cubeSpace) + ground.z,
                        gridWidth + ground.x, ground.y*scale, (x * cubeSpace) + ground.z, 0);
        }
    }
}



