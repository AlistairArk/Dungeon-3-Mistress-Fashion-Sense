#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
bool debug = false;






// calculate particle vector if particle hits a boundary - return new vector
vElement PhysicsBounds(vElement v) {

    if (v.y < 0) {
        v.y = 0;
        v.v.y = -v.v.y*friction;
    }
    
    return v;
}




float VertexDistance(vElement p1, vElement p2){
    return sqrt(pow(p1.x-p2.x, 2)+pow(p1.y-p2.y, 2));
}


vElement Normalize(vElement v){

    float length = sqrt ( pow(v.x,2) + pow(v.y,2) + pow(v.z,2) );

    v.x = v.x / length; 
    v.y = v.y / length; 
    v.z = v.z / length; 

    return v; 
}




void PhysicsSpring(int pIndex) {

    if (debug) std::cout<< "\nPhysicsSpring index: " << pIndex << "\n---------------------" << std::endl;



    // vElement p1 = (*std::next(vList.begin(), pIndex));
    // vElement p2;
    vElement springVector;
    vElement force;


    // Creating a iterator pointing to start of set
    std::set<int>::iterator it = (*std::next(vList.begin(), pIndex)).fIndex.begin();
    int i=-1;
    while (it != (*std::next(vList.begin(), pIndex)).fIndex.end()) { // Iterate till the end of set
        i++;
        if (debug && i>0) std::cout << "------------------------------------------" << std::endl;


        vElement p1 = (*std::next(vList.begin(), pIndex));
        vElement p2 = (*std::next(vList.begin(), (*it)));
        if (debug) std::cout << "\tP1 - Pos         (" << (*std::next(vList.begin(), pIndex)).x << ", " << (*std::next(vList.begin(), pIndex)).y << ", " << (*std::next(vList.begin(), pIndex)).z << ")" << std::endl;
        if (debug) std::cout << "\tP2 - Pos         (" << (*std::next(vList.begin(), (*it))).x << ", " << (*std::next(vList.begin(), (*it))).y << ", " << (*std::next(vList.begin(), (*it))).z << ")" << std::endl;

        // X(current) is just the current length of the spring, and X(rest), the spring's rest length, needs to be stored in each spring structure. 
        springVector.x = p1.x - p2.x;
        springVector.y = p1.y - p2.y;
        springVector.z = p1.z - p2.z;

        float xCurrent  = VertexDistance(p1, p2);
        float xRest     = (*std::next(p1.fDistance.begin(), i));
        float distanceFromRest = xCurrent-xRest;
        if (debug) std::cout << "\tdistanceFromRest: " << distanceFromRest << std::endl;


        float hookesValue = -k * distanceFromRest;
        if (debug) std::cout << "\thookesValue:      " << hookesValue << std::endl;


        springVector = Normalize(springVector);
        if (debug) std::cout << "\tspringVector     (" << springVector.x << ", " << springVector.y << ", " << springVector.z << ")" << std::endl;

        // calculate force
        force.x = ( springVector.x * hookesValue );
        force.y = ( springVector.y * hookesValue );
        force.z = ( springVector.z * hookesValue );
        if (debug) std::cout << "\tforce            (" << force.x << ", " << force.y << ", " << force.z << ")" << std::endl;

        if (debug) std::cout << "\tP1 - PreForce    (" << p1.f.x << ", " << p1.f.y << ", " << p1.f.z << ")" << std::endl;
        p1.f.x = ( p1.f.x + force.x);
        p1.f.y = ( p1.f.y + force.y);
        p1.f.z = ( p1.f.z + force.z);
        if (debug) std::cout << "\tP1 - PostForce   (" << p1.f.x << ", " << p1.f.y << ", " << p1.f.z << ")" << std::endl;
        (*std::next(vList.begin(), pIndex)).f = p1.f;
        if (debug) std::cout << "\tP1 - STOREDForce (" << (*std::next(vList.begin(), pIndex)).f.x << ", " << (*std::next(vList.begin(), pIndex)).f.y << ", " << (*std::next(vList.begin(), pIndex)).f.z << ")" << std::endl;

        p2.f.x = ( p2.f.x - force.x);
        p2.f.y = ( p2.f.y - force.y);
        p2.f.z = ( p2.f.z - force.z);
        (*std::next(vList.begin(), (*it))).f  = p2.f;


        // distanceFromRest  = k*(xCurrent-xRest);
        it++; //Increment the iterator
    }

    if (debug) std::cout<< "\n";

}


