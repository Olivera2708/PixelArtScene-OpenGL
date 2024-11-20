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

const int TARGET_FPS = 60;
const double TARGET_TIME_PER_FRAME = 1.0 / TARGET_FPS;

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
         0.4f, 640.0f / wHeight - 1.0f,      0.0f, 0.0f,
         1.0f, 640.0f / wHeight - 1.0f,      0.3f * wWidth / 128.0f, 0.0f,
         0.4f, 768.0f / wHeight - 1.0f,      0.0f, 1.0f,
         1.0f, 768.0f / wHeight - 1.0f,      0.3f * wWidth / 128.0f, 1.0f
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
        0.4f, 350.0f / wHeight - 1.0f,      0.0f, 0.0f,
        1.0f, 350.0f / wHeight - 1.0f,      0.3f * wWidth / 128.0f, 0.0f,
        0.4f, 640.0f / wHeight - 1.0f,      0.0f, (300.0f / wHeight / 2) * wHeight / 128.0f,
        1.0f, 640.0f / wHeight - 1.0f,      0.3f * wWidth / 128.0f, (300.0f / wHeight / 2) * wHeight / 128.0f
    };

    float dirt_vertices3[] = {
       -0.7f, 350.0f / wHeight - 1.0f,      0.0f, 0.0f,
        0.4f, 350.0f / wHeight - 1.0f,      0.55f * wWidth / 128.0f, 0.0f,
        0.4f, 768.0f / wHeight - 1.0f,      0.55f * wWidth / 128.0f, (418.0f / wHeight / 2)* wHeight / 128.0f
    };

    float bush_vertices1[] = {
        -1.0f, 128.0f / wHeight - 1.0f,                                            0.0f, 0.0f,
        -1.0f + 432.0f / wWidth, 128.0f / wHeight - 1.0f,                          1.0f, 0.0f,
        -1.0f, (128.0f + 304.0f) / wHeight - 1.0f,                                 0.0f, 1.0f,
        -1.0f + 432.0f / wWidth, (128.0f + 304.0f) / wHeight - 1.0f,               1.0f, 1.0f
    };

    float sun_vertices[] = {
        -0.75f, 0.5f,                                              0.0f, 0.0f,
        -0.75f + 256.0f / wWidth, 0.5f,                            1.0f, 0.0f,
        -0.75f, 0.5f + 256.0f / wHeight,                           0.0f, 1.0f,
        -0.75f + 256.0f / wWidth, 0.5f + 256.0f / wHeight,         1.0f, 1.0f
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

    unsigned int sunVAO, sunVBO;
    sunVAO = createVAO(&sunVBO, sun_vertices, sizeof(sun_vertices), stride);

    //Textures
    unsigned groundTexture = loadAndSetupTexture("res/Ground.png", unifiedShader, "uTex");
    unsigned waterfallTexture1 = loadAndSetupTexture("res/Waterfall_1.png", unifiedShader, "uTex");
    unsigned waterfallTexture2 = loadAndSetupTexture("res/Waterfall_2.png", unifiedShader, "uTex");
    unsigned boardTexture = loadAndSetupTexture("res/Board.png", unifiedShader, "uTex");
    unsigned textTexture = loadAndSetupTexture("res/Sign.png", unifiedShader, "uTex");
    unsigned dirtTexture = loadAndSetupTexture("res/Dirt.png", unifiedShader, "uTex");
    unsigned bushTexture3 = loadAndSetupTexture("res/Bush_3.png", unifiedShader, "uTex");
    unsigned sunTexture = loadAndSetupTexture("res/Sun.png", unifiedShader, "uTex");

    double lastSwitchTime = glfwGetTime();
    const double switchInterval = 0.5;
    unsigned int currentWaterfallTexture = waterfallTexture1;

    while (!glfwWindowShouldClose(window))
    {
        double startTime = glfwGetTime();
        if (startTime - lastSwitchTime >= switchInterval) {
            currentWaterfallTexture = (currentWaterfallTexture == waterfallTexture1) ? waterfallTexture2 : waterfallTexture1;
            lastSwitchTime = startTime;
        }
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
        
        glClearColor(169.0f / 255.0f, 233.0f / 255.0f, 255.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(unifiedShader);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        unsigned int sunColorLocation = glGetUniformLocation(unifiedShader, "sunColor");
        unsigned int isSunLocation = glGetUniformLocation(unifiedShader, "isSun");

        //Sun
        float change = (sin(startTime) + 1.0f) / 2.0f + 0.8f;
        glUniform3f(sunColorLocation, 1.0f, change, 1.0f);
        glUniform1i(isSunLocation, true);

        glBindVertexArray(sunVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUniform1i(isSunLocation, false);

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

        //Waterfall
        glBindVertexArray(waterfallVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentWaterfallTexture);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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

        //Bush
        glBindVertexArray(bushVAO1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bushTexture3);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);


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
    glDeleteBuffers(1, &groundVBO1);
    glDeleteBuffers(1, &waterfallVBO);
    glDeleteVertexArrays(1, &groundVAO1);
    glDeleteVertexArrays(1, &waterfallVAO);
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
