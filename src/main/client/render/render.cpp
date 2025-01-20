#include "glad/glad.h"
#include "render.h"
#include "mesh.h"
#include "spdlog/spdlog.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "log.h"

#ifdef DISCRETE_DEBUG
#  define glWrap(x) glClearError(); x; glLogError(#x, __FILE__, __LINE__);

static void glClearError(){
    while(glGetError() != GL_NO_ERROR);
}

static void glLogError(const char* functionName, const char* fileName, int line){
    GLenum errorCode;
    while((errorCode = glGetError()) != GL_NO_ERROR){

    }
}


#else
#define glWrap(x) x;
#endif

unsigned int render::create_shader_program(const std::unordered_map<unsigned int, const char*>& shaders_by_type){

    std::vector<unsigned int> shader_ids{};
    unsigned int programId{glCreateProgram()};
    
    for(const auto &[type, shader]: shaders_by_type){

        auto shader_id{glCreateShader(type)};
        shader_ids.push_back(shader_id);
        glShaderSource(shader_id, 1, &shader, nullptr);
        glCompileShader(shader_id);

        int success;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

        if(!success){
            char infoLog[1024];
            glGetShaderInfoLog(shader_id, 1024, nullptr, infoLog);
            spdlog::get(discrete::CLIENT_LOGGER_NAME)->
                    error(std::string{"gl shader compilation error: "} +  infoLog);
            return 0;
        }

        glAttachShader(programId, shader_id);

    }

    glLinkProgram(programId);

    int success;
    glGetProgramiv(programId, GL_COMPILE_STATUS, &success);
    if(!success){
        char infoLog[1024];
        glGetProgramInfoLog(programId, 1024, nullptr, infoLog);
    }

    return programId;

}

//void render::draw(
//            glm::mat4 view,
//            const std::vector<render::Mesh> &meshes,
//            unsigned int shader_program_id,
//            const std::shared_ptr<GLFWwindow> &window
//            ) {
//
//    glWrap(glUseProgram(shader_program_id));
//
//    //TODO lets move this to config in the future
//    static glm::mat4 projection = glm::perspective(
//            glm::radians(45.0f),
//            (float) discrete::WINDOW_START_WIDTH / (float) discrete::WINDOW_START_HEIGHT,
//            0.1f, 100.0f
//    );
//
//    unsigned int vao, vbo, ebo;
//
//    //TODO we don't need to do this every draw call!
//    glWrap(glGenVertexArrays(1, &vao))
//    glWrap(glGenBuffers(1, &vbo))
//    glWrap(glGenBuffers(1, &ebo))
//
//    glWrap(glBindVertexArray(vao))
//    glWrap(glBindBuffer(GL_ARRAY_BUFFER, vbo))
//    glWrap(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo))
//
//    glWrap(glBufferData(GL_ARRAY_BUFFER, discrete::GL_INDEX_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW))
//    glWrap(glBufferData(GL_ELEMENT_ARRAY_BUFFER, discrete::GL_ELEMENT_BUFFER_SIZE, nullptr, GL_DYNAMIC_DRAW))
//
//    glWrap(glUniformMatrix4fv(glGetUniformLocation(shader_program_id, "view"), 1, GL_FALSE, &view[0][0]))
//    glWrap(glUniformMatrix4fv(glGetUniformLocation(shader_program_id, "projection"), 1, GL_FALSE, &projection[0][0]))
//
//    glWrap(void *vertexBuffer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY))
//    glWrap(void *indexBuffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY))
//
//    //position attribute
//    glWrap(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *) nullptr))
//    glWrap(glEnableVertexAttribArray(0))
//
//    unsigned long indexPointerOffset{0};
//    unsigned long vertexPointerOffset{0};
//
//    for (const render::Mesh &mesh: meshes) {
//        std::memcpy(vertexBuffer, mesh.vertices.data(), mesh.vertices.size() * sizeof(Vertex));
//        std::memcpy(indexBuffer, mesh.indices.data(), mesh.indices.size() * sizeof(unsigned int));
//        vertexPointerOffset += sizeof(Vertex) * mesh.vertices.size();
//        indexPointerOffset += sizeof(unsigned int) * mesh.indices.size();
//    }
//
//    glWrap(glUnmapBuffer(GL_ARRAY_BUFFER))
//    glWrap(glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
//
//    glWrap(glDrawElements(GL_TRIANGLES, indexPointerOffset / sizeof(unsigned int), GL_UNSIGNED_INT, 0))
//
//    glfwSwapBuffers(window.get());
//
//}