vElement PhysicsApply(vElement p1) {

    if (!p1.fixed){
        if (debug) std::cout << "\tGravity         (" << gravity.x << ", " << gravity.y << ", " << gravity.z << ")" << std::endl;

        // if(!particles[i].isStationary()) {
        if(enableGravity) {
            p1.f.x = (p1.f.x + gravity.x * p1.mass);
            p1.f.y = (p1.f.y + gravity.y * p1.mass);
            p1.f.z = (p1.f.z + gravity.z * p1.mass);
        }
        if (debug) std::cout << "\tP1 - Force+Grav (" << p1.f.x << ", " << p1.f.y << ", " << p1.f.z << ")" << std::endl;

        // particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getForce() / (particles[i].getMass() * timeStep)));
        p1.v.x = p1.v.x + ( p1.f.x / (p1.mass*timeStep) );
        p1.v.y = p1.v.y + ( p1.f.y / (p1.mass*timeStep) );
        p1.v.z = p1.v.z + ( p1.f.z / (p1.mass*timeStep) );

        // particles[i].setVelocity(particles[i].getVelocity() * damp);
        p1.v.x = p1.v.x * damp;
        p1.v.y = p1.v.y * damp;
        p1.v.z = p1.v.z * damp;

        // particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
        p1.x = p1.x + (p1.v.x * timeStep);
        p1.y = p1.y + (p1.v.y * timeStep);
        p1.z = p1.z + (p1.v.z * timeStep);

        // particles[i].setForce(Vector(0.0f, 0.0f, 0.0f));
        p1.f.x = 0;
        p1.f.y = 0;
        p1.f.z = 0;        
    }

    return p1;
}






void SimulateNextFrame() {

    std::list<vElement> vListCopy2(vList);
    vListCopy = vListCopy2;

    int i;

    std::list<vElement>::iterator it;
    i = -1;
    for (it = vList.begin(); it != vList.end(); it++) {
        i++;

        PhysicsSpring( i );
        // if (i==0) std::cout << (*std::next(vList.begin(), i)).f.x << "\n";
    }


    i = -1;
    for (it = vList.begin(); it != vList.end(); it++) {
        i++;
        
        if (debug) std::cout<< "\nPhysicsApply index: " << i << "\n---------------------" << std::endl;
        if (debug && i>0) std::cout << "------------------------------------------" << std::endl;

        (*std::next(vList.begin(), i)) = PhysicsApply( (*std::next(vList.begin(), i)) );
        std::cout << " \n";
        // if (i != 0 && i != 4) (*std::next(vList.begin(), i)) = PhysicsApply( (*std::next(vList.begin(), i)) );
        // if (i==0) std::cout << (*std::next(vList.begin(), i)).f.x << "\n";
    }
}




void SetOBJHeight(int height) {


    int i = -1;
    std::list<vElement>::iterator it;
    
    for (it = vList.begin(); it != vList.end(); it++) {
           
        i++;
        (*std::next(vList.begin(), i)).y += height;

        // std::cout << "(" << it->x << ",\t" << it->y << ",\t" << it->z << ") " << std::endl;

    }
}



