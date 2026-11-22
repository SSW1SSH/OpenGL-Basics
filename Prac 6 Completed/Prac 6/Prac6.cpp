#include <glew.h>
#include <glut.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Shader programs
GLuint shaderProgram;
GLuint VAO, VBO;
GLint modelLoc, viewLoc, projectionLoc, normalMatrixLoc;
glm::mat4 model, view, projection;

const GLfloat cubeVerticesWithNormals[] = {
    // Positions, Normals
    // Front face
    // Back face
    // Left face
    // Right face
    // Top face
    // Bottom face
};

// Vertex Shader Source
const char* vertexShaderSource = R"(
#version 330 core

layout(location = 0) in vec3 position; // Vertex position
layout(location = 1) in vec3 normal;   // Normal vector

out vec3 fragNormal;    // Pass the transformed normal to the fragment shader
out vec3 fragPosition;  // Pass the transformed position to the fragment shader

uniform mat4 modelViewProjectionMatrix; // MVP matrix for position
uniform mat4 modelViewMatrix;           // Model-View matrix
uniform mat3 normalMatrix;              // Inverse transpose of the Model-View matrix (normal matrix)

void main() {
    // Transform the position using the MVP matrix
    //TODO ADD CODE HERE
}

)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
#version 330 core

in vec3 fragNormal;    // Interpolated normal from the vertex shader
in vec3 fragPosition;  // Interpolated position from the vertex shader

out vec4 finalColor;   // Final color of the fragment

// Uniforms for light properties
uniform vec3 lightVec;      // Position of the light source
uniform vec3 viewPos;       // Position of the camera/viewer
uniform vec3 ambinentLightColor;    // Color of the ambient light (l_a)
uniform vec3 diffuseLightColor;    // Color of the diffuse light (l_d)
uniform vec3 specularLightColor;    // Color of the specular light (l_s)

// Uniforms for material properties
uniform vec3 ambientMaterialColor;  // Ambient reflection coefficient (k_a)
uniform vec3 diffuseMaterialColor;  // Diffuse reflection coefficient (k_d)
uniform vec3 specularMaterialColor; // Specular reflection coefficient (k_s)
uniform float shininess;    // Shininess coefficient for specular highlights

void main() {
    //TODO ADD CODE HERE 
}

)";

// Shader Compilation Utility
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error compiling shader: " << infoLog << std::endl;
    }
    return shader;
}

// Link Shader Program
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Error linking shader program: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void init() {
    glewExperimental = GL_TRUE;
    glewInit();

    shaderProgram = createShaderProgram();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerticesWithNormals), cubeVerticesWithNormals, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the shader program
    glUseProgram(shaderProgram);
    
    // TODO Define model and view matrices and send them to the shader
    
    // TODO Define light and object uniform variables
    
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Cube with Normals (GLUT + GLEW)");

    glutDisplayFunc(display);
    glutIdleFunc(display);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    init();

    glutMainLoop();

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    return 0;
}
