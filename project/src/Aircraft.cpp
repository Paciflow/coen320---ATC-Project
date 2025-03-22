#include "Aircraft.h"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Model class implementation
Model::Model(const std::string& path) {
    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    // Process vertices, normals, and texture coordinates
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        if (mesh->HasNormals()) {
            normals.push_back(glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
        }
        if (mesh->mTextureCoords[0]) {
            texCoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create and bind VAO, VBO, and EBO
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Clean up
    glBindVertexArray(0);
    VAOs.push_back(VAO);
    VBOs.push_back(VBO);
    EBOs.push_back(EBO);
}

void Model::Draw(GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) const {
    for (size_t i = 0; i < VAOs.size(); i++) {
        glBindVertexArray(VAOs[i]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
}

// Aircraft functions
Aircraft* createAircraft(uint32_t id, double x, double y, double z, double speed_x, double speed_y, double speed_z, double entry_time, double length, double width, double height) {
    Aircraft* newAircraft = static_cast<Aircraft*>(std::malloc(sizeof(Aircraft)));
    if (newAircraft == nullptr) {
        std::perror("Failed to allocate memory for aircraft");
        return nullptr;
    }
    newAircraft->id = id;
    newAircraft->position.x = x;
    newAircraft->position.y = y;
    newAircraft->position.z = z;
    newAircraft->velocity.speed_x = speed_x;
    newAircraft->velocity.speed_y = speed_y;
    newAircraft->velocity.speed_z = speed_z;
    newAircraft->entry_time = entry_time;
    newAircraft->size.length = length;
    newAircraft->size.width = width;
    newAircraft->size.height = height;
    return newAircraft;
}

void updateAircraftPosition(Aircraft* aircraft, double time_elapsed) {
    if (aircraft != nullptr) {
        aircraft->position.x += aircraft->velocity.speed_x * time_elapsed;
        aircraft->position.y += aircraft->velocity.speed_y * time_elapsed;
        aircraft->position.z += aircraft->velocity.speed_z * time_elapsed;
    }
}

void getAircraftInfo(const Aircraft* aircraft, uint32_t* id, Position* position, Velocity* velocity, Dimensions* size) {
    if (aircraft != nullptr) {
        if (id != nullptr) *id = aircraft->id;
        if (position != nullptr) *position = aircraft->position;
        if (velocity != nullptr) *velocity = aircraft->velocity;
        if (size != nullptr) *size = aircraft->size;
    }
}

void drawAircraft(const Aircraft* aircraft, const Model& model, GLuint shaderProgram, const glm::mat4& view, const glm::mat4& projection) {
    if (aircraft == nullptr) return;

    // Create a model matrix for the aircraft
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(aircraft->position.x, aircraft->position.y, aircraft->position.z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f)); // Scale the model

    // Pass the matrices to the shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Draw the model
    model.Draw(shaderProgram, view, projection);
}