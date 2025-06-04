#include "mode_factory.h"
#include "glad.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <fstream>

StaticMesh MeshFactory::build_gltf_mesh(const char* objectName) {
	StaticMesh mesh;
	return mesh;
}

Json::Value MeshFactory::read_gltf_file(const char* filename) {
	std::ifstream jsonFile;
	Json::Value fileContents;
	jsonFile.open(filename, std::ios::binary);
	jsonFile >> fileContents;
	jsonFile.close();
	return fileContents;
}

std::vector<char> MeshFactory::read_binary_file(const char* filename) {
	std::ifstream binaryFile;
	std::vector<char> byteData;
	binaryFile.open(filename, std::ios::binary | std::ios::ate);
	rsize_t byteCount = binaryFile.tellg();
	byteData.resize(byteCount);
	binaryFile.seekg(0);
	binaryFile.read(byteData.data(), byteCount);
	binaryFile.close();
	return byteData;
}

void MeshFactory::fetch_vec3(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<glm::vec3>& dst) {
	int accesor = gltfData["meshes"][0]["primitives"][0]["attributes"][attributeName].asInt();
	int bufferView = gltfData["accesssors"][accesor]["bufferView"].asInt();
	int count = gltfData["accesssors"][accesor]["count"].asInt();
	dst.resize(count);
	int byteLength = gltfData["bufferViews"][bufferView]["byteLength"].asInt();
	int byteOffset = gltfData["bufferViews"][bufferView]["byteOffset"].asInt();
	memcpy(dst.data(), byteData.data() + byteOffset, byteLength);

}

void MeshFactory::fetch_vec2(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<glm::vec2>& dst) {
	int accesor = gltfData["meshes"][0]["primitives"][0]["attributes"][attributeName].asInt();
	int bufferView = gltfData["accesssors"][accesor]["bufferView"].asInt();
	int count = gltfData["accesssors"][accesor]["count"].asInt();
	dst.resize(count);
	int byteLength = gltfData["bufferViews"][bufferView]["byteLength"].asInt();
	int byteOffset = gltfData["bufferViews"][bufferView]["byteOffset"].asInt();
	memcpy(dst.data(), byteData.data() + byteOffset, byteLength);
}

void MeshFactory::fetch_indices(const char* attributeName, Json::Value& gltfData, std::vector<char>& byteData, std::vector<char>& dst) {
	int accesor = gltfData["meshes"][0]["primitives"][0]["indices"].asInt();
	int bufferView = gltfData["accesssors"][accesor]["bufferView"].asInt();
	int count = gltfData["accesssors"][accesor]["count"].asInt();
	indices.resize(count);
	int byteLength = gltfData["bufferViews"][bufferView]["byteLength"].asInt();
	int byteOffset = gltfData["bufferViews"][bufferView]["byteOffset"].asInt();
	memcpy(indices.data(), byteData.data() + byteOffset, byteLength);
}

void MeshFactory::load_material(const char* objectName, Json::Value& gltfData, StaticMesh& mesh) {
	std::stringstream filenameBuilder;
	filenameBuilder << "assets/" << objectName << "/" << gltfData["images"][0]["uri"].asString();
	std::string filename = filenameBuilder.str();

	int width, height, channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels,STBI_rgb_alpha);
	glGenTextures(1, &mesh.material);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);
}

void MeshFactory::build_vertices() {
	size_t vertex_count = v.size();
	vertices.resize(vertex_count);

	for (size_t i = 0; i < vertex_count; ++i) {
		Vertex vertex;
		vertex.position = v[i];
		vertex.texCoord = vt[i];
		vertex.normal = vn[i];
		vertices[i] = vertex;
	}
}

void MeshFactory::build_buffers(StaticMesh& mesh) {
	glGenVertexArrays(1, &mesh.VAO);
	glBindVertexArray(mesh.VAO);

	glGenBuffers(1, &mesh.VBO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &mesh.EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
	mesh.elementCount = indices.size();
}

void MeshFactory::describe_attributes() {
	//position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, (void*)0);
	glEnableVertexAttribArray(0);
	//texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 32, (void*)12);
	glEnableVertexAttribArray(1);
	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 32, (void*)20);
	glEnableVertexAttribArray(2);
}