int CreateOBJ(std::string objFile){

    int offsetX = -2.5;
    int offsetY = 0;

    int sizeX = 5;
    int sizeY = 4;

    int y = 0;
    while (y != sizeY){
        
        int x = 0;
        // std::cout<< "" <<std::endl;
        while (x != sizeX){

            // std::cout<< "v "<< x << " " << -y << " "  << "0.0" <<std::endl;

            vElement v;
            v.x = x+offsetX;
            v.y = -y;
            v.z = 0;
            vList.push_back(v);



            x++;
        }
        y++;
    }



    y = 0;
    while (y != sizeY-1){
        
        int x = 0;
        // std::cout<< "" <<std::endl;
        while (x != sizeX-1){

            int i = (sizeX * y) + x + 1;
            // std::cout<< "f "<< i         << "// " << i+1        << "// "  << i+sizeX    << "//" <<std::endl;
            // std::cout<< "f "<< i+1       << "// " << i+sizeX    << "// "  << i+sizeX+1  << "//" <<std::endl;
            // std::cout<< "f "<< i+sizeX   << "// " << i+sizeX+1  << "// "  << i          << "//" <<std::endl;
            // std::cout<< "f "<< i+sizeX+1 << "// " << i          << "// "  << i+1        << "//"       <<std::endl;
            i-=1;

            fElement f;
            f.v1 = i;
            f.v2 = i+1;
            f.v3 = i+sizeX;
            fList.push_back(f);
            f.v1 = i+1;
            f.v2 = i+sizeX;
            f.v3 = i+sizeX+1;
            fList.push_back(f);
            f.v1 = i+sizeX;
            f.v2 = i+sizeX+1;
            f.v3 = i;
            fList.push_back(f);
            f.v1 = i+sizeX+1;
            f.v2 = i;
            f.v3 = i+1;
            fList.push_back(f);

            x++;
        }
        y++;
    }

/*
vElement v = vList.front();
vList.pop_front();
output << "v " << v.x << " " << v.y << " " << v.z << std::endl;
fElement f = fList.front();
fList.pop_front();
f.v1+=1;
f.v2+=1;
f.v3+=1;



*/
    return 0; 
}


int main( void )
{
    GLFWwindow *window;
    

    // ========== WINDOW INITIALIZATION ==========

    if ( !glfwInit( ) )
    {
        return -1;
    }
    
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Dungeon 3", NULL, NULL );
    
    glfwSetKeyCallback( window, keyCallback );
    glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    if ( !window )
    {
        glfwTerminate( );
        return -1;
    }
    
    glfwMakeContextCurrent( window );
    glViewport( 0.0f, 0.0f, screenWidth, screenHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1000, 10000 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    

    

    // ========== AUTO LOAD TEST ==========

    // LoadOBJ("sphere4x4.obj");
    // LoadOBJ("triangle_groundplane.obj");

    CreateOBJ("cloth");
    SaveOBJ("cloth.obj");
    LoadOBJ("cloth.obj");

    (*std::next(vList.begin(), 0)).fixed = true;
    (*std::next(vList.begin(), 4)).fixed = true;

    SetOBJHeight(5);
    togglePlayClip = true;

    // // simulate 1 frame
    SimulateNextFrame();
    int itLimit = 1;




    // ========== MAIN LOOP ==========

    while ( !glfwWindowShouldClose( window ) )
    {
        
        if (keyCooldown>0) keyCooldown--;
        
        glClear( GL_COLOR_BUFFER_BIT );
        glClearColor(1.0,1.0,1.0,1.0);
        
        // RENDER GL          
        glPushMatrix( );

            // Control the position of the scene itself
            glTranslatef( halfScreenWidth, halfScreenHeight, -500 );
            glRotatef( rotationX, 1, 0, 0 );
            glRotatef( rotationY, 0, 1, 0 );
            glTranslatef( -halfScreenWidth, -halfScreenHeight, 500 );

            if (toggleFocusModel) glTranslatef( -(rootPos[0]*scale), -(rootPos[1]*scale), -(rootPos[2]*scale) );


            if (togglePlayClip) {
                if (debug) itLimit--;
                if (itLimit>0) {
                    std::cout << "\n\n\n\n";
                    SimulateNextFrame();      // Function to simulate the next frame
                }
                // SetFrame(currentFrame+1); // KEEP AT CURRENT FRAME FOR NOW
            }

            // Draw OBJ to screen
            DrawMesh();

            PlaceAxis();


        glPopMatrix();
        
        glfwSwapBuffers( window );
        
        glfwPollEvents( );
    }
        
    glfwTerminate( );
    
    return 0;
}


void keyCallback( GLFWwindow *window, int key, int scancode, int action, int mods )
{

    const GLfloat rotationSpeed = 1;
    
    // actions are GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    if ( action == GLFW_PRESS || action == GLFW_REPEAT )
    {

        if (loadSwitch)
        {

            switch ( key )
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

            }
        }
    }


        
    if (keyCooldown == 0){
        // GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT

        if ( key == 76 && action == GLFW_PRESS){        // L key press
            while(_kbhit()) _getch(); // Clear key buffer
            std::cout << "\n==== LOAD FILE ====" << std::endl;

            string filename;
            cout << "Enter the name of a .obj file: ";
            cin >> filename;
            filename+=".obj";

            if (FILE *file = fopen(filename.c_str(), "r")) {
                fclose(file);
                std::cout << "Loading File... " << filename << std::endl;
                LoadOBJ(filename);
            } else {
                std::cout << "Error: File not found" << std::endl;
                std::cout << "Do not include '.obj' when entering a file name..." << std::endl;
            }   
            keyCooldown = keyCooldownMax;
            while(_kbhit()) _getch(); // Clear key buffer

        }else if ( key == 83 && action == GLFW_PRESS){  // S key press
            while(_kbhit()) _getch(); // Clear key buffer
            std::cout << "\n==== SAVE FILE ====" << std::endl;

            string filename;
            cout << "Enter name to save the file under: ";
            cin >> filename;
            std::cout << "Saving..." << std::endl;

            fFilename = filename;
            filename+=".obj";
            SaveOBJ(filename);

            while(_kbhit()) _getch(); // Clear key buffer
            keyCooldown = keyCooldownMax;

        }
    }
}



