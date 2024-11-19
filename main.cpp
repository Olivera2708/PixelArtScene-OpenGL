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

    float ground_vertices[] = {
        -1.0f, -1.0f,                        0.0f, 0.0f,
         1.0f, -1.0f,                        wWidth / 128.0f, 0.0f,
        -1.0f, 128.0f / wHeight - 1.0f,      0.0f, 1.0f,
         1.0f, 128.0f / wHeight - 1.0f,      wWidth / 128.0f, 1.0f
    };

    float waterfall_vertices[] = {
        0.6f, -1.0f,                       0.0f, 0.0f,
        0.6f + 400.0f / wWidth, -1.0f,     1.0f, 0.0f,
        0.6f, 768.0f / wHeight - 1.0f,                        0.0f, 1.0f,
        0.6f + 400.0f / wWidth, 768.0f / wHeight - 1.0f,      1.0f, 1.0f
    };

    unsigned int stride = (2 + 2) * sizeof(float);

    //VAO and VBO
    unsigned int groundVAO, groundVBO;
    groundVAO = createVAO(&groundVBO, ground_vertices, sizeof(ground_vertices), stride);

    unsigned int waterfallVAO, waterfallVBO;
    waterfallVAO = createVAO(&waterfallVBO, waterfall_vertices, sizeof(waterfall_vertices), stride);

    //Textures
    unsigned groundTexture = loadAndSetupTexture("res/Ground.png", unifiedShader, "uTex");
    unsigned waterfallTexture1 = loadAndSetupTexture("res/Waterfall_1.png", unifiedShader, "uTex");
    unsigned waterfallTexture2 = loadAndSetupTexture("res/Waterfall_2.png", unifiedShader, "uTex");

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

        //Ground
        glBindVertexArray(groundVAO);
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
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &waterfallVBO);
    glDeleteVertexArrays(1, &groundVAO);
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