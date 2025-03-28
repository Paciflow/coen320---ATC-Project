#include <iostream>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


#include "Airspace.h"
#include "Aircraft.h"

void checkOpenGLError(const char* stmt, const char* fname, int line) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error " << err << " at " << fname << ":" << line << " - for " << stmt << std::endl;
    }
}

#define GL_CHECK(stmt) do { \
        stmt; \
        checkOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

// Vertex Shader Source
const char* vertexShaderSource = R"(
    #version 330 core
    in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

// Fragment Shader Source
const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0); // White color
    }
)";

// Function to compile shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    GL_CHECK(glShaderSource(shader, 1, &source, NULL));
    GL_CHECK(glCompileShader(shader));

    GLint success;
    GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        char infoLog[512];
        GL_CHECK(glGetShaderInfoLog(shader, 512, NULL, infoLog));
        std::cerr << "Shader Compilation Failed:\n" << infoLog << std::endl;
    }
    return shader;
}

// Function to load a 3D model using Assimp
void loadModel(const std::string& path, std::vector<GLfloat>& vertices, std::vector<GLuint>& indices) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::cout << "Model loaded successfully: " << path << std::endl;
    std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;

    // Process all meshes in the scene
    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        aiMesh* mesh = scene->mMeshes[m];
        if (!mesh) {
            std::cerr << "Error: Mesh is null." << std::endl;
            continue;
        }

        std::cout << "Processing mesh " << m << ":" << std::endl;
        std::cout << "  Number of vertices: " << mesh->mNumVertices << std::endl;
        std::cout << "  Number of faces: " << mesh->mNumFaces << std::endl;

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            vertices.push_back(mesh->mVertices[i].x);
            vertices.push_back(mesh->mVertices[i].y);
            vertices.push_back(mesh->mVertices[i].z);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; ++j) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    if (vertices.empty() || indices.empty()) {
        std::cerr << "Warning: No vertex or index data loaded from the model." << std::endl;
    }
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

    // Request OpenGL 3.3 context (Core Profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Air Traffic Control", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Print OpenGL version
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    GL_CHECK(glEnable(GL_DEPTH_TEST));

    // Load the 3D model
    std::vector<GLfloat> airplaneVertices;
    std::vector<GLuint> airplaneIndices;
    std::string modelPath = "/Users/abou/Desktop/ATC/airplane.obj"; // Adjust this path as needed
    std::cout << "Loading airplane model: " << modelPath << std::endl;
    loadModel(modelPath, airplaneVertices, airplaneIndices);

    if (airplaneVertices.empty() || airplaneIndices.empty()) {
        std::cerr << "Error: Failed to load airplane model." << std::endl;
        glfwTerminate();
        return -1;
    } else {
        std::cout << "Airplane model loaded. Number of vertices: " << airplaneVertices.size() / 3 << ", Number of indices: " << airplaneIndices.size() << std::endl;
    }

    // Create and compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    // Create shader program
    GLuint shaderProgram = glCreateProgram();
    GL_CHECK(glAttachShader(shaderProgram, vertexShader));
    GL_CHECK(glAttachShader(shaderProgram, fragmentShader));
    GL_CHECK(glLinkProgram(shaderProgram));

    GLint programSuccess;
    GL_CHECK(glGetProgramiv(shaderProgram, GL_LINK_STATUS, &programSuccess));
    if (!programSuccess) {
        char infoLog[512];
        GL_CHECK(glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog));
        std::cerr << "Shader Program Linking Failed:\n" << infoLog << std::endl;
        return -1;
    }

    GL_CHECK(glUseProgram(shaderProgram));

    // Cleanup shaders
    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));


    GLuint airplaneVAO, airplaneVBO, airplaneEBO;
    GL_CHECK(glGenVertexArrays(1, &airplaneVAO));
    GL_CHECK(glGenBuffers(1, &airplaneVBO));
    GL_CHECK(glGenBuffers(1, &airplaneEBO));

    GL_CHECK(glBindVertexArray(airplaneVAO));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, airplaneVBO));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, airplaneVertices.size() * sizeof(GLfloat), airplaneVertices.data(), GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, airplaneEBO));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, airplaneIndices.size() * sizeof(GLuint), airplaneIndices.data(), GL_STATIC_DRAW));

    // Position attribute
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0));
    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GL_CHECK(glBindVertexArray(0));
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    // Create Airspace
    Airspace* airspace = createAirspace(200.0, 200.0, 50.0, 10.0, 10);

    // Create some Aircraft
    Aircraft* aircraft1 = createAircraft(1, -10.0, 0.0, -20.0, 15.0, 2.0, 0.0, 0.0, 5.0, 5.0, 2.0);
    Aircraft* aircraft2 = createAircraft(2, 10.0, 0.0, 20.0, -3.0, 0.0, 0.0, 0.0, 7.0, 6.0, 2.5);
    Aircraft* aircraft3 = createAircraft(3, 0.0, 15.0, 25.0, 0.0, -8.0, 0.0, 20.0, 4.0, 4.0, 1.5);

    // Add aircraft to airspace
    addAircraftToAirspace(airspace, aircraft1);
    addAircraftToAirspace(airspace, aircraft2);
    addAircraftToAirspace(airspace, aircraft3);

    // Get uniform locations
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    float fov = glm::radians(45.0f);
    float aspectRatio = 800.0f / 600.0f;
    float nearClip = 0.1f;
    float farClip = 100.0f;
    glm::mat4 projection = glm::perspective(fov, aspectRatio, nearClip, farClip);

    // View matrix (camera position)
    glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 40.0f);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

    float vertical_offset = 0.0f;
    float initialRotationAngle = 90.0f;

    // Render Loop
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Update aircraft positions
        const auto& aircraft_list = getAircraftInAirspace(airspace);
        for (Aircraft* aircraft : aircraft_list) {
            if (!isAircraftOutOfBounds(aircraft, 200.0f, 200.0f, 50.0f)) {
                updateAircraftPosition(aircraft, deltaTime);
            }
        }

        // Check for separation violations
        checkSeparationViolations(airspace, 5.0); // Check 5 seconds into the future

        // Clear Screen
        GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Set view and projection matrices
        GL_CHECK(glUseProgram(shaderProgram));
        GL_CHECK(glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view)));
        GL_CHECK(glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection)));

