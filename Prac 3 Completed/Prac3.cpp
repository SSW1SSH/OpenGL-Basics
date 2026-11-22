#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint VAO, VBO, CBO;
GLuint shaderProgram;
GLint gCompositeMatrix;

int indexCount = 0;

// Vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec4 aColor;
uniform mat4 compositeMatrix;
out vec4 vertexColor;
void main() {
    gl_Position = compositeMatrix * vec4(aPos, 0.0, 1.0);
    vertexColor = aColor;
}
)";

// Fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
in vec4 vertexColor;
out vec4 FragColor;
void main() {
    FragColor = vertexColor;
}
)";

// Shader compilation helper
GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return shader;
}

void initShaders() {
    GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, log);
        std::cerr << "Program link error:\n" << log << std::endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);

    gCompositeMatrix = glGetUniformLocation(shaderProgram, "compositeMatrix");
}

void CreateVertexBuffer() {
    glm::vec2 Vertices[4] = {
        glm::vec2(-0.25f, -0.25f),
        glm::vec2(0.25f, -0.25f),
        glm::vec2(0.25f,  0.25f),
        glm::vec2(-0.25f,  0.25f)
    };
    glm::vec4 Colors[4] = {
        glm::vec4(1,1,1,1),
        glm::vec4(1,1,1,1),
        glm::vec4(1,1,1,1),
        glm::vec4(1,1,1,1)
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    // TODO: setup your vertex attribute pointers for VBO
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &CBO);
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Colors), Colors, GL_STATIC_DRAW);

    // TODO: setup your vertex attribute pointers for CBO
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

}

//helper function to draw objects with a given transformation matrix and color
void drawObjects(glm::mat4 matrix, glm::vec4 color[4]) {
    glBindVertexArray(VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 4, color, GL_STATIC_DRAW);
    glUniformMatrix4fv(gCompositeMatrix, 1, GL_FALSE, glm::value_ptr(matrix));
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}
float theta = 0.0f;

void timeRotate(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000, timeRotate, 0);
	theta += 10.0f;
}



void RenderSceneCB() {
	glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glLineWidth(4.0f);

    glm::mat4 initMatrix = glm::mat4(1.0f);
    initMatrix = glm::rotate(initMatrix, glm::radians(0.0f + theta), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec4 BlueColor[4] = {
     glm::vec4(0,0,1,1),
     glm::vec4(0,0,1,1),
     glm::vec4(0,0,1,1),
     glm::vec4(0,0,1,1)
    };

	drawObjects(initMatrix, BlueColor);
    //rotated square
	glm::mat4 RotateMatrix = glm::rotate(initMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::vec4 RedColor[4] = {
     glm::vec4(1,0,0,1),
     glm::vec4(1,0,0,1),
     glm::vec4(1,0,0,1),
     glm::vec4(1,0,0,1)
    };
	drawObjects(RotateMatrix, RedColor);

	//rotated and scaled square
	glm::mat4 smallsquareMatrix = glm::rotate(initMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	smallsquareMatrix = glm::scale(smallsquareMatrix, glm::vec3(0.5f, 1.0f, 1.0f));

    glm::vec4 BlackColor[4] = {
     glm::vec4(0,0,0,1),
     glm::vec4(0,0,0,1),
     glm::vec4(0,0,0,1),
     glm::vec4(0,0,0,1)
    };
	drawObjects(smallsquareMatrix, BlackColor);

	//rotated and scaled and translated square
    glm::mat4 translatedMatrix = glm::rotate(initMatrix, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    translatedMatrix = glm::scale(translatedMatrix, glm::vec3(0.5f, 1.0f, 1.0f));
	translatedMatrix = glm::translate(translatedMatrix, glm::vec3(0.5f, 0.5f, 0.0f));

    glm::vec4 GreenColor[4] = {
     glm::vec4(0,1,0,1),
     glm::vec4(0,1,0,1),
     glm::vec4(0,1,0,1),
     glm::vec4(0,1,0,1)
    };
	drawObjects(translatedMatrix, GreenColor);

    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 100);
    glutCreateWindow("Single Blue Square");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
        return 1;
    }
    CreateVertexBuffer();
    initShaders();

    glutDisplayFunc(RenderSceneCB);
    glutTimerFunc(0, timeRotate, 0);

    glutMainLoop();

    return 0;
}
