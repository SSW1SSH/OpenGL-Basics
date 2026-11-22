#include <GL/glew.h>
#include <GL/freeglut.h>
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
     -1,-1,1,  0, 0, 1,
     1,-1, 1,  0, 0, 1,
     1, 1, 1,  0, 0, 1,

	 -1, 1, 1,  0, 0, 1,
     -1,-1,1,  0, 0, 1,
     1, 1, 1,  0, 0, 1,

    // Back face
    -1,-1,-1, 0, 0, -1,
     1,-1,-1, 0, 0, -1,
	 1, 1,-1, 0, 0, -1,  

	 -1,1,-1, 0, 0, -1,
     -1,-1,-1, 0, 0, -1,
      1, 1,-1, 0, 0, -1,

    // Left face
	-1, 1, 1, -1, 0, 0,
	-1, -1, 1, -1, 0, 0,
    -1,-1,-1, -1, 0, 0,

	-1,1,-1, -1, 0, 0,
    -1, 1, 1, -1, 0, 0,
    -1,-1,-1, -1, 0, 0,

    // Right face
     1, 1, 1,  1, 0, 0,
     1, -1, 1,  1, 0, 0,
	 1,-1,-1, 1, 0, 0,

	 1,1,-1,  1, 0, 0,
    1, 1, 1,  1, 0, 0,
    1,-1,-1, 1, 0, 0,

    // Top face
    -1,1,1, 0, 1, 0,
     1,1,1,   0, 1, 0,
	 1,1,-1,  0, 1, 0,

	 -1,1,-1 , 0, 1, 0,
    -1,1,1, 0, 1, 0,
     1,1,-1,  0, 1, 0,

    // Bottom face
    -1,-1,1, 0, -1, 0,
	 1,-1,1, 0, -1, 0,
	 1, -1, -1, 0, -1, 0,

	 -1, -1, -1, 0, -1, 0,
     -1,-1,1, 0, -1, 0,
     1, -1, -1, 0, -1, 0,

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

    gl_Position = modelViewProjectionMatrix * vec4(position, 1.0);

    // Transform the normal using the normal matrix
    fragNormal = normalize(normalMatrix * normal);

    // Pass the transformed position to the fragment shader
    fragPosition = vec3(modelViewMatrix * vec4(position, 1.0));
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
uniform vec3 ambientLightColor;    // Color of the ambient light (l_a)
uniform vec3 diffuseLightColor;    // Color of the diffuse light (l_d)
uniform vec3 specularLightColor;    // Color of the specular light (l_s)

// Uniforms for material properties
uniform vec3 ambientMaterialColor;  // Ambient reflection coefficient (k_a)
uniform vec3 diffuseMaterialColor;  // Diffuse reflection coefficient (k_d)
uniform vec3 specularMaterialColor; // Specular reflection coefficient (k_s)
uniform float shininess;    // Shininess coefficient for specular highlights

void main() {
   // TODO ADD CODE HERE 

vec3 N = normalize(fragNormal); // Normal vector
vec3 L = normalize(lightVec - fragPosition); // Light direction vector
vec3 V = normalize(viewPos - fragPosition); // View direction vector
vec3 R = reflect(-L, N); // Reflection vector

vec3 ambient = ambientLightColor * ambientMaterialColor; // Ambient component

float diff = max(dot(N, L), 0.0); // Diffuse factor
vec3 diffuse = diffuseLightColor * (diff * diffuseMaterialColor); // Diffuse component

float spec = pow(max(dot(R, V), 0.0), shininess); // Specular factor
vec3 specular = specularLightColor * (spec * specularMaterialColor); // Specular component


float distance = length(lightVec - fragPosition); // Distance from light to fragment
float constant = 1.0; // Constant attenuation factor
float linear = 0.09; // Linear attenuation factor
float quadratic = 0.032; // Quadratic attenuation factor
float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance); // Attenuation factor


vec3 result = (ambient + diffuse + specular) * attenuation; // Final color calculation

finalColor = vec4(result, 1.0); // Use the calculated color
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

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 800.0f, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(5.0f, 5.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 model = glm::mat4(1.0f);

    float time = (GLfloat)glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
    model = glm::rotate(model, time, glm::vec3(1.0f, 1.0f, 1.0f));


    glm::mat4 modelView = view * model;
    glm::mat4 mvp = projection * modelView;

    glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelView)));

    GLint mvpLoc = glGetUniformLocation(shaderProgram, "modelViewProjectionMatrix");
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

    GLint mvLoc = glGetUniformLocation(shaderProgram, "modelViewMatrix");
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(modelView));

    GLint normalMatLoc = glGetUniformLocation(shaderProgram, "normalMatrix");
    glUniformMatrix3fv(normalMatLoc, 1, GL_FALSE, glm::value_ptr(normalMat));

    
    // TODO Define light and object uniform variables
// TODO Define light and object uniform variables
    // 1. Define the World Space light position
    glm::vec3 worldLightPos(2.0f, -0.5f, -3.0f);

    // 2. Transform the light position into View Space
    glm::vec3 viewLightPos = glm::vec3(view * glm::vec4(worldLightPos, 1.0f));
    glUniform3fv(glGetUniformLocation(shaderProgram, "lightVec"), 1, glm::value_ptr(viewLightPos));

    glm::vec3 viewSpaceCameraPos(0.0f, 0.0f, 0.0f);
    glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(viewSpaceCameraPos));

    glm::vec3 ambientLightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 diffuseLightColor(0.0f, 0.0f, 1.0f);
    glm::vec3 specularLightColor(0.0f, 1.0f, 0.0f);


    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientLightColor"), 1, glm::value_ptr(ambientLightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "diffuseLightColor"), 1, glm::value_ptr(diffuseLightColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "specularLightColor"), 1, glm::value_ptr(specularLightColor));


    glm::vec3 ambientMaterialColor(0.1f, 0.1f, 0.1f);
    glm::vec3 diffuseMaterialColor(1.0f, 1.0f, 1.0f);
    glm::vec3 specularMaterialColor(1.0f, 1.0f, 1.0f);
    GLfloat shininess = 32.0f;

    glUniform3fv(glGetUniformLocation(shaderProgram, "ambientMaterialColor"), 1, glm::value_ptr(ambientMaterialColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "diffuseMaterialColor"), 1, glm::value_ptr(diffuseMaterialColor));
    glUniform3fv(glGetUniformLocation(shaderProgram, "specularMaterialColor"), 1, glm::value_ptr(specularMaterialColor));
    glUniform1f(glGetUniformLocation(shaderProgram, "shininess"), shininess);
    
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