vElement LogVertexDistances(vElement v) {


    // Creating a iterator pointing to start of set
    std::set<int>::iterator it = v.fIndex.begin();
    // std::cout << "> DISTANCES";
    
    // Iterate till the end of set
    while (it != v.fIndex.end()) {

        // log the distance between the two verticies
        v.fDistance.push_back( VertexDistance( v, (*std::next(vList.begin(), (*it))) ) );

        // std::cout << "   " << (*it) << "_" << VertexDistance( v, (*std::next(vList.begin(), (*it))) );

        //Increment the iterator
        it++;
    }

    std::cout << std::endl;
    return v;
}


// Generate a linked list of adjacent nodes for each vertex in the mesh
void GenerateLinkList(){

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



void DrawLine(float x1, float y1, float z1, float x2, float y2, float z2, GLfloat edgeLength )
{
    // GLfloat halfSideLength = edgeLength * 0.5f;
    GLfloat vertices[] =
    {
        x1, y1, z1,
        x2, y2, z2
    };

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f( lineColour[0], lineColour[1], lineColour[2] );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );

    glDrawArrays( GL_LINES, 0, 2 );

    glDisableClientState( GL_VERTEX_ARRAY );
}



void DrawCube( GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength )
{
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
    



    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glColor3f( cubeColour[0], cubeColour[1], cubeColour[2] );
    glEnableClientState( GL_VERTEX_ARRAY );
    glVertexPointer( 3, GL_FLOAT, 0, vertices );

    glDrawArrays( GL_QUADS, 0, 24 );
    
    glDisableClientState( GL_VERTEX_ARRAY );

    // std::cout << centerPosX - halfSideLength << centerPosY + halfSideLength << centerPosZ + halfSideLength << std::endl;
    // std::cout << centerPosX + halfSideLength << centerPosY - halfSideLength << centerPosZ - halfSideLength << std::endl;

}







void Clear()
{
    // Reset variables to defaults before loading new obj
    fList.clear();
    vList.clear();
    vnList.clear();
    vtList.clear();

}


