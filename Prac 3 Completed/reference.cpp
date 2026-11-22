
// OPENGL TRANSFORMATION DEMONSTRATION

//
// Concepts demonstrated:
//
// 1. GLFW
//      - Creates the OpenGL window.
//      - Handles keyboard input.
//      - Handles the application loop.
//
// 2. GLAD
//      - Loads OpenGL function pointers.
//
// 3. GLM
//      - Performs vector and matrix mathematics.
//
// 4. Homogeneous coordinates
//      - Vertices are represented as vec4(x, y, z, 1.0).
// 
//
//
// 5. Model-View-Projection (MVP)
//      - projection * view * model * vertex
//
// 6. Rotation about an arbitrary point
//
//      M_rotation = T(C) * R(theta) * T(-C)
//
// 7. Reflection about an arbitrary line
//
//      M_reflection = T(C) * R(theta) * F * R(-theta) * T(-C)
//
//      where F reflects about the x-axis.
//
// 8. Matrix composition
//      - Transformations are applied right-to-left.
//
// 9. Interactive animation
//      - The rotation angle changes continuously.
//      - Keyboard controls can change the animation.
//


// 1. INCLUDE GLAD
// GLAD loads the OpenGL functions after GLFW has created the OpenGL context.
// IMPORTANT:
// GLAD should generally be included before using modern OpenGL functions.

#include <glad/glad.h>


// 2. INCLUDE GLFW

//
// GLFW is responsible for:
//
// - Creating the operating-system window.
// - Creating the OpenGL context.
// - Processing keyboard/mouse input.
// - Handling the application event loop.
//

#include <GLFW/glfw3.h>




// 3. INCLUDE GLM
// 
// GLM = OpenGL Mathematics.
//
// It provides:
//
// - vec2
// - vec3
// - vec4
// - mat3
// - mat4
// - translate()
// - rotate()
// - scale()
// - inverse()
// - transpose()
// - radians()
//
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// 4. STANDARD C++ LIBRARIES
//
// iostream  -> console output
// string    -> string manipulation
// cmath     -> mathematical functions such as atan()
//
#include <iostream>
#include <string>
#include <cmath>


// 5. WINDOW DIMENSIONS
// These determine the initial size of the GLFW window.

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;


// 6. GLOBAL ANIMATION VARIABLES
// rotationAngle:
//     Current rotation angle in degrees.
// rotationSpeed:
//     Number of degrees rotated per second.
// animationEnabled:
//     Determines whether the rotation is active.
float rotationAngle = 0.0f;
float rotationSpeed = 45.0f;
bool animationEnabled = true;


// 7. SHADER PROGRAM
// We will create one shader program.
// The shader receives:
//     uMVP
// and transforms every vertex using:
//     gl_Position = uMVP * vec4(aPos, 1.0);

GLuint shaderProgram = 0;


// 8. VAO AND VBO
// VAO = Vertex Array Object
// It remembers how vertex data is configured.
// VBO = Vertex Buffer Object
// It stores vertex data in GPU memory.

GLuint VAO = 0;
GLuint VBO = 0;


// 9. SHADER SOURCE CODE

// VERTEX SHADER
// The vertex shader runs once for every vertex.

const char* vertexShaderSource = R"(
#version 330 core

// Vertex attribute.

// location = 0 means that the CPU-side vertex data will be connected to
// this variable through:

//     glVertexAttribPointer(0, ...)

layout(location = 0) in vec3 aPos;


// MVP transformation matrix.
// The CPU calculates this matrix using GLM and sends it to the GPU.

uniform mat4 uMVP;


void main()
{
    // Convert the 3-D position into homogeneous coordinates.
    
    // aPos:
    
    //     (x, y, z)
    
    // becomes:
    
    //     (x, y, z, 1)
    
    // The final coordinate is 1 because aPos represents a POINT rather
    // than a direction.
    
    ----
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";


// FRAGMENT SHADER

// The fragment shader determines the colour of each rendered fragment.

const char* fragmentShaderSource = R"(
#version 330 core


// Colour supplied by the CPU.

// vec4 means:

//     red
//     green
//     blue
//     alpha
//
uniform vec4 uColor;


void main()
{
    // Assign the requested colour to the current fragment.
    FragColor = uColor;
}
)";


