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

    float vertices[] = {
        //ground1 0, 4
        -1.0f, -1.0f,                                                               0.0f, 0.0f,
         1.0f, -1.0f,                                                               wWidth / 128.0f, 0.0f,
        -1.0f, 128.0f / wHeight - 1.0f,                                             0.0f, 1.0f,
         1.0f, 128.0f / wHeight - 1.0f,                                             wWidth / 128.0f, 1.0f,


         //ground2 4, 4
         0.2f, 640.0f / wHeight - 1.0f,                                             0.0f, 0.0f,
         1.0f, 640.0f / wHeight - 1.0f,                                             0.4f * wWidth / 128.0f, 0.0f,
         0.2f, 768.0f / wHeight - 1.0f,                                             0.0f, 1.0f,
         1.0f, 768.0f / wHeight - 1.0f,                                             0.4f * wWidth / 128.0f, 1.0f,

         //waterfall 8, 4
         0.6f, -1.0f,                                                               0.0f, 0.0f,
         0.6f + 400.0f / wWidth, -1.0f,                                             1.0f, 0.0f,
         0.6f, 768.0f / wHeight - 1.0f,                                             0.0f, 1.0f,
         0.6f + 400.0f / wWidth, 768.0f / wHeight - 1.0f,                           1.0f, 1.0f,

         //board 12, 4
        -0.8f, 128.0f / wHeight - 1.0f,                                             0.0f, 0.0f,
        -0.8f + 256.0f / wWidth, 128.0f / wHeight - 1.0f,                           1.0f, 0.0f,
        -0.8f, 288.0f / wHeight + 128.0f / wHeight - 1.0f,                          0.0f, 1.0f,
        -0.8f + 256.0f / wWidth, 288.0f / wHeight + 128.0f / wHeight - 1.0f,        1.0f, 1.0f,

        //text 16, 4
        -0.785f, 290.0f / wHeight - 1.0f,                                           0.0f, 0.0f,
        -0.785f + 200.0f / wWidth, 290.0f / wHeight - 1.0f,                         1.0f, 0.0f,
        -0.785f, 350.0f / wHeight - 1.0f,                                           0.0f, 1.0f,
        -0.785f + 200.0f / wWidth, 350.0f / wHeight - 1.0f,                         1.0f, 1.0f,

        //dirt1 20, 4
        -0.8f, 128.0f / wHeight - 1.0f,                                             0.0f, 0.0f,
        1.0f, 128.0f / wHeight - 1.0f,                                              0.9f * wWidth / 128.0f, 0.0f,
        -0.8f, 350.0f / wHeight - 1.0f,                                             0.0f, (222.0f / wHeight / 2)* wHeight / 128.0f,
        1.0f, 350.0f / wHeight - 1.0f,                                              0.9f * wWidth / 128.0f, (222.0f / wHeight / 2)* wHeight / 128.0f,

        //dirt2 24, 4
        0.2f, 350.0f / wHeight - 1.0f,                                              0.0f, 0.0f,
        1.0f, 350.0f / wHeight - 1.0f,                                              0.4f * wWidth / 128.0f, 0.0f,
        0.2f, 640.0f / wHeight - 1.0f,                                              0.0f, (300.0f / wHeight / 2)* wHeight / 128.0f,
        1.0f, 640.0f / wHeight - 1.0f,                                              0.4f * wWidth / 128.0f, (300.0f / wHeight / 2)* wHeight / 128.0f,

        //dirt3 28, 3
       -0.7f, 350.0f / wHeight - 1.0f,                                              0.0f, 0.0f,
        0.2f, 350.0f / wHeight - 1.0f,                                              0.45f * wWidth / 128.0f, 0.0f,
        0.2f, 768.0f / wHeight - 1.0f,                                              0.45f * wWidth / 128.0f, (418.0f / wHeight / 2)* wHeight / 128.0f,

        //bush1 31, 4
        -1.0f, 128.0f / wHeight - 1.0f,                                             0.0f, 0.0f,
        -1.0f + 432.0f / wWidth, 128.0f / wHeight - 1.0f,                           1.0f, 0.0f,
        -1.0f, (128.0f + 304.0f) / wHeight - 1.0f,                                  0.0f, 1.0f,
        -1.0f + 432.0f / wWidth, (128.0f + 304.0f) / wHeight - 1.0f,                1.0f, 1.0f,

        //bush2 35, 4
        0.2f, -0.34f,                                                               0.0f, 0.0f,
        0.2f + 1568.0f / wWidth, -0.34f,                                            1.0f, 0.0f,
        0.2f, -0.34f + 320.0f / wHeight,                                            0.0f, 1.0f,
        0.2f + 1568.0f / wWidth, -0.34f + 320.0f / wHeight,                         1.0f, 1.0f,

        //bush3 39, 4
        0.1f, -0.4f,                                                                0.0f, 0.0f,
        0.1f + 256.0f / wWidth, -0.4f,                                              1.0f, 0.0f,
        0.1f, -0.4f + 208.0f / wHeight,                                             0.0f, 1.0f,
        0.1f + 256.0f / wWidth, -0.4f + 208.0f / wHeight,                           1.0f, 1.0f,

        //bush4 43, 4  
        0.0f, -0.45f,                                                               0.0f, 0.0f,
        0.0f + 256.0f / wWidth, -0.45f,                                             1.0f, 0.0f,
        0.0f, -0.45f + 208.0f / wHeight,                                            0.0f, 1.0f,
        0.0f + 256.0f / wWidth, -0.45f + 208.0f / wHeight,                          1.0f, 1.0f,

        //bush5 47, 4
        -0.1f, -0.46f,                                                              0.0f, 0.0f,
        -0.1f + 256.0f / wWidth, -0.46f,                                            1.0f, 0.0f,
        -0.1f, -0.46f + 208.0f / wHeight,                                           0.0f, 1.0f,
        -0.1f + 256.0f / wWidth, -0.46f + 208.0f / wHeight,                         1.0f, 1.0f,

        //bush6 51, 4
        -0.2f, -0.48f,                                                              0.0f, 0.0f,
        -0.2f + 256.0f / wWidth, -0.48f,                                            1.0f, 0.0f,
        -0.2f, -0.48f + 208.0f / wHeight,                                           0.0f, 1.0f,
        -0.2f + 256.0f / wWidth, -0.48f + 208.0f / wHeight,                         1.0f, 1.0f,

        //bush7 55, 4
        -0.3f, -0.53f,                                                              0.0f, 0.0f,
        -0.3f + 256.0f / wWidth, -0.53f,                                            1.0f, 0.0f,
        -0.3f, -0.53f + 208.0f / wHeight,                                           0.0f, 1.0f,
        -0.3f + 256.0f / wWidth, -0.53f + 208.0f / wHeight,                         1.0f, 1.0f,

        //bush8 59, 4
        -0.5f, -0.62f,                                                              0.0f, 0.0f,
        -0.5f + 256.0f / wWidth, -0.62f,                                            1.0f, 0.0f,
        -0.5f, -0.62f + 208.0f / wHeight,                                           0.0f, 1.0f,
        -0.5f + 256.0f / wWidth, -0.62f + 208.0f / wHeight,                         1.0f, 1.0f,

        //bush9 63, 4
        -0.6f, -0.66f,                                                              0.0f, 0.0f,
        -0.6f + 256.0f / wWidth, -0.66f,                                            1.0f, 0.0f,
        -0.6f, -0.66f + 208.0f / wHeight,                                           0.0f, 1.0f,
        -0.6f + 256.0f / wWidth, -0.66f + 208.0f / wHeight,                         1.0f, 1.0f,

        //sun 67, 4
        -0.75f, 0.5f,                                                               0.0f, 0.0f,
        -0.75f + 256.0f / wWidth, 0.5f,                                             1.0f, 0.0f,
        -0.75f, 0.5f + 256.0f / wHeight,                                            0.0f, 1.0f,
        -0.75f + 256.0f / wWidth, 0.5f + 256.0f / wHeight,                          1.0f, 1.0f,

        //cloud1 71, 4
        0.0f, 0.65f,                                                                0.0f, 0.0f,
        0.0f + 512.0f / wWidth, 0.65f,                                              1.0f, 0.0f,
        0.0f, 0.65f + 192.0f / wHeight,                                             0.0f, 1.0f,
        0.0f + 512.0f / wWidth, 0.65f + 192.0f / wHeight,                           1.0f, 1.0f,

        //cloud2 75, 4
        0.0f, 0.45f,                                                                0.0f, 0.0f,
        0.0f + 384.0f / wWidth, 0.45f,                                              1.0f, 0.0f,
        0.0f, 0.45f + 160.0f / wHeight,                                             0.0f, 1.0f,
        0.0f + 384.0f / wWidth, 0.45f + 160.0f / wHeight,                           1.0f, 1.0f,

        //cloud3 79, 4
        0.0f, 0.8f,                                                                 0.0f, 0.0f,
        0.0f + 224.0f / wWidth, 0.8f,                                               1.0f, 0.0f,
        0.0f, 0.8f + 96.0f / wHeight,                                               0.0f, 1.0f,
        0.0f + 224.0f / wWidth, 0.8f + 96.0f / wHeight,                             1.0f, 1.0f,

        //volcano 83, 4
        0.35f, -0.17f,                                                              0.0f, 0.0f,
        0.35f + 512.0f / wWidth, -0.17f,                                            1.0f, 0.0f,
        0.35f, -0.17f + 512.0f / wHeight,                                           0.0f, 1.0f,
        0.35f + 512.0f / wWidth, -0.17f + 512.0f / wHeight,                         1.0f, 1.0f,

        //tree1 87, 4
        0.76f, -0.35f,                                                              0.0f, 0.0f,
        0.76f + 432.0f / wWidth, -0.35f,                                            1.0f, 0.0f,
        0.76f, -0.35f + 788.0f / wHeight,                                           0.0f, 1.0f,
        0.76f + 432.0f / wWidth, -0.35f + 788.0f / wHeight,                         1.0f, 1.0f,

        //tree2 91, 4
        -0.25f, -0.44f,                                                             0.0f, 0.0f,
        -0.25f + 432.0f / wWidth, -0.44f,                                           1.0f, 0.0f,
        -0.25f, -0.44f + 788.0f / wHeight,                                          0.0f, 1.0f,
        -0.25f + 432.0f / wWidth, -0.44f + 788.0f / wHeight,                        1.0f, 1.0f,

        //tree3 95, 4
        -0.76f, -1.0f,                                                              0.0f, 0.0f,
        -0.76f + 432.0f / wWidth, -1.0f,                                            1.0f, 0.0f,
        -0.76f, -1.0f + 788.0f / wHeight,                                           0.0f, 1.0f,
        -0.76f + 432.0f / wWidth, -1.0f + 788.0f / wHeight,                         1.0f, 1.0f,
            
        //tree4 99, 4
        -0.45f, 128.0f / wHeight - 1.0f,                                            0.0f, 0.0f,
        -0.45f + 432.0f / wWidth, 128.0f / wHeight - 1.0f,                          1.0f, 0.0f,
        -0.45f, 916.0f / wHeight - 1.0f,                                            0.0f, 1.0f,
        -0.45f + 432.0f / wWidth,  916.0f / wHeight - 1.0f,                         1.0f, 1.0f,

        //tree5 103, 4
        0.0f, -0.55f,                                                               0.0f, 0.0f,
        0.0f + 432.0f / wWidth, -0.55f,                                             1.0f, 0.0f,
        0.0f, -0.55f + 788.0f / wHeight,                                            0.0f, 1.0f,
        0.0f + 432.0f / wWidth, -0.55f + 788.0f / wHeight,                          1.0f, 1.0f,

        //person 107, 4
        0.0f, 128.0f / wHeight - 1.0f,                                              0.0f, 0.0f,
        -0.0f + 272.0f / wWidth, 128.0f / wHeight - 1.0f,                           1.0f, 0.0f,
        -0.0f, 498.0f / wHeight - 1.0f,                                             0.0f, 1.0f,
        -0.0f + 272.0f / wWidth,  496.0f / wHeight - 1.0f,                          1.0f, 1.0f
    };

    const int size = 111;
    unsigned int stride = 4 * sizeof(float);

    //VAO and VBO
    unsigned int VAO, VBO;
    VAO = createVAO(&VBO, vertices, size, stride);

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

        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 67, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
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
        glBindTexture(GL_TEXTURE_2D, cloudTexture1);
        glDrawArrays(GL_TRIANGLE_STRIP, 71, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUniform2f(uPosLoc, cloud2_x, 0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 75, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUniform2f(uPosLoc, cloud3_x, 0);
        glBindTexture(GL_TEXTURE_2D, cloudTexture3);
        glDrawArrays(GL_TRIANGLE_STRIP, 79, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glUniform2f(uPosLoc, 0, 0);

        //Vulcano
        glBindTexture(GL_TEXTURE_2D, currentVolcanoTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 83, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Trees
        glBindTexture(GL_TEXTURE_2D, currentTree1Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 87, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, currentTree2Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 91, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, currentTree3Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 103, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Dirt
        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 24, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, dirtTexture);
        glDrawArrays(GL_TRIANGLES, 28, 3);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Board
        glBindTexture(GL_TEXTURE_2D, boardTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Text
        glBindTexture(GL_TEXTURE_2D, textTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Tree
        glBindTexture(GL_TEXTURE_2D, currentTree3Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 95, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Ground
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Waterfall
        glBindTexture(GL_TEXTURE_2D, currentWaterfallTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Bush
        glBindTexture(GL_TEXTURE_2D, bushTexture3);
        glDrawArrays(GL_TRIANGLE_STRIP, 31, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture1);
        glDrawArrays(GL_TRIANGLE_STRIP, 35, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 39, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 43, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 47, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 51, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 55, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 59, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindTexture(GL_TEXTURE_2D, bushTexture2);
        glDrawArrays(GL_TRIANGLE_STRIP, 63, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

        //Tree
        glBindTexture(GL_TEXTURE_2D, currentTree1Texture);
        glDrawArrays(GL_TRIANGLE_STRIP, 99, 4);
        glBindTexture(GL_TEXTURE_2D, 0);

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

        glBindTexture(GL_TEXTURE_2D, currentPersonTexture);
        glUniform2f(uPosLoc, person_position, 0);
        glDrawArrays(GL_TRIANGLE_STRIP, 107, 4);
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

    glDeleteVertexArrays(1, &VAO);

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
    glBufferData(GL_ARRAY_BUFFER, size * 4 * sizeof(float), vertices, GL_STATIC_DRAW);

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