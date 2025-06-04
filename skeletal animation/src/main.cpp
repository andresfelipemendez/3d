#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>

#include <glm.hpp>
#include <SDL3/SDL.h>

namespace fs = std::filesystem;

#include "json.h"

int main() {
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("failed to initialize sdl: %s ", SDL_GetError());
	}
	return 0;
}