// 10. SHADER COMPILATION FUNCTION

// This function:

// 1. Creates a shader.
// 2. Loads the source code.
// 3. Compiles it.
// 4. Checks whether compilation succeeded.
// 5. Returns the shader ID.
//

GLuint compileShader(GLenum type, const char* source)
{
    // Create a shader object.
    
    // type can be:
    
    // GL_VERTEX_SHADER
    // GL_FRAGMENT_SHADER
    
    GLuint shader = glCreateShader(type);


    // Attach the source code to the shader.
    glShaderSource(shader, 1, &source, nullptr);


    // Ask OpenGL to compile the shader.
    glCompileShader(shader);


    // Check whether compilation succeeded.
    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );


    // If compilation failed, retrieve the error message.
    if (!success)
    {
        char infoLog[1024];

        glGetShaderInfoLog(
            shader,
            1024,
            nullptr,
            infoLog
        );

        std::cerr
            << "ERROR::SHADER::COMPILATION_FAILED\n"
            << infoLog
            << std::endl;
    }


    // Return the compiled shader ID.
    return shader;
}


// 11. CREATE SHADER PROGRAM

// This function:

//     vertex shader
//          +
//     fragment shader
//          ↓
//     shader program


GLuint createShaderProgram()
{
    // Compile the vertex shader.
    GLuint vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexShaderSource
        );


    // Compile the fragment shader.
    GLuint fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentShaderSource
        );


    // Create an OpenGL shader program.
    GLuint program =
        glCreateProgram();


    // Attach both shaders to the program.
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);


    // Link the shaders together.
    //
    // After linking, the vertex and fragment shaders become one executable
    // GPU program.
    glLinkProgram(program);


    // Check whether linking succeeded.
    int success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );


    // Report linking errors.
    if (!success)
    {
        char infoLog[1024];

        glGetProgramInfoLog(
            program,
            1024,
            nullptr,
            infoLog
        );

        std::cerr
            << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
            << infoLog
            << std::endl;
    }


    // The individual shader objects are no longer needed after linking.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Return the final shader program.
    return program;
}


// 12. GLFW ERROR CALLBACK

// GLFW can report errors through this function.

void glfwErrorCallback(
    int error,
    const char* description
)
{
    std::cerr
        << "GLFW ERROR "
        << error
        << ": "
        << description
        << std::endl;
}


// 13. FRAMEBUFFER RESIZE CALLBACK

// If the user resizes the window, OpenGL needs to know the new viewport.

void framebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height
)
{
    // Tell OpenGL that the drawable area has changed.
    glViewport(
        0,
        0,
        width,
        height
    );
}


// 14. KEYBOARD INPUT

// ESC     -> close program
// SPACE   -> pause/resume animation
// UP      -> increase rotation speed
// DOWN    -> decrease rotation speed
// R       -> reset angle

void processInput(GLFWwindow* window)
{
    // Escape closes the application.
    if (
        glfwGetKey(window, GLFW_KEY_ESCAPE)
        == GLFW_PRESS
        )
    {
        glfwSetWindowShouldClose(
            window,
            true
        );
    }


    // SPACE toggling requires edge detection; otherwise holding the key
    // would toggle continuously.
    //
    // A simple static variable remembers the previous state.
    static bool previousSpaceState = false;

    bool currentSpaceState =
        glfwGetKey(window, GLFW_KEY_SPACE)
        == GLFW_PRESS;


    if (
        currentSpaceState
        && !previousSpaceState
        )
    {
        animationEnabled =
            !animationEnabled;
    }


    previousSpaceState =
        currentSpaceState;


    // Increase rotation speed.
    if (
        glfwGetKey(window, GLFW_KEY_UP)
        == GLFW_PRESS
        )
    {
        rotationSpeed += 30.0f;
    }


    // Decrease rotation speed.
    if (
        glfwGetKey(window, GLFW_KEY_DOWN)
        == GLFW_PRESS
        )
    {
        rotationSpeed -= 30.0f;

        // Prevent negative speed.
        if (rotationSpeed < 0.0f)
        {
            rotationSpeed = 0.0f;
        }
    }


    // Reset rotation angle.
    if (
        glfwGetKey(window, GLFW_KEY_R)
        == GLFW_PRESS
        )
    {
        rotationAngle = 0.0f;
    }
}


