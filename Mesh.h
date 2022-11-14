#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <iostream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Object {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
};

struct Vertex {
	glm::vec3 position;  /* position vector */
	glm::vec3 normal;    /* normal vector */
	glm::vec2 texture;   /* texture coordinate */
};

class Mesh {
public:
	std::vector<Vertex> vertices;          /* a collection of vertices */
	unsigned int textureID;                /* the mesh's texture ID    */
	
	Mesh(std::string objectPath, std::string texturePath);
	void render();
	void deleteBuffers();
private:
	Object obj;                             /* object loaded from file  */
	int texWidth, texHeight, texNrChannels; /* texture props */
	unsigned char* texData;                 /* texture data */
	unsigned int VAO, VBO;
	void setupMeshVertices();
	void setupMeshTexture();
	void loadVertices(std::string objectPath);
	void loadTexture(std::string texturePath);
};


