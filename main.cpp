#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned int compileShader(GLenum type, const char* source);
unsigned int createShader(const char* vsSource, const char* fsSource);
static unsigned loadImageToTexture(const char* filePath);
unsigned int createVAO(unsigned int* VBO, const float* vertices, unsigned int size, unsigned int stride);
unsigned int loadAndSetupTexture(const char* texturePath, unsigned int shaderProgram, const char* uniformName);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

const int TARGET_FPS = 60;
const double TARGET_TIME_PER_FRAME = 1.0 / TARGET_FPS;

bool animationsActive = true;

int main(void)
{

    if (!glfwInit())
    {
        std::cout<<"GLFW Biblioteka se nije ucitala! :(\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const char wTitle[] = "Pixel Art";

    GLFWwindow* window;
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
    unsigned int wWidth = videoMode->width;
    unsigned int wHeight = videoMode->height;
    window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL);
    
    if (window == NULL)
    {
        std::cout << "Prozor nije napravljen! :(\n";
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glViewport(0, 0, wWidth, wHeight);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "GLEW nije mogao da se ucita! :'(\n";
        return 3;
    }

    unsigned int unifiedShader = createShader("basic.vert", "basic.frag");

    float ground_vertices1[] = {
        -1.0f, -1.0f,                        0.0f, 0.0f,
         1.0f, -1.0f,                        wWidth / 128.0f, 0.0f,
        -1.0f, 128.0f / wHeight - 1.0f,      0.0f, 1.0f,
         1.0f, 128.0f / wHeight - 1.0f,      wWidth / 128.0f, 1.0f
    };

    float ground_vertices2[] = {
         0.2f, 640.0f / wHeight - 1.0f,      0.0f, 0.0f,
         1.0f, 640.0f / wHeight - 1.0f,      0.4f * wWidth / 128.0f, 0.0f,
         0.2f, 768.0f / wHeight - 1.0f,      0.0f, 1.0f,
         1.0f, 768.0f / wHeight - 1.0f,      0.4f * wWidth / 128.0f, 1.0f
    };

    float waterfall_vertices[] = {
        0.6f, -1.0f,                                          0.0f, 0.0f,
        0.6f + 400.0f / wWidth, -1.0f,                        1.0f, 0.0f,
        0.6f, 768.0f / wHeight - 1.0f,                        0.0f, 1.0f,
        0.6f + 400.0f / wWidth, 768.0f / wHeight - 1.0f,      1.0f, 1.0f
    };

    float board_vertices[] = {
        -0.8f, 128.0f / wHeight - 1.0f,                                          0.0f, 0.0f,
        -0.8f + 256.0f / wWidth, 128.0f / wHeight - 1.0f,                        1.0f, 0.0f,
        -0.8f, 288.0f / wHeight + 128.0f / wHeight - 1.0f,                       0.0f, 1.0f,
        -0.8f + 256.0f / wWidth, 288.0f / wHeight + 128.0f / wHeight - 1.0f,     1.0f, 1.0f
    };

    float text_vertices[] = {
        -0.785f, 290.0f / wHeight - 1.0f,                                0.0f, 0.0f,
        -0.785f + 200.0f / wWidth, 290.0f / wHeight - 1.0f,              1.0f, 0.0f,
        -0.785f, 350.0f / wHeight - 1.0f,                                0.0f, 1.0f,
        -0.785f + 200.0f / wWidth, 350.0f / wHeight - 1.0f,              1.0f, 1.0f
    };
    
    float dirt_vertices1[] = {
        -0.8f, 128.0f / wHeight - 1.0f,     0.0f, 0.0f,
        1.0f, 128.0f / wHeight - 1.0f,      0.9f * wWidth / 128.0f, 0.0f,
        -0.8f, 350.0f / wHeight - 1.0f,     0.0f, (222.0f / wHeight / 2)* wHeight / 128.0f,
        1.0f, 350.0f / wHeight - 1.0f,      0.9f * wWidth / 128.0f, (222.0f / wHeight / 2) * wHeight / 128.0f
    };

    float dirt_vertices2[] = {
        0.2f, 350.0f / wHeight - 1.0f,      0.0f, 0.0f,
        1.0f, 350.0f / wHeight - 1.0f,      0.4f * wWidth / 128.0f, 0.0f,
        0.2f, 640.0f / wHeight - 1.0f,      0.0f, (300.0f / wHeight / 2) * wHeight / 128.0f,
        1.0f, 640.0f / wHeight - 1.0f,      0.4f * wWidth / 128.0f, (300.0f / wHeight / 2) * wHeight / 128.0f
    };

    float dirt_vertices3[] = {
       -0.7f, 350.0f / wHeight - 1.0f,      0.0f, 0.0f,
        0.2f, 350.0f / wHeight - 1.0f,      0.45f * wWidth / 128.0f, 0.0f,
        0.2f, 768.0f / wHeight - 1.0f,      0.45f * wWidth / 128.0f, (418.0f / wHeight / 2)* wHeight / 128.0f
    };

    float bush_vertices1[] = {
        -1.0f, 128.0f / wHeight - 1.0f,                                            0.0f, 0.0f,
        -1.0f + 432.0f / wWidth, 128.0f / wHeight - 1.0f,                          1.0f, 0.0f,
        -1.0f, (128.0f + 304.0f) / wHeight - 1.0f,                                 0.0f, 1.0f,
        -1.0f + 432.0f / wWidth, (128.0f + 304.0f) / wHeight - 1.0f,               1.0f, 1.0f
    };

    float bush_vertices2[] = {
        0.2f, -0.34f,                                                          0.0f, 0.0f,
        0.2f + 1568.0f / wWidth, -0.34f,                                       1.0f, 0.0f,
        0.2f, -0.34f + 320.0f / wHeight,                                       0.0f, 1.0f,
        0.2f + 1568.0f / wWidth, -0.34f + 320.0f / wHeight,                    1.0f, 1.0f
    };

    float bush_vertices3[] = {
        0.1f, -0.4f,                                                          0.0f, 0.0f,
        0.1f + 256.0f / wWidth, -0.4f,                                        1.0f, 0.0f,
        0.1f, -0.4f + 208.0f / wHeight,                                       0.0f, 1.0f,
        0.1f + 256.0f / wWidth, -0.4f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float bush_vertices4[] = {
        0.0f, -0.45f,                                                          0.0f, 0.0f,
        0.0f + 256.0f / wWidth, -0.45f,                                        1.0f, 0.0f,
        0.0f, -0.45f + 208.0f / wHeight,                                       0.0f, 1.0f,
        0.0f + 256.0f / wWidth, -0.45f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float bush_vertices5[] = {
        -0.1f, -0.46f,                                                          0.0f, 0.0f,
        -0.1f + 256.0f / wWidth, -0.46f,                                        1.0f, 0.0f,
        -0.1f, -0.46f + 208.0f / wHeight,                                       0.0f, 1.0f,
        -0.1f + 256.0f / wWidth, -0.46f + 208.0f / wHeight,                     1.0f, 1.0f
    };
    
    float bush_vertices6[] = {
        -0.2f, -0.48f,                                                          0.0f, 0.0f,
        -0.2f + 256.0f / wWidth, -0.48f,                                        1.0f, 0.0f,
        -0.2f, -0.48f + 208.0f / wHeight,                                       0.0f, 1.0f,
        -0.2f + 256.0f / wWidth, -0.48f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float bush_vertices7[] = {
        -0.3f, -0.53f,                                                          0.0f, 0.0f,
        -0.3f + 256.0f / wWidth, -0.53f,                                        1.0f, 0.0f,
        -0.3f, -0.53f + 208.0f / wHeight,                                       0.0f, 1.0f,
        -0.3f + 256.0f / wWidth, -0.53f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float bush_vertices8[] = {
        -0.5f, -0.62f,                                                          0.0f, 0.0f,
        -0.5f + 256.0f / wWidth, -0.62f,                                        1.0f, 0.0f,
        -0.5f, -0.62f + 208.0f / wHeight,                                       0.0f, 1.0f,
        -0.5f + 256.0f / wWidth, -0.62f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float bush_vertices9[] = {
        -0.6f, -0.66f,                                                          0.0f, 0.0f,
        -0.6f + 256.0f / wWidth, -0.66f,                                        1.0f, 0.0f,
        -0.6f, -0.66f + 208.0f / wHeight,                                       0.0f, 1.0f,
        -0.6f + 256.0f / wWidth, -0.66f + 208.0f / wHeight,                     1.0f, 1.0f
    };

    float sun_vertices[] = {
        -0.75f, 0.5f,                                              0.0f, 0.0f,
        -0.75f + 256.0f / wWidth, 0.5f,                            1.0f, 0.0f,
        -0.75f, 0.5f + 256.0f / wHeight,                           0.0f, 1.0f,
        -0.75f + 256.0f / wWidth, 0.5f + 256.0f / wHeight,         1.0f, 1.0f
    };

    float cloud_vertices1[] = {
        0.0f, 0.65f,                                              0.0f, 0.0f,
        0.0f + 512.0f / wWidth, 0.65f,                            1.0f, 0.0f,
        0.0f, 0.65f + 192.0f / wHeight,                           0.0f, 1.0f,
        0.0f + 512.0f / wWidth, 0.65f + 192.0f / wHeight,          1.0f, 1.0f
    };

    float cloud_vertices2[] = {
        0.0f, 0.45f,                                              0.0f, 0.0f,
        0.0f + 384.0f / wWidth, 0.45f,                            1.0f, 0.0f,
        0.0f, 0.45f + 160.0f / wHeight,                           0.0f, 1.0f,
        0.0f + 384.0f / wWidth, 0.45f + 160.0f / wHeight,          1.0f, 1.0f
    };

    float cloud_vertices3[] = {
        0.0f, 0.8f,                                              0.0f, 0.0f,
        0.0f + 224.0f / wWidth, 0.8f,                            1.0f, 0.0f,
        0.0f, 0.8f + 96.0f / wHeight,                            0.0f, 1.0f,
        0.0f + 224.0f / wWidth, 0.8f + 96.0f / wHeight,          1.0f, 1.0f
    };

    float volcano_vertices[] = {
        0.35f, -0.17f,                                                         0.0f, 0.0f,
        0.35f + 512.0f / wWidth, -0.17f,                                       1.0f, 0.0f,
        0.35f, -0.17f + 512.0f / wHeight,                                      0.0f, 1.0f,
        0.35f + 512.0f / wWidth, -0.17f + 512.0f / wHeight,                    1.0f, 1.0f
    };

    float tree_vertices1[] = {
        0.76f, -0.35f,                                                         0.0f, 0.0f,
        0.76f + 432.0f / wWidth, -0.35f,                                       1.0f, 0.0f,
        0.76f, -0.35f + 788.0f / wHeight,                                      0.0f, 1.0f,
        0.76f + 432.0f / wWidth, -0.35f + 788.0f / wHeight,                    1.0f, 1.0f
    };

    float tree_vertices2[] = {
        -0.25f, -0.44f,                                                         0.0f, 0.0f,
        -0.25f + 432.0f / wWidth, -0.44f,                                       1.0f, 0.0f,
        -0.25f, -0.44f + 788.0f / wHeight,                                      0.0f, 1.0f,
        -0.25f + 432.0f / wWidth, -0.44f + 788.0f / wHeight,                    1.0f, 1.0f
    };

    float tree_vertices5[] = {
        0.0f, -0.55f,                                                         0.0f, 0.0f,
        0.0f + 432.0f / wWidth, -0.55f,                                       1.0f, 0.0f,
        0.0f, -0.55f + 788.0f / wHeight,                                      0.0f, 1.0f,
        0.0f + 432.0f / wWidth, -0.55f + 788.0f / wHeight,                    1.0f, 1.0f
    };

    float tree_vertices3[] = {
        -0.76f, -1.0f,                                      0.0f, 0.0f,
        -0.76f + 432.0f / wWidth, -1.0f,                    1.0f, 0.0f,
        -0.76f, -1.0f + 788.0f / wHeight,                                      0.0f, 1.0f,
        -0.76f + 432.0f / wWidth, -1.0f + 788.0f / wHeight,                   1.0f, 1.0f
    };

    float tree_vertices4[] = {
        -0.45f, 128.0f / wHeight - 1.0f,                                      0.0f, 0.0f,
        -0.45f + 432.0f / wWidth, 128.0f / wHeight - 1.0f,                    1.0f, 0.0f,
        -0.45f, 916.0f / wHeight - 1.0f,                                      0.0f, 1.0f,
        -0.45f + 432.0f / wWidth,  916.0f / wHeight - 1.0f,                   1.0f, 1.0f
    };

    float person_vertices[] = {
        0.0f, 128.0f / wHeight - 1.0f,                                       0.0f, 0.0f,
        -0.0f + 272.0f / wWidth, 128.0f / wHeight - 1.0f,                    1.0f, 0.0f,
        -0.0f, 498.0f / wHeight - 1.0f,                                      0.0f, 1.0f,
        -0.0f + 272.0f / wWidth,  496.0f / wHeight - 1.0f,                   1.0f, 1.0f
    };

    unsigned int stride = (2 + 2) * sizeof(float);

    //VAO and VBO
    unsigned int groundVAO1, groundVBO1;
    groundVAO1 = createVAO(&groundVBO1, ground_vertices1, sizeof(ground_vertices1), stride);

    unsigned int groundVAO2, groundVBO2;
    groundVAO2 = createVAO(&groundVBO2, ground_vertices2, sizeof(ground_vertices2), stride);

    unsigned int waterfallVAO, waterfallVBO;
    waterfallVAO = createVAO(&waterfallVBO, waterfall_vertices, sizeof(waterfall_vertices), stride);

    unsigned int boardVAO, boardVBO;
    boardVAO = createVAO(&boardVBO, board_vertices, sizeof(board_vertices), stride);

    unsigned int textVAO, textVBO;
    textVAO = createVAO(&textVBO, text_vertices, sizeof(text_vertices), stride);

    unsigned int dirtVAO1, dirtVBO1;
    dirtVAO1 = createVAO(&dirtVBO1, dirt_vertices1, sizeof(dirt_vertices1), stride);

    unsigned int dirtVAO2, dirtVBO2;
    dirtVAO2 = createVAO(&dirtVBO2, dirt_vertices2, sizeof(dirt_vertices2), stride);

    unsigned int dirtVAO3, dirtVBO3;
    dirtVAO3 = createVAO(&dirtVBO3, dirt_vertices3, sizeof(dirt_vertices3), stride);

    unsigned int bushVAO1, bushVBO1;
    bushVAO1 = createVAO(&bushVBO1, bush_vertices1, sizeof(bush_vertices1), stride);

    unsigned int bushVAO2, bushVBO2;
    bushVAO2 = createVAO(&bushVBO2, bush_vertices2, sizeof(bush_vertices2), stride);

    unsigned int bushVAO3, bushVBO3;
    bushVAO3 = createVAO(&bushVBO3, bush_vertices3, sizeof(bush_vertices3), stride);

    unsigned int bushVAO4, bushVBO4;
    bushVAO4 = createVAO(&bushVBO4, bush_vertices4, sizeof(bush_vertices4), stride);

    unsigned int bushVAO5, bushVBO5;
    bushVAO5 = createVAO(&bushVBO5, bush_vertices5, sizeof(bush_vertices5), stride);

    unsigned int bushVAO6, bushVBO6;
    bushVAO6 = createVAO(&bushVBO6, bush_vertices6, sizeof(bush_vertices6), stride);

    unsigned int bushVAO7, bushVBO7;
    bushVAO7 = createVAO(&bushVBO7, bush_vertices7, sizeof(bush_vertices7), stride);

    unsigned int bushVAO8, bushVBO8;
    bushVAO8 = createVAO(&bushVBO8, bush_vertices8, sizeof(bush_vertices8), stride);

    unsigned int bushVAO9, bushVBO9;
    bushVAO9 = createVAO(&bushVBO9, bush_vertices9, sizeof(bush_vertices9), stride);

    unsigned int sunVAO, sunVBO;
    sunVAO = createVAO(&sunVBO, sun_vertices, sizeof(sun_vertices), stride);

    unsigned int cloudVAO1, cloudVBO1;
    cloudVAO1 = createVAO(&cloudVBO1, cloud_vertices1, sizeof(cloud_vertices1), stride);

    unsigned int cloudVAO2, cloudVBO2;
    cloudVAO2 = createVAO(&cloudVBO2, cloud_vertices2, sizeof(cloud_vertices2), stride);

    unsigned int cloudVAO3, cloudVBO3;
    cloudVAO3 = createVAO(&cloudVBO3, cloud_vertices3, sizeof(cloud_vertices3), stride);

    unsigned int volcanoVAO, volcanoVBO;
    volcanoVAO = createVAO(&volcanoVBO, volcano_vertices, sizeof(volcano_vertices), stride);

    unsigned int treeVAO1, treeVBO1;
    treeVAO1 = createVAO(&treeVBO1, tree_vertices1, sizeof(tree_vertices1), stride);

    unsigned int treeVAO2, treeVBO2;
    treeVAO2 = createVAO(&treeVBO2, tree_vertices2, sizeof(tree_vertices2), stride);

    unsigned int treeVAO3, treeVBO3;
    treeVAO3 = createVAO(&treeVBO3, tree_vertices3, sizeof(tree_vertices3), stride);

    unsigned int treeVAO4, treeVBO4;
    treeVAO4 = createVAO(&treeVBO4, tree_vertices4, sizeof(tree_vertices4), stride);

    unsigned int treeVAO5, treeVBO5;
    treeVAO5 = createVAO(&treeVBO5, tree_vertices5, sizeof(tree_vertices5), stride);

    unsigned int personVAO, personVBO;
    personVAO = createVAO(&personVBO, person_vertices, sizeof(person_vertices), stride);

    //Textures
    unsigned groundTexture = loadAndSetupTexture("res/Ground.png", unifiedShader, "uTex");
    unsigned waterfallTexture1 = loadAndSetupTexture("res/Waterfall_1.png", unifiedShader, "uTex");
    unsigned waterfallTexture2 = loadAndSetupTexture("res/Waterfall_2.png", unifiedShader, "uTex");
    unsigned boardTexture = loadAndSetupTexture("res/Board.png", unifiedShader, "uTex");
    unsigned textTexture = loadAndSetupTexture("res/Sign.png", unifiedShader, "uTex");
    unsigned dirtTexture = loadAndSetupTexture("res/Dirt.png", unifiedShader, "uTex");
    unsigned bushTexture3 = loadAndSetupTexture("res/Bush_3.png", unifiedShader, "uTex");
    unsigned bushTexture2 = loadAndSetupTexture("res/Bush_2.png", unifiedShader, "uTex");
    unsigned bushTexture1 = loadAndSetupTexture("res/Bush_1.png", unifiedShader, "uTex");
    unsigned sunTexture = loadAndSetupTexture("res/Sun.png", unifiedShader, "uTex");
    unsigned cloudTexture1 = loadAndSetupTexture("res/Cloud_1.png", unifiedShader, "uTex");
    unsigned cloudTexture2 = loadAndSetupTexture("res/Cloud_2.png", unifiedShader, "uTex");
    unsigned cloudTexture3 = loadAndSetupTexture("res/Cloud_3.png", unifiedShader, "uTex");
    unsigned volcanoTexture1 = loadAndSetupTexture("res/Volacno_1.png", unifiedShader, "uTex");
    unsigned volcanoTexture2 = loadAndSetupTexture("res/Volacno_2.png", unifiedShader, "uTex");
    unsigned volcanoTexture3 = loadAndSetupTexture("res/Volacno_3.png", unifiedShader, "uTex");
    unsigned volcanoTexture4 = loadAndSetupTexture("res/Volacno_4.png", unifiedShader, "uTex");
    unsigned treeTexture1 = loadAndSetupTexture("res/Tree_1.png", unifiedShader, "uTex");
    unsigned treeTexture2 = loadAndSetupTexture("res/Tree_2.png", unifiedShader, "uTex");
    unsigned treeTexture3 = loadAndSetupTexture("res/Tree_3.png", unifiedShader, "uTex");
    unsigned treeTexture4 = loadAndSetupTexture("res/Tree_4.png", unifiedShader, "uTex");
    unsigned treeTexture5 = loadAndSetupTexture("res/Tree_5.png", unifiedShader, "uTex");
    unsigned treeTexture6 = loadAndSetupTexture("res/Tree_6.png", unifiedShader, "uTex");
    unsigned personTextureStanding = loadAndSetupTexture("res/Character_1.png", unifiedShader, "uTex");
    unsigned personTexture1 = loadAndSetupTexture("res/Character_3.png", unifiedShader, "uTex");
    unsigned personTexture2 = loadAndSetupTexture("res/Character_4.png", unifiedShader, "uTex");

    double lastSwitchTime = glfwGetTime();
    double lastSwitchTimePerson = glfwGetTime();

    const double switchInterval = 0.5;
    unsigned int currentWaterfallTexture = waterfallTexture1;
    unsigned int currentVolcanoTexture = volcanoTexture1;
    unsigned int currentTree1Texture = treeTexture1;
    unsigned int currentTree2Texture = treeTexture3;
    unsigned int currentTree3Texture = treeTexture5;
    unsigned int currentPersonTexture = personTextureStanding;

    unsigned int sunColorLocation = glGetUniformLocation(unifiedShader, "sunColor");
    unsigned int isSunLocation = glGetUniformLocation(unifiedShader, "isSun");
    unsigned int uPosLoc = glGetUniformLocation(unifiedShader, "uPos");

    float cloud_movement = 0.0f;
    float person_position = 0.0f;

    bool isVolcanoActive = false;
    float change = 0;

    float cloud1_x = 0.1f, cloud2_x = 0.7f, cloud3_x = -0.95f;

    while (!glfwWindowShouldClose(window))
    {
        double startTime = glfwGetTime();
        if (animationsActive) {
            if (startTime - lastSwitchTime >= switchInterval) {
                currentWaterfallTexture = (currentWaterfallTexture == waterfallTexture1) ? waterfallTexture2 : waterfallTexture1;
                if (isVolcanoActive)
                    currentVolcanoTexture = (currentVolcanoTexture == volcanoTexture3) ? volcanoTexture4 : volcanoTexture3;
                else
                    currentVolcanoTexture = (currentVolcanoTexture == volcanoTexture1) ? volcanoTexture2 : volcanoTexture1;

                currentTree1Texture = (currentTree1Texture == treeTexture1) ? treeTexture2 : treeTexture1;
                currentTree2Texture = (currentTree2Texture == treeTexture3) ? treeTexture4 : treeTexture3;
                currentTree3Texture = (currentTree3Texture == treeTexture5) ? treeTexture6 : treeTexture5;
                lastSwitchTime = startTime;
            }
        }
        
        glfwSetMouseButtonCallback(window, mouseButtonCallback);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        glClearColor(169.0f / 255.0f, 233.0f / 255.0f, 255.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(unifiedShader);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Sun
        if (animationsActive)
            change = (sin(startTime) + 1.0f) / 2.0f + 0.8f;
        
        glUniform3f(sunColorLocation, 1.0f, change, 1.0f);
        glUniform1i(isSunLocation, true);

        glBindVertexArray(sunVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUniform1i(isSunLocation, false);

        //Clouds
        if (animationsActive) {
            cloud1_x -= 0.0008f;
            cloud2_x -= 0.0005f;
            cloud3_x -= 0.0003f;
        }

        if (cloud1_x < -1.5f) {
            cloud1_x = 1.0f;
        }

        if (cloud2_x < -1.3f) {
            cloud2_x = 1.0f;
        }

        if (cloud3_x < -1.15f) {
            cloud3_x = 1.0f;
        }

        glUniform2f(uPosLoc, cloud1_x, 0);
        glBindVertexArray(cloudVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glUniform2f(uPosLoc, cloud2_x, 0);
        glBindVertexArray(cloudVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glUniform2f(uPosLoc, cloud3_x, 0);
        glBindVertexArray(cloudVAO3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glUniform2f(uPosLoc, 0, 0);

        //Vulcano
        glBindVertexArray(volcanoVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentVolcanoTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Trees
        glBindVertexArray(treeVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTree1Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(treeVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTree2Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(treeVAO5);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTree3Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Dirt
        glBindVertexArray(dirtVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(dirtVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(dirtVAO3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLES, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Board
        glBindVertexArray(boardVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, boardTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Text
        glBindVertexArray(textVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Tree
        glBindVertexArray(treeVAO3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTree3Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Ground
        glBindVertexArray(groundVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(groundVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Waterfall
        glBindVertexArray(waterfallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentWaterfallTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Bush
        glBindVertexArray(bushVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO2);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture1);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO3);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO5);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO6);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO7);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO8);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        glBindVertexArray(bushVAO9);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Tree
        glBindVertexArray(treeVAO4);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentTree1Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);

        //Person
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            person_position += 0.005f;
            if (startTime - lastSwitchTimePerson >= 0.15) {
                if (currentPersonTexture == personTextureStanding)
                    currentPersonTexture = personTexture1;
                currentPersonTexture = (currentPersonTexture == personTexture1) ? personTexture2 : personTexture1;
                lastSwitchTimePerson = startTime;
            }
        }
        else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            person_position -= 0.005f;
            if (startTime - lastSwitchTimePerson >= 0.15) {
                if (currentPersonTexture == personTextureStanding)
                    currentPersonTexture = personTexture1;
                currentPersonTexture = (currentPersonTexture == personTexture1) ? personTexture2 : personTexture1;
                lastSwitchTimePerson = startTime;
            }
            glUniform1i(glGetUniformLocation(unifiedShader, "mirror"), true);
        }
        else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            person_position = 0.0f;
            currentPersonTexture = personTextureStanding;
        }
        else {
            currentPersonTexture = personTextureStanding;
        }

        if (person_position > 0.5f) {
            person_position = 0.5f;
        }

        if (person_position < -1.0)
            isVolcanoActive = true;
        else
            isVolcanoActive = false;

        glBindVertexArray(personVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentPersonTexture);
        glUniform2f(uPosLoc, person_position, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUniform2f(uPosLoc, 0, 0);

        glUniform1i(glGetUniformLocation(unifiedShader, "mirror"), false);



        glUseProgram(0);

        glfwSwapBuffers(window);

        double endTime = glfwGetTime();
        double timeTaken = endTime - startTime;
        if (timeTaken < TARGET_TIME_PER_FRAME) {
            double sleepTime = TARGET_TIME_PER_FRAME - timeTaken;
            glfwWaitEventsTimeout(sleepTime);
        }

        glfwPollEvents();
    }


    glDeleteTextures(1, &groundTexture);
    glDeleteTextures(1, &waterfallTexture1);
    glDeleteTextures(1, &waterfallTexture2);
    glDeleteTextures(1, &boardTexture);
    glDeleteTextures(1, &textTexture);
    glDeleteTextures(1, &dirtTexture);
    glDeleteTextures(1, &bushTexture3);
    glDeleteTextures(1, &bushTexture2);
    glDeleteTextures(1, &bushTexture1);
    glDeleteTextures(1, &sunTexture);
    glDeleteTextures(1, &cloudTexture1);
    glDeleteTextures(1, &cloudTexture2);
    glDeleteTextures(1, &cloudTexture3);
    glDeleteTextures(1, &volcanoTexture1);
    glDeleteTextures(1, &volcanoTexture2);
    glDeleteTextures(1, &volcanoTexture3);
    glDeleteTextures(1, &volcanoTexture4);
    glDeleteTextures(1, &treeTexture1);
    glDeleteTextures(1, &treeTexture2);
    glDeleteTextures(1, &treeTexture3);
    glDeleteTextures(1, &treeTexture4);
    glDeleteTextures(1, &treeTexture5);
    glDeleteTextures(1, &treeTexture6);
    glDeleteTextures(1, &personTextureStanding);
    glDeleteTextures(1, &personTexture1);
    glDeleteTextures(1, &personTexture2);

    glDeleteVertexArrays(1, &groundVAO1);
    glDeleteVertexArrays(1, &groundVAO2);
    glDeleteVertexArrays(1, &waterfallVAO);
    glDeleteVertexArrays(1, &boardVAO);
    glDeleteVertexArrays(1, &textVAO);
    glDeleteVertexArrays(1, &dirtVAO1);
    glDeleteVertexArrays(1, &dirtVAO2);
    glDeleteVertexArrays(1, &dirtVAO3);
    glDeleteVertexArrays(1, &bushVAO1);
    glDeleteVertexArrays(1, &bushVAO2);
    glDeleteVertexArrays(1, &bushVAO3);
    glDeleteVertexArrays(1, &bushVAO4);
    glDeleteVertexArrays(1, &bushVAO5);
    glDeleteVertexArrays(1, &bushVAO6);
    glDeleteVertexArrays(1, &bushVAO7);
    glDeleteVertexArrays(1, &bushVAO8);
    glDeleteVertexArrays(1, &bushVAO9);
    glDeleteVertexArrays(1, &sunVAO);
    glDeleteVertexArrays(1, &cloudVAO1);
    glDeleteVertexArrays(1, &cloudVAO2);
    glDeleteVertexArrays(1, &cloudVAO3);
    glDeleteVertexArrays(1, &volcanoVAO);
    glDeleteVertexArrays(1, &treeVAO1);
    glDeleteVertexArrays(1, &treeVAO2);
    glDeleteVertexArrays(1, &treeVAO3);
    glDeleteVertexArrays(1, &treeVAO4);
    glDeleteVertexArrays(1, &treeVAO5);
    glDeleteVertexArrays(1, &personVAO);

    glDeleteProgram(unifiedShader);

    glfwTerminate();
    return 0;
}


unsigned int compileShader(GLenum type, const char* source)
{
    std::string content = "";
    std::ifstream file(source);
    std::stringstream ss;
    if (file.is_open())
    {
        ss << file.rdbuf();
        file.close();
        std::cout << "Uspjesno procitao fajl sa putanje \"" << source << "\"!" << std::endl;
    }
    else {
        ss << "";
        std::cout << "Greska pri citanju fajla sa putanje \"" << source << "\"!" << std::endl;
    }
     std::string temp = ss.str();
     const char* sourceCode = temp.c_str();

    int shader = glCreateShader(type);
    
    int success;
    char infoLog[512];
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        if (type == GL_VERTEX_SHADER)
            printf("VERTEX");
        else if (type == GL_FRAGMENT_SHADER)
            printf("FRAGMENT");
        printf(" sejder ima gresku! Greska: \n");
        printf(infoLog);
    }
    return shader;
}

unsigned int createShader(const char* vsSource, const char* fsSource)
{
    
    unsigned int program;
    unsigned int vertexShader;
    unsigned int fragmentShader;

    program = glCreateProgram();

    vertexShader = compileShader(GL_VERTEX_SHADER, vsSource);
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSource);

    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glValidateProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
    if (success == GL_FALSE)
    {
        glGetShaderInfoLog(program, 512, NULL, infoLog);
        std::cout << "Objedinjeni sejder ima gresku! Greska: \n";
        std::cout << infoLog << std::endl;
    }

    glDetachShader(program, vertexShader);
    glDeleteShader(vertexShader);
    glDetachShader(program, fragmentShader);
    glDeleteShader(fragmentShader);

    return program;
}

static unsigned loadImageToTexture(const char* filePath) {
    int TextureWidth;
    int TextureHeight;
    int TextureChannels;
    unsigned char* ImageData = stbi_load(filePath, &TextureWidth, &TextureHeight, &TextureChannels, 0);
    if (ImageData != NULL)
    {
        stbi__vertical_flip(ImageData, TextureWidth, TextureHeight, TextureChannels);

        GLint InternalFormat = -1;
        switch (TextureChannels) {
        case 1: InternalFormat = GL_RED; break;
        case 2: InternalFormat = GL_RG; break;
        case 3: InternalFormat = GL_RGB; break;
        case 4: InternalFormat = GL_RGBA; break;
        default: InternalFormat = GL_RGB; break;
        }

        unsigned int Texture;
        glGenTextures(1, &Texture);
        glBindTexture(GL_TEXTURE_2D, Texture);
        glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, TextureWidth, TextureHeight, 0, InternalFormat, GL_UNSIGNED_BYTE, ImageData);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(ImageData);
        return Texture;
    }
    else
    {
        std::cout << "Textura nije ucitana! Putanja texture: " << filePath << std::endl;
        stbi_image_free(ImageData);
        return 0;
    }
}

unsigned int createVAO(unsigned int* VBO, const float* vertices, unsigned int size, unsigned int stride) {
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return VAO;
}

unsigned int loadAndSetupTexture(const char* texturePath, unsigned int shaderProgram, const char* uniformName) {
    unsigned int texture = loadImageToTexture(texturePath);

    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUseProgram(shaderProgram);
    unsigned int uTexLoc = glGetUniformLocation(shaderProgram, uniformName);
    glUniform1i(uTexLoc, 0);
    glUseProgram(0);

    return texture;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            animationsActive = !animationsActive;
        }
    }
}