// 15. CREATE GEOMETRY
//
// Our basic object is a triangle.
//
// The triangle is intentionally NOT centred at the origin.
//
// This makes the arbitrary-pivot rotation easier to see.
//
void createGeometry()
{
    // Triangle vertices.
    //
    // All coordinates are in object space.
    //
    // The z-coordinate is 0 because this demonstration is essentially 2-D.
    //
    float vertices[] =
    {
        // x       y       z

        -0.20f, -0.20f, 0.0f,
         0.30f, -0.20f, 0.0f,
         0.00f,  0.35f, 0.0f
    };


    // Create the Vertex Array Object.
    glGenVertexArrays(
        1,
        &VAO
    );


    // Create the Vertex Buffer Object.
    glGenBuffers(
        1,
        &VBO
    );


    // Activate the VAO.
    glBindVertexArray(VAO);


    // Activate the VBO.
    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO
    );


    // Copy vertex data from CPU memory to GPU memory.
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // Tell OpenGL how the vertex data is structured.
    //
    // Each vertex contains:
    //
    //     x
    //     y
    //     z
    //
    // Therefore:
    //
    //     3 values per vertex
    //
    // GL_FLOAT:
    //     each value is a float.
    //
    // GL_FALSE:
    //     no normalisation.
    //
    // 3 * sizeof(float):
    //     distance from one vertex to the next.
    //
    // (void*)0:
    //     position begins at the beginning of each vertex.
    //
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );


    // Enable vertex attribute location 0.
    glEnableVertexAttribArray(0);


    // Unbind VBO.
    glBindBuffer(
        GL_ARRAY_BUFFER,
        0
    );


    // Unbind VAO.
    glBindVertexArray(0);
}


// 16. CREATE A MODEL MATRIX FOR ROTATION ABOUT AN ARBITRARY POINT
//
// Mathematics:
//
//     M = T(C) * R(theta) * T(-C)
//
//
//
// Right-to-left interpretation:
//
//     vertex
//       ↓
//     T(-C)
//       ↓
//     R(theta)
//       ↓
//     T(C)
//       ↓
//     transformed vertex
//

glm::mat4 createArbitraryPointRotation(
    float cx,
    float cy,
    float angleDegrees
)
{
    // Convert degrees into radians because GLM's trigonometric transformations
    // use radians.
    float angleRadians =
        glm::radians(angleDegrees);


    // STEP 1
    //
    // Move the rotation centre C to the origin.
    //
    // If C = (cx, cy), then:
    //
    //     T(-C) = T(-cx, -cy)
    //
    glm::mat4 T_toOrigin =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                -cx,
                -cy,
                0.0f
            )
        );


    // STEP 2
    //
    // Rotate around the origin.
    //
    // The z-axis is the axis perpendicular to our 2-D xy plane.
    //
    glm::mat4 R =
        glm::rotate(
            glm::mat4(1.0f),
            angleRadians,
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


    // STEP 3
    //
    // Move the rotation centre back to its original location.
    //
    //     T(C) = T(cx, cy)
    //
    glm::mat4 T_back =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                cx,
                cy,
                0.0f
            )
        );


    // Compose the transformations.
    //
    // IMPORTANT:
    //
    // Matrix multiplication is applied to the vertex from RIGHT to LEFT.
    //
    // Therefore:
    //
    //     T_back * R * T_toOrigin
    //
    // means:
    //
    //     first T_toOrigin
    //     then R
    //     then T_back
    //
    glm::mat4 result =
        T_back *
        R *
        T_toOrigin;


    // Return the final transformation matrix.
    return result;
}


