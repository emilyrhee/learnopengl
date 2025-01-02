#include "glad/glad.h" // glad must go b4 glfw
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

void processInput(GLFWwindow *window) {
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

std::string fileToStr(const char* filePath) {
    std::ifstream file(filePath); // Open file!!
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file." << std::endl;
    }

    std::stringstream stream;
    stream << file.rdbuf(); // Read the file content into the stream. You'd use while loop for da long way.

    return stream.str(); // Convert to a string
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

    std::string vertexCode = fileToStr("../shaders/vertex.vert");
    const char* vertexShaderSource = vertexCode.c_str(); 

    std::string colorFromVertexCode = fileToStr("../shaders/colorFromVertex.frag");
    const char* colorFromVertexSrc = colorFromVertexCode.c_str(); 

    std::string uniformColorCode = fileToStr("../shaders/uniformColor.frag");
    const char* uniformColorSrc = uniformColorCode.c_str(); 

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &colorFromVertexSrc, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int fragmentShaderTwo = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShaderTwo, 1, &uniformColorSrc, NULL);
    glCompileShader(fragmentShaderTwo);

    unsigned int shaderProgramTwo = glCreateProgram();
    glAttachShader(shaderProgramTwo, vertexShader);
    glAttachShader(shaderProgramTwo, fragmentShaderTwo);
    glLinkProgram(shaderProgramTwo);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float triangleOne[] = {
       -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // top
        0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
       -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom left
        0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f,  // top
        1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f,  // bottom right
        0.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        // 1, 2, 3  // second triangle
    };

    unsigned int elementBufferObject = createEBO(indices, sizeof(indices));

    unsigned int vertexBufferObject; // Stores raw vertex data (positions, colors, texture coordinates, etc.) in GPU memory.
    unsigned int vertexArrayObject;  // Stores the configuration of how vertex data is interpreted and used.
    handleVertexObjects(
        &vertexBufferObject,
        &vertexArrayObject,
        triangleOne,
        sizeof(triangleOne)
    );

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // position
    glVertexAttribPointer(
        0, 3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void *)0
    );
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject); // u don't need to this in loop unless ur object changes
        glDrawArrays(GL_TRIANGLES, 0, 3);                           // drawin 6 vertices
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glUseProgram(shaderProgramTwo);
        float greenValue = (sin(glfwGetTime()) / 6.0f) + 0.5f; // sin keeps value between 0-1
        int vertexColorLocation = glGetUniformLocation(shaderProgramTwo, "ourColor");
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 3, 3); // drawin 6 vertices

        processInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
