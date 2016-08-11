// CS148 Summer 2016 Homework 3 - Shaders
// Do not modify this file; you should only
// need to fill out phong.vs and phong.frag .

#include <iostream>
#include <cmath>
#include "TerrainGrid.h"
#include "SOIL/SOIL.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM Mathematics
#define GLM_FORCE_RADIANS // force everything in radian
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
GLuint *LoadTextures (vector<const GLchar*> imageFile);
GLuint LoadTexture (const GLchar* imageFile);
GLuint loadCubemap(vector<const GLchar*> faces);

// Window dimensions
const GLuint WIDTH = 1920, HEIGHT = 1080;

// Camera
Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));
GLfloat lastX  =  WIDTH  / 2.0;
GLfloat lastY  =  HEIGHT / 2.0;
bool    keys[1024];

// Light attributes
glm::vec3 lightPos(-10.0f, 20.0f, -10.0f); //100,150,100
glm::vec3 origLightPos(-10.0f, 20.0f, -10.0f);

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// The MAIN function, from here we start the application and run the game loop
int main()
{
    int DEBUG = 0;
    // should be on the scale of (2^x) + 1
    int recursiveSteps = 10;
    int xScale = pow(2,recursiveSteps) + 1, zScale = pow(2,recursiveSteps) + 1;
    float MAP_SCALE = .03;

    float*** terrain = new float**[xScale];
    for(int i = 0; i < xScale; i++) {
        terrain[i] = new float*[zScale];
        for(int j = 0; j < zScale; j++){
            terrain[i][j] = new float[3];
        }
    }
    float testMaxHeight = 0, testMinHeight = 0;
    // should be the 'x' in the scale equation.
    
    float maxPossibleHeight = initialize(terrain);
    //DiamondSquare(terrain,0, 0, xScale-1, zScale-1, recursiveSteps, testMaxHeight, testMinHeight);

    float range = 10;
    diamondSquare(terrain, xScale-1, range);
    
    int laplacianSmoothingRecursion = 16;
    laplacianSmoothing(terrain,laplacianSmoothingRecursion);

    
    float*** normals = new float**[xScale];
    for(int i = 0; i < xScale; i++) {
        normals[i] = new float*[zScale];
        for(int j = 0; j < zScale; j++){
            normals[i][j] = new float[3];
        }
    }

    GenerateNormals(normals, terrain);


    if(DEBUG == 1){
    //DEBUG
        for(int i = 0; i < xScale; i++) {
            for(int j = 0; j < zScale; j++){
                for(int k = 0; k < 3; k++) {
                    cout<<"Index "<<i*xScale+j*zScale+k<< " Vertex "<< terrain[i][j][k]<<endl;
                }   
            }
        }
    }

    camera.Position.y = testMaxHeight;
    camera.Pitch = -70.0f;

    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "HW3", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // GLFW Options
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();

    int w,h;
    glfwGetFramebufferSize( window, &w, &h);

    // Define the viewport dimensions
    glViewport(0, 0, w, h);

    // OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Build and compile our shader program
    Shader lightingShader("phong.vs", "phong.frag");
    Shader skyboxShader("skybox.vs", "skybox.frag");
    Shader simpleDepthShader("shadowShader.vs","shadowShader.frag");

    /*
    added in the texture coordinates but they're just either randomly a 1 or a 0 right now
    which is why we're getting the craziness.

    we should create textures[xScale][zScale][2] that holds all the texture Coordinate info
    that way we can iterate over it easily here

    */
   
    int verticesSize = xScale*zScale*8;
    GLfloat *vertices = new GLfloat[verticesSize];
    int index = 0;
    for(int i = 0; i < xScale; i++){
        for(int j = 0 ; j < zScale; j++){
            for(int k = 0; k < 3; k++){
                vertices[index] = terrain[i][j][k];
                //cout << "Index " << index << " vertex " << vertices[index] << endl;
                index++;

            }
            for(int k = 0; k < 3; k++){
                vertices[index] = normals[i][j][k];
                index++;
            }
            vertices[index] = (float)(i%50)/49.0;
            index++;
            vertices[index] = (float)(j%50)/49.0;
            index++;
        }
    }
    index = 0;
    int degenTriangleSize = 2*(xScale-1)*zScale+2*(xScale-1)-2;
    unsigned int *indices = new unsigned int[degenTriangleSize];
    for(int x = 0; x < xScale-1; x++){
        for(int z = 0; z < zScale; z++){
            indices[index] = (x*xScale)+z;
            //cout << "indices Index "<< index << " vertex Index " << indices[index] << endl;
            index++;
            indices[index] = (x+1)*xScale+z;
            //cout << "indices Index "<< index << " vertex Index " << indices[index] << endl;
            index++;
        }
        if(x<xScale-2){
            indices[index] = (x+1)*xScale + zScale -1;
            //cout << "indices Index "<< index << " vertex Index " << indices[index] << endl;
            index++;
            indices[index] = (x+1)*xScale;
            //cout << "indices Index "<< index << " vertex Index " << indices[index] << endl;
            index++;
        }   
    }
    
    /*
    for (int i = 0; i < degenTriangleSize; i = i + 3){
        normals[floor(indices[i]/xScale)][indices[i]%xScale]
    }
    */

   // cout << index << endl;
    //cout << degenTriangleSize<< endl;
    // First, set the container's VAO (and VBO)
    GLuint VBO, containerVAO, elementBuffer;
    glGenBuffers(1,&elementBuffer);
    glGenVertexArrays(1, &containerVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(containerVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, verticesSize*sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, degenTriangleSize*sizeof(unsigned int),indices,GL_STATIC_DRAW);
    
    //glBindVertexArray(containerVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
   // GLuint lightVAO;
    //glGenVertexArrays(1, &lightVAO);
    //glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


    //This is where the texture stuff starts
    /*
    I essentially copied the code from this website http://learnopengl.com/#!Getting-started/Textures
    this pretty much just initializes and loads the image we want into a texture buffer, which in our 
    case is the GLuint grasstexture or the GLuint sandtexture.

    pretty straightforward.

    you do need to download SOIL from here: http://www.lonesock.net/soil.html 
    im pretty sure it should run after you just download it.

    */
    //grass
    GLuint grasstexture = LoadTexture("newGrass.jpg");

    //sand
    GLuint sandtexture = LoadTexture("sandRipples.jpg");

    //snow
    GLuint snowtexture = LoadTexture("plainSnow.jpg");

    //granite
    GLuint rocktexture = LoadTexture("GrayRippleRock.jpg");


    //pragma region "object_initialization"
    GLfloat skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
   
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
  
        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
  
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    for(int i = 0; i < 36*3; i++){
        skyboxVertices[i]*= float(xScale)*.05;
    }

    // Setup skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    vector<const GLchar*> faces;
    faces.push_back("right.jpg");
    faces.push_back("left.jpg");
    faces.push_back("top.jpg");
    faces.push_back("bottom.jpg");
    faces.push_back("back.jpg");
    faces.push_back("front.jpg");
    GLuint cubemapTexture = loadCubemap(faces);


    //ADDING SHADOW STUFF HERE
    // Configure depth map FBO
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // - Create depth texture
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //END OF SHADOW SETUP STUFF

    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Clear the colorbuffer
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //SHADOW STUFF STARTS HERE


        // 1. Render depth of scene to texture (from light's perspective)
        // - Get light projection/view matrix.
        glm::mat4 lightProjection, lightView;
        glm::mat4 lightSpaceMatrix;
        GLfloat near_plane = 0.1f, far_plane = 100.0f;
        lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
        //lightProjection = glm::perspective(45.0f, (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane); // Note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
        lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightSpaceMatrix = lightProjection * lightView;
        // - now render scene from light's point of view
        simpleDepthShader.Use();
        glUniformMatrix4fv(glGetUniformLocation(simpleDepthShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
            glClear(GL_DEPTH_BUFFER_BIT);
        //this is where render scene goes  

            GLint modelShadowLoc = glGetUniformLocation(simpleDepthShader.Program, "model");          
            // Draw the container (using container's vertex attributes)
            glBindVertexArray(containerVAO);
            glm::mat4 model;
            glUniformMatrix4fv(modelShadowLoc, 1, GL_FALSE, glm::value_ptr(model));
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBuffer);
            glBindVertexArray(containerVAO);

            //this is for wireframeyness
            //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

            glDrawElements(GL_TRIANGLE_STRIP,degenTriangleSize,GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);


        //end of rendering the scene.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        //SHADOW STUFF ENDS HERE
        glViewport(0, 0, WIDTH, HEIGHT);



        // Use cooresponding shader when setting uniforms/drawing objects
        lightingShader.Use();
        GLint objectColorLoc = glGetUniformLocation(lightingShader.Program, "objectColor");
        GLint lightColorLoc  = glGetUniformLocation(lightingShader.Program, "lightColor");
        GLint lightPosLoc    = glGetUniformLocation(lightingShader.Program, "lightPos");
        GLint viewPosLoc     = glGetUniformLocation(lightingShader.Program, "viewPos");
        GLint maxHeight      = glGetUniformLocation(lightingShader.Program, "maxHeight");
        GLint minHeight      = glGetUniformLocation(lightingShader.Program, "minHeight");
        GLint MaxPosHeight   = glGetUniformLocation(lightingShader.Program, "MaxPosHeight");

        glUniform1f(MaxPosHeight, maxPossibleHeight); 
        glUniform1f(maxHeight, testMaxHeight);
        glUniform1f(minHeight, testMinHeight);
        glUniform3f(objectColorLoc, 0.4f, 0.5f, 0.7f);
        glUniform3f(lightColorLoc,  1.0f, 1.0f, 1.0f);
        glUniform3f(lightPosLoc,    lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(viewPosLoc,     camera.Position.x, camera.Position.y, camera.Position.z);


        GLuint texLoc;

        glActiveTexture(GL_TEXTURE0);
        texLoc = glGetUniformLocation(lightingShader.Program, "grasstexture");
        glUniform1i(texLoc, 0);
        glBindTexture(GL_TEXTURE_2D, grasstexture);

        glActiveTexture(GL_TEXTURE1);
        texLoc = glGetUniformLocation(lightingShader.Program, "sandtexture");
        glUniform1i(texLoc, 1);
        glBindTexture(GL_TEXTURE_2D, sandtexture);

        glActiveTexture(GL_TEXTURE2);
        texLoc = glGetUniformLocation(lightingShader.Program, "snowtexture");
        glUniform1i(texLoc, 2);
        glBindTexture(GL_TEXTURE_2D, snowtexture);

        glActiveTexture(GL_TEXTURE3);
        texLoc = glGetUniformLocation(lightingShader.Program, "rocktexture");
        glUniform1i(texLoc, 3);
        glBindTexture(GL_TEXTURE_2D, rocktexture);

        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "shadowMap"), 4);
        glBindTexture(GL_TEXTURE_2D, depthMap);


        // Create camera transformations
        //glm::mat4 view;
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)w / (GLfloat)h, 0.1f, 100.0f);
        // Get the uniform locations
        GLint modelLoc = glGetUniformLocation(lightingShader.Program, "model");
        GLint viewLoc  = glGetUniformLocation(lightingShader.Program,  "view");
        GLint projLoc  = glGetUniformLocation(lightingShader.Program,  "projection");
        // Pass the matrices to the shader
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lightingShader.Program, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Draw the container (using container's vertex attributes)
        glBindVertexArray(containerVAO);
        glm::mat4 newModel;
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(newModel));
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBuffer);
        glBindVertexArray(containerVAO);

        //this is for wireframeyness
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

        glDrawElements(GL_TRIANGLE_STRIP,degenTriangleSize,GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);


        
        //SKYBOX STUFF
        // Draw skybox as last
        glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.Use();     
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));    // Remove any translation component of the view matrix
        //view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(lightingShader.Program, "skybox"), 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // Set depth function back to default
        

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    for(int i = 0; i < xScale; i++) {
        for(int j = 0; j < zScale; j++){
            delete terrain[i][j];
            delete normals[i][j];
        }
        delete terrain[i];
        delete normals[i];
    }
    delete terrain;
    delete normals;
    delete indices;
    delete vertices;
    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement()
{
    // Camera controls
    if (keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (keys[GLFW_KEY_U])
        lightPos.x+=2.0f;
    if (keys[GLFW_KEY_Y])
        lightPos.x-=2.0f;
    if (keys[GLFW_KEY_J])
        lightPos.y+=2.0f;
    if (keys[GLFW_KEY_H])
        lightPos.y-=2.0f;
    if (keys[GLFW_KEY_M])
        lightPos.z+=2.0f;
    if (keys[GLFW_KEY_N])
        lightPos.z-=2.0f;
    if (keys[GLFW_KEY_SPACE])
        lightPos = origLightPos;
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

GLuint LoadTexture (const GLchar* imageFile){
    GLuint texture;

    // Load image, create texture and generate mipmaps
    int width, height;
    unsigned char* image;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);   // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    image = SOIL_load_image(imageFile, &width, &height, 0, SOIL_LOAD_RGB);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}


GLuint loadCubemap(vector<const GLchar*> faces)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width,height;
    unsigned char* image;
    for(int i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}