// 17. CREATE REFLECTION ABOUT AN ARBITRARY LINE
//
// Suppose the reflection line:
//
//     - passes through point C = (cx, cy)
//     - has orientation angle theta
//
// We perform:
//
//     T(-C)
//         ↓
//     R(-theta)
//         ↓
//     Reflection about x-axis
//         ↓
//     R(theta)
//         ↓
//     T(C)
//
// Therefore:
//
//     M = T(C) * R(theta) * F * R(-theta) * T(-C)
//
//
//
// This is equivalent to:
//
//     T_Tr^{-1} * T_R^T * T_Refl * T_R * T_Tr
//
// when:
//
//     T_R = R(-theta)
//
// and:
//
//     T_R^T = R(theta)
//

glm::mat4 createArbitraryLineReflection(
    float cx,
    float cy,
    float lineAngleDegrees
)
{
    // Convert the line angle from degrees to radians.
    float theta =
        glm::radians(
            lineAngleDegrees
        );


    // STEP 1
    //
    // Translate the selected point on the reflection line to the origin.
    //
    //     T(-C)
    //
    glm::mat4 T_toOrigin =
        glm::translate(
            glm::mat4(1.0f),
            glm::vec3(
                -cx,
                -cy,
                0.0f
            )
        );


    // STEP 2
    //
    // Rotate the coordinate system so that the arbitrary line becomes
    // the x-axis.
    //
    // If the line has angle theta, we rotate by -theta.
    //
    //     R(-theta)
    //
    glm::mat4 R_align =
        glm::rotate(
            glm::mat4(1.0f),
            -theta,
            glm::vec3(
                0.0f,
                0.0f,
                1.0f
            )
        );


    // STEP 3
    //
    // Reflection about the x-axis.
    //
    // The ordinary x-axis reflection is:
    //
    //     x' = x
    //     y' = -y
    //
    // Therefore:
    //
    //     | 1   0   0   0 |
    //     | 0  -1   0   0 |
    //     | 0   0   1   0 |
    //     | 0   0   0   1 |
    //
    glm::mat4 F =
        glm::mat4(1.0f);

    F[1][1] = -1.0f;


    // STEP 4
    //
    // Undo the rotation.
    //
    // For a pure rotation:
    //
    //     R^-1 = R^T
    //
    // Therefore we can use transpose(R_align).
    //
    glm::mat4 R_back =
        glm::transpose(
            R_align
        );


    // STEP 5
    //
    // Undo the original translation.
    //
    glm::mat4 T_back =
        glm::inverse(
            T_toOrigin
        );


    // FINAL MATRIX
    //
    // Right-to-left execution:
    //
    //     vertex
    //       ↓
    //     T(-C)
    //       ↓
    //     R(-theta)
    //       ↓
    //     F
    //       ↓
    //     R(theta)
    //       ↓
    //     T(C)
    //
    glm::mat4 result =
        T_back *
        R_back *
        F *
        R_align *
        T_toOrigin;


    return result;
}


// 18. CREATE A SIMPLE MVP MATRIX

// Because our scene is 2-D, we use an orthographic projection.
//
// Orthographic projection means that objects do not become smaller when they
// move farther away from the camera.
//
// This is ideal for visualising geometric transformations.
//
glm::mat4 createProjection()
{
    // left   = -2.0
    // right  =  2.0
    // bottom = -1.5
    // top    =  1.5
    //
    // z range is -1 to 1.

    return glm::ortho(
        -2.0f,
        2.0f,
        -1.5f,
        1.5f,
        -1.0f,
        1.0f
    );
}


// 19. DRAW TRIANGLE

// model:

//     Transformation applied to the triangle.

// colour:

//     Colour of the triangle.

void drawTriangle(
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection,
    const glm::vec4& colour
)
{
    // Construct the complete Model-View-Projection transformation.
    
    //     MVP = Projection * View * Model
    //
    glm::mat4 MVP =
        projection *
        view *
        model;


    // Tell OpenGL which shader program should be active.
    glUseProgram(shaderProgram);


    // Find the location of the uMVP uniform inside the shader.
    GLint mvpLocation =
        glGetUniformLocation(
            shaderProgram,
            "uMVP"
        );


    // Send the 4x4 matrix to the GPU.
    
    // GL_FALSE:
    //     Do not transpose the matrix because GLM already provides the
    //     matrix in the layout expected here.
  
    // glm::value_ptr(MVP):
    //     Gives OpenGL a pointer to the matrix's underlying float data.
   
    glUniformMatrix4fv(
        mvpLocation,
        1,
        GL_FALSE,
        glm::value_ptr(MVP)
    );


    // Find the colour uniform.
    GLint colorLocation =
        glGetUniformLocation(
            shaderProgram,
            "uColor"
        );


    // Send the colour to the fragment shader.
    glUniform4fv(
        colorLocation,
        1,
        glm::value_ptr(colour)
    );



    // Activate the triangle's VAO.

    glBindVertexArray(VAO);



    // Draw three vertices as one triangle.

    glDrawArrays(
        GL_TRIANGLES,
        0,
        3
    );



    // Unbind VAO.

    glBindVertexArray(0);
}



