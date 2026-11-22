#include <glad/glad.h>   // MUST be included before GLFW
#include <GLFW/glfw3.h>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColour;
out vec3 ourColour;
void main() {
    gl_Position = vec4(aPos, 1.0);
    ourColour = aColour;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 ourColour;

void main() {
    FragColor = vec4(ourColour, 1.0);  
}
)";

GLuint VAO, VBO, shaderProgram;

void compileShaders() {
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void setupDrawing() {
    float vertices[] = {
        //square                    //yellow
		 0.25f,  0.25f, 0.0f,       1.0f, 1.0f,  0.0f,
         0.75f,  0.25f, 0.0f,       1.0f, 1.0f,  0.0f,
         0.75f,  0.75f, 0.0f,       1.0f, 1.0f,  0.0f,
         0.25f,  0.75f, 0.0f,       1.0f, 1.0f,  0.0f,

		//line segment              //cyan
       -0.5f,  0.0f, 0.0f,          0.0f, 1.0f,  1.0f,
		0.0f,  0.0f, 0.0f,          0.0f, 1.0f,  1.0f,
        0.0f,  -0.5f, 0.0f,         0.0f, 1.0f,  1.0f,
       -0.5f,  -0.5f, 0.0f,         0.0f, 1.0f,  1.0f,
       -0.5f,  -0.25f, 0.0f,        0.0f, 1.0f,  1.0f,
       -0.25f,  -0.25f, 0.0f,       0.0f, 1.0f,  1.0f,

       //triangle                   //RGB
		0.0f, 0.0f, 0.0f,           0.0f, 0.0f,  1.0f,
		1.0f, -1.0f, 0.0f,          1.0f, 0.0f,  0.0f,
        1.0f, 0.0f, 0.0f,           0.0f, 1.0f,  0.0f,

		//triangle outline		    //white
		0.0f, 0.0f, 0.0f,           1.0f, 1.0f,  1.0f,
		1.0f, -1.0f, 0.0f,          1.0f, 1.0f,  1.0f,
		1.0f, 0.0f, 0.0f,           1.0f, 1.0f,  1.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(600, 600, "Red Line", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    compileShaders();
    setupDrawing();


    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDrawArrays(GL_LINE_STRIP, 4, 6);

		glDrawArrays(GL_TRIANGLE_FAN, 10, 3);

        glDrawArrays(GL_LINE_LOOP, 13, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}