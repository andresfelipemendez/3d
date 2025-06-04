#pragma once

#include <glm.hpp>
#include "json.h"

struct StaticMesh {
	unsigned int elementCount, VAO, VBO, EBO, material;
};

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec3 normal;
};

class MeshFactory
{
public:
	StaticMesh build_gltf_mesh(const char* objectName);

private:
	Json::Value read_gltf_file(const char* filename);

	std::vector<char> read_binary_file(const char* filename);

	void fetch_vec3(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<glm::vec3>& dst);

	void fetch_vec2(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<glm::vec2>& dst);

	void fetch_indices(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<char>& dst);

	void load_material(const char* objectName, Json::Value& gltfData, StaticMesh& mesh);

	void build_vertices();

	void build_buffers(StaticMesh& mesh);

	void describe_attributes();

	std::vector<glm::vec3> v;
	std::vector<glm::vec2> vt;
	std::vector<glm::vec3> vn;
	std::vector<Vertex> vertices;
	std::vector<Vertex> indices;
};