// 20. DRAW AXES AND REFLECTION LINE

//
// We use immediate-style temporary geometry through a small dynamic VBO.
//
// This is purely for educational visualisation.
//
void drawLine(
    const glm::vec3& p1,
    const glm::vec3& p2,
    const glm::vec4& colour,
    const glm::mat4& view,
    const glm::mat4& projection
)
{

    // Two points form one line.

    float vertices[] =
    {
        p1.x, p1.y, p1.z,
        p2.x, p2.y, p2.z
    };



    // Create temporary VAO and VBO.

    GLuint lineVAO;
    GLuint lineVBO;


    glGenVertexArrays(
        1,
        &lineVAO
    );

    glGenBuffers(
        1,
        &lineVBO
    );



    // Bind VAO.

    glBindVertexArray(lineVAO);



    // Upload line data.

    glBindBuffer(
        GL_ARRAY_BUFFER,
        lineVBO
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_DYNAMIC_DRAW
    );



    // Define vertex layout.

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );


    glEnableVertexAttribArray(0);



    // Because the line itself already contains world coordinates, we use
    // identity as the model matrix.

    glm::mat4 model =
        glm::mat4(1.0f);


    glm::mat4 MVP =
        projection *
        view *
        model;



    // Activate shader.

    glUseProgram(shaderProgram);



    // Send MVP matrix.

    GLint mvpLocation =
        glGetUniformLocation(
            shaderProgram,
            "uMVP"
        );

    glUniformMatrix4fv(
        mvpLocation,
        1,
        GL_FALSE,
        glm::value_ptr(MVP)
    );



    // Send colour.

    GLint colorLocation =
        glGetUniformLocation(
            shaderProgram,
            "uColor"
        );

    glUniform4fv(
        colorLocation,
        1,
        glm::value_ptr(colour)
    );



    // Make the line visually thicker.

    glLineWidth(3.0f);



    // Draw two vertices as a line.

    glDrawArrays(
        GL_LINES,
        0,
        2
    );



    // Cleanup temporary objects.

    glDeleteBuffers(
        1,
        &lineVBO
    );

    glDeleteVertexArrays(
        1,
        &lineVAO
    );


    glBindVertexArray(0);
}



// 21. MAIN FUNCTION


