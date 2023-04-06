#include <iostream>
#include "opengl_helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


void framebuffer_size_callback(GLFWwindow* win, int w, int h){
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
        printf("You have pressed ESE\n");
    }
}


int main(int argc, char** argv){
	printf("hello OpenGL\n");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    const int screen_width = 800, screen_height = 600;
 
    GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "LearnOpenGL", NULL, NULL);
    if(window==NULL){
        printf("Failed to create Window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        printf("Failed to init GLAD\n");
        return -1;
    }

    shader_obj shader("../vertex.vs", "../fragment.fs");
    texture_obj texture1("../resource/container.jpg", GL_RGB);
    texture_obj texture2("../resource/awesomeface.png", GL_RGBA);
    

    // vertex data
    float vertices[] = {
        // position         // color            // texture
        // top right        // red              // top right
        0.5f, 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
        // bot right        // green            // bot right
        0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
        // bot left         // blue             // bot left
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 
        // top left         // white            // top left
        -0.5f, 0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f,
    };
    // 
    unsigned int indices[] = {
        0, 2, 3, // 1st Triangle
        1, 2, 3  // 2nd Triangle
    };

    vertex_array_obj VAO(4, {3, 3, 2}, vertices, 6, indices, GL_STATIC_DRAW, GL_TRIANGLES);

    // wire frame polygons
    //`glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shader.use();
    
    shader.blind_texture("Tex1", 0);
    shader.blind_texture("Tex2", 1);

    // glm::mat4 trans(1.0f);
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
    // shader.setMatrix4f("transform", trans);
    

    // Render loop
    while(!glfwWindowShouldClose(window)){
        // key event
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // draw vertex
        /*
        float val = sin(glfwGetTime())/2.0+0.5f;
        int uniform_loc = glGetUniformLocation(shaderProgram, "col");
        glUseProgram(shaderProgram);
        glUniform4f(uniform_loc, 0.0f, val, 0.0f, 1.0f);
        */
        texture1.blind(0);
        texture2.blind(1);

        shader.use();
        glm::mat4 model(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        glm::mat4 view(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(45.0f), screen_width *1.0f / screen_height, 0.1f, 100.0f);
        shader.setMatrix4f(KEY_VAL(model));
        shader.setMatrix4f(KEY_VAL(view));
        shader.setMatrix4f(KEY_VAL(projection));
        

        VAO.draw_element(GL_TRIANGLES, 6);

        // check event and swap buffer
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    // release resources
    
    

    glfwTerminate();
    return 0;
}

