#define STB_IMAGE_IMPLEMENTATION
#include "glad/glad.h" // glad must go b4 glfw
#include "include/shader.hpp"
#include "include/texture.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cmath>
#include <filesystem>

void setGlfwVersion(int major, int minor) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void glfw_error_callback(int error, const char *description) {
    std::cout << "GLFW Error (" << error << "): " << description << std::endl;
}

void processKeyboardInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void checkShaderError(unsigned int shader) {
    int success;
    char infoLog[512];

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }
}

// For reusing vertices when making triangles.
uint createEBO(uint *indices, size_t size) {
    uint EBO;

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        size,
        indices,
        GL_STATIC_DRAW);

    return EBO;
}

void handleVertexObjects(
    unsigned int *VAO,
    unsigned int *VBO,
    const float *vertices,
    size_t sizeOfVertices
) {
    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);             // Sets the currently active VAO
    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO); // Associates the VBO with the current VAO
    glBufferData(GL_ARRAY_BUFFER, sizeOfVertices, vertices, GL_STATIC_DRAW);
}

int main() {
    glfwInit();
    setGlfwVersion(3, 3);
    glfwSetErrorCallback(glfw_error_callback);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // For displaying transparency properly
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader("../shaders/vertex.vert", "../shaders/colorFromVertex.frag");
    Shader shaderTwo("../shaders/vertex.vert", "../shaders/uniformColor.frag");
    shader.use();
    shader.setUniformInt("texture1", 0);
    shader.setUniformInt("texture2", 1);
    float alpha = 0.0f;
    shader.setUniformFloat("alpha", alpha);

    float triangles[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // top left
    };

    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    unsigned int EBO = createEBO(indices, sizeof(indices));
    unsigned int vertexBufferObject; // Stores raw vertex data (positions, colors, texture coordinates, etc.) in GPU memory.
    unsigned int vertexArrayObject;  // Stores the configuration of how vertex data is interpreted and used.
    handleVertexObjects(
        &vertexBufferObject,
        &vertexArrayObject,
        triangles,
        sizeof(triangles)
    );

    // position
    glVertexAttribPointer(
        0, 3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void *)0
    );
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(
        1, 3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);
    // texture
    glVertexAttribPointer(
        2, 2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)(6 * sizeof(float))
    );
    glEnableVertexAttribArray(2);

    float textureCoords[] = {
        0.0f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f
    };

    Texture slop("../textures/slop.jpg");
    Texture tomato("../textures/tomato.png");
    tomato.setFilter(GL_NEAREST);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        slop.bind();

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            alpha = alpha + 0.01f;
            shader.setUniformFloat("alpha", alpha);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            alpha = alpha - 0.01f;
            shader.setUniformFloat("alpha", alpha);
        }
        
        glActiveTexture(GL_TEXTURE1);
        tomato.bind();
        
        shader.use();
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // u don't need to this in loop unless ur object changes
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        processKeyboardInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}