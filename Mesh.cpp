#include "Mesh.h"

Mesh::Mesh(std::string objectPath, std::string texturePath) {
	loadVertices(objectPath);
	loadTexture(texturePath);
	setupMeshVertices();
	setupMeshTexture();
}

/* Load an .obj file into a vector containing vertices' attributes */
void Mesh::loadVertices(std::string objectPath) {
	std::string warn, err;
	bool bTriangulate = true;
	bool bSuc = tinyobj::LoadObj(&obj.attrib, &obj.shapes, &obj.materials,
		&warn, &err, objectPath.c_str(), nullptr, bTriangulate);
	if (!bSuc)
	{
		std::cout << "tinyobj error: " << err.c_str() << std::endl;
		return;
	}

	for (auto face : obj.shapes[0].mesh.indices) {
		int vid = face.vertex_index;
		int nid = face.normal_index;
		int tid = face.texcoord_index;

		Vertex vertex = {};
		vertex.position = glm::vec3(obj.attrib.vertices[vid * 3], obj.attrib.vertices[vid * 3 + 1], obj.attrib.vertices[vid * 3 + 2]);
		vertex.normal = glm::vec3(obj.attrib.normals[nid * 3], obj.attrib.normals[nid * 3 + 1], obj.attrib.normals[nid * 3 + 2]);
		vertex.texture = glm::vec2(obj.attrib.texcoords[tid * 2], obj.attrib.texcoords[tid * 2 + 1]);
		vertices.push_back(vertex);
	}
}

/* Load texture from file */
void Mesh::loadTexture(std::string texturePath) {
	stbi_set_flip_vertically_on_load(true);
	texData = stbi_load(texturePath.c_str(), &texWidth, &texHeight, &texNrChannels, 0);
	if (!texData) {
		std::cout << "Failed to load texture " << texturePath << std::endl;
		return;
	}
}

/* Create vertex buffers and pass data to vertex shader */
void Mesh::setupMeshVertices() {
	// Create buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	// Pass vertices data to vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	// Set vertex position in vertex shader
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// Set vertex normal in vertex shader
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set vertex texture in vertex shader
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void Mesh::setupMeshTexture() {
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
	stbi_image_free(texData);
}

void Mesh::render() {
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

void Mesh::deleteBuffers() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
