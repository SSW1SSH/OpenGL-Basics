#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint modelLoc, viewLoc, projectionLoc;
glm::mat4 model, view, projection;

float carMovement = 4.0f;
float carAngle = glm::radians(45.0);
float wheelAngle = 0.0;
float rimAngle = 0.0;
int refreshMills = 10;

GLuint shaderProgram;

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main()
{
    gl_Position =  projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 objectColor;
void main()
{
    // Use the uniform color for the fragment
    FragColor = vec4(objectColor, 1.0);  // Assign color with alpha = 1.0 (fully opaque
}
)";



GLuint compileShader(GLenum type, const char* source) {
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	return shader;
}

GLuint createShaderProgram() {
	GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cout << "ERROR::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUseProgram(shaderProgram);

	return shaderProgram;
}

void init() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}
	shaderProgram = createShaderProgram();

	//TODO SETUP CAMERA AND PROJECTION
		projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
		view = glm::lookAt(glm::vec3(15.0f, 45.0f, 45.0f),
		glm::vec3(15.0f, 0.0f, 0.0f),   // Look at the origin
		glm::vec3(0.0f, 1.0f, 0.0f));  // Up direction

	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

/*draws a single rim*/
void drawRim(glm::mat4 rim) {
	//TODO
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(rim)); // set model to the rims

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f); // Red color for rims

	glutWireCone(0.5, 3.0, 20, 20); // Draw a wireframe cone as the rim
}

/*draws a single wheel*/
void drawWheel(glm::mat4 wheel) {
	//TODO
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(wheel));

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f); // Black color for wheels

	glutWireTorus(0.5, 0.6, 20, 40); // Draw a wireframe torus as the wheel

	glm::mat4 rimRotate = glm::rotate(wheel, glm::radians(rimAngle), glm::vec3(0.0f, 0.0f, 1.0f));

	drawRim(rimRotate);
}

/* The main drawing function. */
void DrawCarBody() {
	//TODO  
	glm::mat4 carBody = glm::mat4(1.0f);
	
	carBody = glm::translate(carBody, glm::vec3(carMovement,0.0f, 0.0f));

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carBody));

	GLuint colorLoc = glGetUniformLocation(shaderProgram, "objectColor");
	glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); 

	carBody = glm::rotate(carBody, carAngle, glm::vec3(0.16f, 0.15f, 0.0f));

	glm::mat4 blueSqaure = glm::translate(carBody, glm::vec3(-3.75f, 2.0f, -2.0f)); 
	blueSqaure = glm::scale(blueSqaure, glm::vec3(1.9f, 1.0f, 1.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(blueSqaure));
	glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f);
	glutWireCube(4.0);

	glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); 

	glm::mat4 carFrontBlock = glm::scale(carBody, glm::vec3(3.0f, 2.0f, 4.0f));
	carFrontBlock = glm::translate(carFrontBlock, glm::vec3(0.5f, 0.5f, -0.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carFrontBlock));
	glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); 
	glutSolidCube(1.0);

	

	glm::mat4 carWheelFR = carBody;
	glm::mat4 carWheelFL = glm::translate(carBody,glm::vec3(0.0f,0.0f,-3.5f));



	carWheelFL = glm::rotate(carWheelFL,glm::radians(180.0f),glm::vec3(1.0f,0.0f,0.0f));
	carWheelFR = glm::rotate(carWheelFR, glm::radians(-wheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	carWheelFL = glm::rotate(carWheelFL, glm::radians(-wheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

	drawWheel(carWheelFR);
	drawWheel(carWheelFL);


	glm::mat4 carBackBlock = glm::scale(carBody, glm::vec3(3.0f, 2.0f, 4.0f));
	carBackBlock = glm::translate(carBackBlock, glm::vec3(-3.0f, 0.5f, -0.5f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(carBackBlock));
	glUniform3f(colorLoc, 0.0f, 1.0f, 0.0f); 
	glutSolidCube(1.0);

	glm::mat4 carWheelBR = glm::translate(carBody, glm::vec3(-7.5f, 0.0f, 0.0f));
	glm::mat4 carWheelBL = glm::translate(carBody, glm::vec3(-7.5f, 0.0f, -3.5f));
	carWheelBL = glm::rotate(carWheelBL, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	carWheelBR = glm::rotate(carWheelBR, glm::radians(-wheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	carWheelBL = glm::rotate(carWheelBL, glm::radians(-wheelAngle), glm::vec3(0.0f, 0.0f, 1.0f));

	drawWheel(carWheelBR);
	drawWheel(carWheelBL);


	glUniform3f(colorLoc, 0.0f, 0.0f, 1.0f); 

}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawCarBody();


	//TODO Update car parameters
	glutSwapBuffers();
}

void timer(int value) {
	// TODO complete timer function

	carMovement += 0.05f;
	if (carMovement >= 30.0f) {
		carMovement = 4.0f;
	}
	carAngle += 0.02;

	wheelAngle -= 5.0f;
	rimAngle -= 15.0f;

	glutPostRedisplay(); 
	glutTimerFunc(refreshMills, timer, 10);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(700, 700);
	glutCreateWindow("Test");
	init();
	glutDisplayFunc(display);
	glutTimerFunc(0, timer, 0);
	glutMainLoop();
	return 0;
}
