#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

#include "Airspace.h"
#include "Aircraft.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Vertex Data for a single point (position and color)
GLfloat vertices[] = {
        0.0f, 0.0f, 0.0f,  1.0f, 1.0f, 1.0f  
};

// Vertex Shader Code
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 vertexColor;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)";

// Fragment Shader Code
const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)";

// Compile Shader Function
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader Compilation Failed:\n" << infoLog << std::endl;
    }
    return shader;
}

// Function to check if an aircraft is out of bounds
bool isAircraftOutOfBounds(const Aircraft* aircraft, float airspaceWidth, float airspaceHeight, float airspaceDepth) {
    return aircraft->position.x < -airspaceWidth || aircraft->position.x > airspaceWidth ||
           aircraft->position.y < -airspaceHeight || aircraft->position.y > airspaceHeight ||
           aircraft->position.z < -airspaceDepth || aircraft->position.z > airspaceDepth;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // OpenGL Version & Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Air Traffic Control", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Build and Compile Shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Create Shader Program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Cleanup Shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind and Fill VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position Attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color Attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Enable Depth Testing
    glEnable(GL_DEPTH_TEST);

    // Create Airspace
    Airspace* airspace = createAirspace(200.0, 200.0, 50.0, 10.0, 10); // Example dimensions

    // Create some Aircraft
    Aircraft* aircraft1 = createAircraft(1, -10.0, 0.0, 15.0, 5.0, 0.0, 0.0, 0.0, 5.0, 5.0, 2.0);
    Aircraft* aircraft2 = createAircraft(2, 10.0, 0.0, 20.0, -3.0, 0.0, 0.0, 0.0, 7.0, 6.0, 2.5);
    Aircraft* aircraft3 = createAircraft(3, 0.0, 15.0, 25.0, 0.0, -4.0, 0.0, 0.0, 4.0, 4.0, 1.5);

    // Add aircraft to airspace
    addAircraftToAirspace(airspace, aircraft1);
    addAircraftToAirspace(airspace, aircraft2);
    addAircraftToAirspace(airspace, aircraft3);

    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    // Projection matrix (perspective)
    float fov = glm::radians(45.0f);
    float aspectRatio = 800.0f / 600.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;
    glm::mat4 projection = glm::perspective(fov, aspectRatio, nearClip, farClip);

    // View matrix (camera position)
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 30.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    // Render Loop
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update aircraft positions
        const auto& aircraft_list = getAircraftInAirspace(airspace);
        for (Aircraft* aircraft : aircraft_list) {
            // Stop aircraft if they go out of bounds
            if (!isAircraftOutOfBounds(aircraft, 200.0f, 200.0f, 50.0f)) {
                updateAircraftPosition(aircraft, deltaTime);
            }
        }

        // Check for separation violations
        checkSeparationViolations(airspace, 5.0); // Check 5 seconds into the future

        // Clear Screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set view and projection matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Draw each aircraft as a point
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Set the point size (optional)
        glPointSize(10.0f);

        for (Aircraft* aircraft : aircraft_list) {
            // Transformation matrix (translation based on aircraft position)
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3((float)aircraft->position.x, (float)aircraft->position.y, (float)aircraft->position.z));

            // Scale the model (make the planes bigger, you can adjust the scaling factor)
            float scaleFactor = 3.0f; // Increase this value to make the planes bigger
            model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

            // Apply the transformation
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_POINTS, 0, 1); // Draw a single point
        }

        // Swap Buffers & Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    // Cleanup allocated memory
    delete aircraft1;
    delete aircraft2;
    delete aircraft3;
    delete airspace; // Assuming you'll implement a proper destructor or free function for Airspace
    return 0;
}