int main()
{

    // GLFW INITIALISATION



    // Install the GLFW error callback.

    glfwSetErrorCallback(
        glfwErrorCallback
    );



    // Initialise GLFW.
    //
    // If this fails, there is no point continuing.

    if (!glfwInit())
    {
        std::cerr
            << "Failed to initialise GLFW."
            << std::endl;

        return -1;
    }



    // REQUEST OPENGL 3.3 CORE PROFILE



    // Tell GLFW which OpenGL version we want.
    //
    // Major = 3
    // Minor = 3
    //

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );



    // Request the modern Core Profile.
    //
    // Core Profile removes many old fixed-function OpenGL features and forces
    // us to use modern OpenGL concepts such as shaders and VAOs.

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );



    // macOS compatibility requirement.
    //
    // It does not hurt to include this conditionally.

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
#endif



    // CREATE WINDOW


    GLFWwindow* window =
        glfwCreateWindow(
            SCR_WIDTH,
            SCR_HEIGHT,
            "OpenGL Transformation Laboratory",
            nullptr,
            nullptr
        );



    // Verify that window creation succeeded.

    if (!window)
    {
        std::cerr
            << "Failed to create GLFW window."
            << std::endl;

        glfwTerminate();

        return -1;
    }



    // Make the OpenGL context associated with this window current.
    //
    // OpenGL calls now affect this window/context.

    glfwMakeContextCurrent(window);



    // Register the framebuffer resize callback.

    glfwSetFramebufferSizeCallback(
        window,
        framebufferSizeCallback
    );



    // LOAD OPENGL USING GLAD



    // GLAD must be initialised AFTER an OpenGL context exists.
    //
    // GLFW created the context above.

    if (!gladLoadGLLoader(
        (GLADloadproc)glfwGetProcAddress
    ))
    {
        std::cerr
            << "Failed to initialise GLAD."
            << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }



    // Print OpenGL information.
    //
    // This is extremely useful when debugging OpenGL installations.

    std::cout
        << "OpenGL Version: "
        << glGetString(GL_VERSION)
        << std::endl;

    std::cout
        << "GLSL Version: "
        << glGetString(GL_SHADING_LANGUAGE_VERSION)
        << std::endl;



    // CREATE SHADERS


    shaderProgram =
        createShaderProgram();



    // CREATE GEOMETRY


    createGeometry();



    // OPENGL STATE



    // Enable alpha blending.
    //
    // This allows transparent colours if we choose to use them.

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );



    // Initial viewport.

    glViewport(
        0,
        0,
        SCR_WIDTH,
        SCR_HEIGHT
    );



    // PROJECTION MATRIX



    // Because our demonstration is 2-D, use orthographic projection.

    glm::mat4 projection =
        createProjection();



    // VIEW MATRIX



    // We do not need a complicated camera for this demonstration.
    //
    // Identity means:
    //
    //     camera coordinates = world coordinates
    //

    glm::mat4 view =
        glm::mat4(1.0f);



    // ROTATION PIVOT



    // This is the point around which the rotating triangle will rotate.
    //
    // It is deliberately NOT the origin.
    //
    // Therefore we demonstrate:
    //
    //     rotation about an arbitrary point.

    const float pivotX = 0.75f;
    const float pivotY = 0.45f;



    // REFLECTION LINE



    // Choose a point through which the reflection line passes.

    const float reflectionX = -0.65f;
    const float reflectionY = -0.10f;



    // Orientation of the reflection line.
    //
    // 25 degrees means the line is tilted 25 degrees relative to the
    // positive x-axis.

    const float reflectionAngle =
        25.0f;



    // TIME MANAGEMENT



    // Used to calculate frame-to-frame elapsed time.

    float lastFrameTime =
        static_cast<float>(
            glfwGetTime()
            );



    // MAIN RENDER LOOP


    while (
        !glfwWindowShouldClose(window)
        )
    {

        // CALCULATE DELTA TIME



        // Get current time.

        float currentTime =
            static_cast<float>(
                glfwGetTime()
                );



        // Difference between current frame and previous frame.
        //
        // This makes animation frame-rate independent.

        float deltaTime =
            currentTime -
            lastFrameTime;



        // Store current time for the next frame.

        lastFrameTime =
            currentTime;



        // PROCESS INPUT


        processInput(window);



        // UPDATE ROTATION


        if (animationEnabled)
        {
            
            // Increase angle according to elapsed time.
            //
            // Example:
            //
            // rotationSpeed = 45 degrees/second
            //
            // deltaTime = 0.016 seconds
            //
            // angle change ≈ 0.72 degrees.
            
            rotationAngle +=
                rotationSpeed *
                deltaTime;


            
            // Keep the angle within 0-360 degrees.
            
            if (rotationAngle >= 360.0f)
            {
                rotationAngle -= 360.0f;
            }
        }



        // CLEAR SCREEN



        // Clear the colour buffer.

        glClearColor(
            0.05f,
            0.05f,
            0.08f,
            1.0f
        );


        glClear(
            GL_COLOR_BUFFER_BIT
        );



        // DRAW WORLD X-AXIS


        drawLine(
            glm::vec3(-2.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 0.0f, 0.0f),
            glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
            view,
            projection
        );



        // DRAW WORLD Y-AXIS


        drawLine(
            glm::vec3(0.0f, -1.5f, 0.0f),
            glm::vec3(0.0f, 1.5f, 0.0f),
            glm::vec4(0.8f, 0.8f, 0.8f, 1.0f),
            view,
            projection
        );



        // DRAW REFLECTION LINE



        // Convert the reflection-line angle into radians.

        float lineTheta =
            glm::radians(
                reflectionAngle
            );



        // Direction vector along the reflection line.
        //
        // For a line with angle theta:
        //
        //     direction = (cos(theta), sin(theta))
        //

        glm::vec3 lineDirection(
            std::cos(lineTheta),
            std::sin(lineTheta),
            0.0f
        );



        // Choose two points far apart on the reflection line.
        //
        // Both points are centred around (reflectionX, reflectionY).

        glm::vec3 lineStart =
            glm::vec3(
                reflectionX,
                reflectionY,
                0.0f
            )
            -
            lineDirection * 2.0f;


        glm::vec3 lineEnd =
            glm::vec3(
                reflectionX,
                reflectionY,
                0.0f
            )
            +
            lineDirection * 2.0f;



        // Draw the arbitrary reflection line.

        drawLine(
            lineStart,
            lineEnd,
            glm::vec4(
                1.0f,
                0.2f,
                0.2f,
                1.0f
            ),
            view,
            projection
        );



        // DRAW ROTATION PIVOT



        // We represent the pivot using a small cross.


        float pivotSize = 0.08f;


        drawLine(
            glm::vec3(
                pivotX - pivotSize,
                pivotY,
                0.0f
            ),
            glm::vec3(
                pivotX + pivotSize,
                pivotY,
                0.0f
            ),
            glm::vec4(
                0.2f,
                1.0f,
                0.2f,
                1.0f
            ),
            view,
            projection
        );


        drawLine(
            glm::vec3(
                pivotX,
                pivotY - pivotSize,
                0.0f
            ),
            glm::vec3(
                pivotX,
                pivotY + pivotSize,
                0.0f
            ),
            glm::vec4(
                0.2f,
                1.0f,
                0.2f,
                1.0f
            ),
            view,
            projection
        );



        // DRAW ORIGINAL TRIANGLE



        // Identity model matrix means:
        //
        //     do not transform the object.
        //
        // Therefore this is the original geometry.

        glm::mat4 originalModel =
            glm::mat4(1.0f);


        drawTriangle(
            originalModel,
            view,
            projection,
            glm::vec4(
                0.3f,
                0.6f,
                1.0f,
                0.35f
            )
        );



        // CREATE ARBITRARY-POINT ROTATION MATRIX


        glm::mat4 rotationModel =
            createArbitraryPointRotation(
                pivotX,
                pivotY,
                rotationAngle
            );



        // DRAW ROTATED TRIANGLE


        drawTriangle(
            rotationModel,
            view,
            projection,
            glm::vec4(
                0.2f,
                1.0f,
                0.3f,
                0.85f
            )
        );



        // CREATE ARBITRARY-LINE REFLECTION MATRIX


        glm::mat4 reflectionModel =
            createArbitraryLineReflection(
                reflectionX,
                reflectionY,
                reflectionAngle
            );



        // DRAW REFLECTED TRIANGLE


        drawTriangle(
            reflectionModel,
            view,
            projection,
            glm::vec4(
                1.0f,
                0.7f,
                0.2f,
                0.85f
            )
        );



        // SWAP FRONT AND BACK BUFFERS



        // The back buffer contains the newly rendered frame.
        //
        // Swap it with the front buffer so the user can see it.

        glfwSwapBuffers(window);



        // PROCESS GLFW EVENTS



        // Handle:
        //
        // - keyboard events
        // - window events
        // - mouse events
        //

        glfwPollEvents();
    }



    // CLEANUP



    // Delete VBO.

    glDeleteBuffers(
        1,
        &VBO
    );



    // Delete VAO.

    glDeleteVertexArrays(
        1,
        &VAO
    );



    // Delete shader program.

    glDeleteProgram(
        shaderProgram
    );



    // Destroy GLFW window.

    glfwDestroyWindow(
        window
    );



    // Shut down GLFW.

    glfwTerminate();



    // Successful program termination.

    return 0;
}