// Draw each aircraft as the loaded 3D model
        GL_CHECK(glBindVertexArray(airplaneVAO));
        for (Aircraft* aircraft : aircraft_list) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3((float)aircraft->position.x, (float)aircraft->position.y + vertical_offset, (float)aircraft->position.z));

            float currentRotationAngle = initialRotationAngle;
            if (aircraft->id == 1) {
                currentRotationAngle += 180.0f;
            }

            if (aircraft->id == 3) {
                currentRotationAngle += 90.0f;
            }
            model = glm::rotate(model, glm::radians(currentRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));

            // Scale the model down to a reasonable size
            float scaleFactor = 0.0005f;
            model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

            // Apply the transformation
            GL_CHECK(glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model)));

            GL_CHECK(glDrawElements(GL_TRIANGLES, airplaneIndices.size(), GL_UNSIGNED_INT, 0));
        }
        GL_CHECK(glBindVertexArray(0));

        // Swap Buffers & Poll Events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    GL_CHECK(glDeleteVertexArrays(1, &airplaneVAO));
    GL_CHECK(glDeleteBuffers(1, &airplaneVBO));
    GL_CHECK(glDeleteBuffers(1, &airplaneEBO));
    GL_CHECK(glDeleteProgram(shaderProgram));
    glfwDestroyWindow(window);
    glfwTerminate();


    delete aircraft1;
    delete aircraft2;
    delete aircraft3;
    delete airspace;
    return 0;
}
