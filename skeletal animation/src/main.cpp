#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#include <glm.hpp>

namespace fs = std::filesystem;

#include "json.h"

int main() {
	std::ifstream myBinaryFile;
	std::vector<char> myByteData;
	myBinaryFile.open("assets/my_cube.bin",std::ios::binary | std::ios::ate);
	size_t byteCount = myBinaryFile.tellg();
	myByteData.resize(byteCount);
	myBinaryFile.seekg(0);

	myBinaryFile.read(myByteData.data(), byteCount);
	myBinaryFile.close();

	std::cout << "vec3 data: " << std::endl;
	size_t vector_count = 24;

	std::cout << "vec3 data: " << std::endl;
	
	size_t offset = 288;
	size_t byte_size = vector_count * sizeof(glm::vec3);

	std::vector<glm::vec3> fetched_data;
	fetched_data.resize(vector_count);

	memcpy(fetched_data.data(), myByteData.data() + offset, byte_size);

	for (size_t i = 0; i < vector_count; ++i) {
		glm::vec3 v = fetched_data[i];
		printf("%zd: <%.3f, %.3f, %.3f>\n", i, v.x,v.y,v.z);
	}

	myBinaryFile.close();
	return 0;
}