int LoadOBJ(std::string file) {
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
            sscanf(line, "%s %f %f %f", dummy, &(v.x), &(v.y), &(v.z));

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
            // vfList.push_back(vElement);
            break;
        }
    }
    inputFile.close();
    
    loadSwitch = true;
   
    GenerateLinkList();

    return 0;
}



int SaveOBJ(std::string objFile) {
    
    ofstream output(objFile);
    if (!output.is_open())
        return -1;

    output << "o " << fFilename << std::endl;

    while (vList.size()>0)
    {
        vElement v = vList.front();
        vList.pop_front();
        output << "v " << v.x << " " << v.y << " " << v.z << std::endl;
    }

    while (vnList.size()>0)
    {
        vElement v = vnList.front();
        vnList.pop_front();
        output << "vn " << v.x << " " << v.y << " " << v.z << std::endl;
    }

    while (vtList.size()>0)
    {
        vElement v = vtList.front();
        vtList.pop_front();
        output << "vt " << v.x << " " << v.y << " " << v.z << std::endl;
    }

    while (fList.size()>0)
    {
        fElement f = fList.front();
        fList.pop_front();
        f.v1+=1;
        f.v2+=1;
        f.v3+=1;
        output << "f " << f.v1 << "/" << f.vt1 << "/" << f.vn1 << " " << f.v2 << "/" << f.vt2 << "/" << f.vn2 << " " << f.v3 << "/" << f.vt3 << "/" << f.vn3 << std::endl;
    }

    output.close();
    
    // Re-load file to fill lists again
    LoadOBJ(objFile);

    return 0; 
}



// Multiply 2 matrices together and return the result
std::vector<float> MultMatrix (std::vector<float> m0, std::vector<float> m1)
{
    std::vector<float> M
    {
        0,0,0,0,
        0,0,0,0,
        0,0,0,0,
        0,0,0,0
    };

    // for (int ny = 0; ny < 4; ny++)
    // {
    //     // std::cout << "---------------\n";
    //     for (int nx = 0; nx < 4; nx++)
    //     {
    //         // std::cout << "M[" << (ny*4)+nx << "]\n";
    //         // std::cout << "\t " << ((ny*4)+0) << "\t" << (nx + 0) << "\n";
    //         // std::cout << "\t " << ((ny*4)+1) << "\t" << (nx + 4) << "\n";
    //         // std::cout << "\t " << ((ny*4)+2) << "\t" << (nx + 8) << "\n";
    //         // std::cout << "\t " << ((ny*4)+3) << "\t" << (nx + 12) << "\n";

    //         M[(ny*4)+nx] =  ( m0[(ny*4)+0] * m1[nx + 0]  ) + 
    //                         ( m0[(ny*4)+1] * m1[nx + 4]  ) + 
    //                         ( m0[(ny*4)+2] * m1[nx + 8]  ) + 
    //                         ( m0[(ny*4)+3] * m1[nx + 12] ); 
    //     }
    // }

    M = 
    {
        (m0[0]*m1[0] + m0[1]*m1[4] + m0[2]*m1[8] + m0[3]*m1[12]), (m0[0]*m1[1] + m0[1]*m1[5] + m0[2]*m1[9] + m0[3]*m1[13]), (m0[0]*m1[2] + m0[1]*m1[6] + m0[2]*m1[10] + m0[3]*m1[14]), (m0[0]*m1[3] + m0[1]*m1[7] + m0[2]*m1[11] + m0[3]*m1[15]), 
        (m0[4]*m1[0] + m0[5]*m1[4] + m0[6]*m1[8] + m0[7]*m1[12]), (m0[4]*m1[1] + m0[5]*m1[5] + m0[6]*m1[9] + m0[7]*m1[13]), (m0[4]*m1[2] + m0[5]*m1[6] + m0[6]*m1[10] + m0[7]*m1[14]), (m0[4]*m1[3] + m0[5]*m1[7] + m0[6]*m1[11] + m0[7]*m1[15]), 
        (m0[8]*m1[0] + m0[9]*m1[4] + m0[10]*m1[8]+ m0[11]*m1[12]),(m0[8]*m1[1] + m0[9]*m1[5] + m0[10]*m1[9]+ m0[11]*m1[13]),(m0[8]*m1[2] + m0[9]*m1[6] + m0[10]*m1[10]+ m0[11]*m1[14]),(m0[8]*m1[3] + m0[9]*m1[7] + m0[10]*m1[11]+ m0[11]*m1[15]), 
        (m0[12]*m1[0]+ m0[13]*m1[4]+ m0[14]*m1[8]+ m0[15]*m1[12]),(m0[12]*m1[1]+ m0[13]*m1[5]+ m0[14]*m1[9]+ m0[15]*m1[13]),(m0[12]*m1[2]+ m0[13]*m1[6]+ m0[14]*m1[10]+ m0[15]*m1[14]),(m0[12]*m1[3]+ m0[13]*m1[7]+ m0[14]*m1[11]+ m0[15]*m1[15]) 
    };

    return M;
}





