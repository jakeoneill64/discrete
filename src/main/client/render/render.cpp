//
// Created by Jake M O'Neill on 07/08/2022.
//

#include "glad/glad.h"
#include "client/Constants.h"
#include "render.h"
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



discrete::render::render()
:
projection(glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f))
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, discrete::GL_BUF_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (3*sizeof(float)));
    glEnableVertexAttribArray(1);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(discrete::GL_BUF_SIZE), nullptr, GL_DYNAMIC_DRAW);

    Shader vertexShader{"/User/jake/Dev/discrete/src/resources/shader/vertex-basic.glsl", GL_VERTEX_SHADER};
    Shader fragmentShader{"/User/jake/Dev/discrete/src/resource/shader/fragment-basic.glsl", GL_FRAGMENT_SHADER};

    shaderProgramId = Shader::linkShaders(vertexShader, fragmentShader);

    glUseProgram(shaderProgramId);

    

}

void discrete::render::draw(Camera camera, GLFWwindow* window) {

    glWrap(glUniformMatrix4fv(glGetUniformLocation(shaderProgramId, "view"), 1, GL_FALSE, &camera.getViewMatrix()[0][0]));
    glWrap(glUniformMatrix4fv(glGetUniformLocation(shaderProgramId, "projection"), 1, GL_FALSE, &projection[0][0]));


    unsigned int indexPointer{0};
    unsigned int elementPointer{0};

    glWrap(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    glWrap(glClear(GL_COLOR_BUFFER_BIT));


    for(const Mesh& mesh: m_meshes){
        glWrap(glBufferSubData(GL_ARRAY_BUFFER, indexPointer, (long) (sizeof(Vertex) * mesh.vertices.size()), mesh.vertices.data()));
        glWrap(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, elementPointer, (long) (sizeof(unsigned int) * mesh.indices.size()), mesh.indices.data()));
        indexPointer += sizeof(Vertex) * mesh.vertices.size();
        elementPointer += (sizeof(unsigned int) * mesh.indices.size());
    }

    glWrap(glBindVertexArray(VAO));
    glWrap(glDrawElements(GL_TRIANGLES, elementPointer, GL_UNSIGNED_INT, 0));

    glfwSwapBuffers(window);

}

void discrete::render::addMesh(const Mesh& mesh) {
    m_meshes.push_back(mesh);
}

discrete::render::~render() {
    glfwTerminate();
}
