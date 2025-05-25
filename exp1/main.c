#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "glad.h"
#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

// Vertex shader source - data flows through the GPU pipeline
static const char* vertex_shader_source = 
    "#version 450 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";

// Fragment shader source - determines pixel colors
static const char* fragment_shader_source = 
    "#version 450 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

// Triangle vertices in normalized device coordinates
// Data-oriented: just raw vertex data, no abstractions
static float vertices[] = {
    -0.5f, -0.5f, 0.0f,  // Bottom left
     0.5f, -0.5f, 0.0f,  // Bottom right
     0.0f,  0.5f, 0.0f   // Top center
};

static GLuint compile_shader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    // Check compilation status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        printf("Shader compilation failed: %s\n", info_log);
        return 0;
    }
    
    return shader;
}

static GLuint create_shader_program(void) {
    GLuint vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_source);
    
    if (!vertex_shader || !fragment_shader) {
        return 0;
    }
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    
    // Check linking status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        printf("Shader program linking failed: %s\n", info_log);
        return 0;
    }
    
    // Clean up individual shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    
    return program;
}

int SDL_main(int argc, char* argv[]) {
    // Initialize SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return -1;
    }
    
    // OpenGL context attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "OpenGL 4.5 Triangle",
        800, 600,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    
    if (!window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }
    
    // Create OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    if (!gl_context) {
        printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
    
    // Create shader program
    GLuint shader_program = create_shader_program();
    if (!shader_program) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    
    // Generate and bind Vertex Array Object (VAO)
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Generate and bind Vertex Buffer Object (VBO)
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Upload vertex data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Configure vertex attribute pointer
    // Layout location 0, 3 floats per vertex, no normalization, stride 12 bytes, offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Unbind to prevent accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    // Main loop
    bool running = true;
    SDL_Event event;

    cgltf_options options = {0};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, "assets/SimpleSkin.gltf", &data);
    if (result == cgltf_result_success)
    {
        /* TODO make awesome stuff */
        
        printf("Loaded file SimpleSkin\n");
        
    }
    
    
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                    running = false;
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;
            }
        }
        
        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Use shader program and draw triangle
        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        // Swap buffers
        SDL_GL_SwapWindow(window);
    }
    cgltf_free(data);
    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);
    
    SDL_GL_DestroyContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}