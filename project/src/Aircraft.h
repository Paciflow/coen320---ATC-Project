#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <cstdint>
#include <cmath>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Structure to represent the position of an aircraft
struct Position {
    double x;
    double y;
    double z;
};

// Structure to represent the velocity of an aircraft
struct Velocity {
    double speed_x;
    double speed_y;
    double speed_z;
};

// Structure to represent the dimensions (size) of an aircraft
struct Dimensions {
    double length;
    double width;
    double height;
};

// Structure to represent an aircraft
struct Aircraft {
    uint32_t id;
    Position position;
    Velocity velocity;
    Dimensions size;
    double entry_time; // Time when the aircraft enters the airspace
};

// Class to handle 3D model loading and rendering
class Model {
public:
    Model(const std::string& path);
    void Draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) const; // Add const here

private:
    std::vector<GLuint> VAOs, VBOs, EBOs;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    void processMesh(aiMesh* mesh, const aiScene* scene);
};

// Function to create a new aircraft with size
Aircraft* createAircraft(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time, double length, double width, double height);

// Function to update the position of an aircraft based on elapsed time
void updateAircraftPosition(Aircraft* aircraft, double time_elapsed);

// Function to get the current information (ID, position, velocity, size) of an aircraft
void getAircraftInfo(const Aircraft* aircraft, uint32_t* id, Position* position, Velocity* velocity, Dimensions* size);

// Function to draw the aircraft using a 3D model
void drawAircraft(const Aircraft* aircraft, const Model& model, GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection);

#endif // AIRCRAFT_H