void DrawMesh()
{

    // Place Verticies
    for (int i = 0; i < vList.size(); i++)
    {
        vector<float> xyz  = { 0, 0, 0 };


        vElement v = *std::next(vList.begin(), i);


        // Scale Up - Add Offset
        xyz[0] = (v.x*scale)+halfScreenWidth;
        xyz[1] = (v.y*scale)+halfScreenHeight;
        xyz[2] = (v.z*scale)+-500;
        
        cubeColour = 
            {
                0,0,255,
                0,0,255,
                0,0,255
            };

        DrawCube(xyz[0], xyz[1], xyz[2], 5);

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
        v1.x = (v1.x*scale)+halfScreenWidth;
        v1.y = (v1.y*scale)+halfScreenHeight;
        v1.z = (v1.z*scale)+-500;

        v2.x = (v2.x*scale)+halfScreenWidth;
        v2.y = (v2.y*scale)+halfScreenHeight;
        v2.z = (v2.z*scale)+-500;

        v3.x = (v3.x*scale)+halfScreenWidth;
        v3.y = (v3.y*scale)+halfScreenHeight;
        v3.z = (v3.z*scale)+-500;       

        DrawLine(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, 50);
        DrawLine(v3.x, v3.y, v3.z, v2.x, v2.y, v2.z, 50);
        DrawLine(v3.x, v3.y, v3.z, v1.x, v1.y, v1.z, 50);

    }
}



void PlaceAxis()
{
    // ----- ORIGIN -----
    // Red Line
    lineColour[0] = 255;
    lineColour[1] = 0;
    lineColour[2] = 0;
    DrawLine(halfScreenWidth, halfScreenHeight, -500, halfScreenWidth+100, halfScreenHeight, -500, 50);

    // Green Line
    lineColour[0] = 0;
    lineColour[1] = 255;
    lineColour[2] = 0;
    DrawLine(halfScreenWidth, halfScreenHeight, -500, halfScreenWidth, halfScreenHeight+100, -500, 50);

    // Blue Line
    lineColour[0] = 0;
    lineColour[1] = 0;
    lineColour[2] = 255;
    DrawLine(halfScreenWidth, halfScreenHeight, -500, halfScreenWidth, halfScreenHeight, -400, 50);


    // ----- GRID -----
    lineColour =
    {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    float gridWidth = 100000;
    float cubeSpace = 80;
    for (int x = -40; x < 20; x++)
    {
        DrawLine(   halfScreenWidth + (x*cubeSpace), halfScreenHeight-1, -gridWidth, 
                    halfScreenWidth + (x*cubeSpace), halfScreenHeight-1,  gridWidth, 0);

        DrawLine(   -gridWidth, halfScreenHeight-1, halfScreenWidth + (x*cubeSpace), 
                     gridWidth, halfScreenHeight-1, halfScreenWidth + (x*cubeSpace), 0);
    }
}



