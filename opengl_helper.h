#pragma once

#include <cstdlib>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <initializer_list>


#define KEY_VAL(X) #X,X 

class texture_obj;

class shader_obj{
    public:
        unsigned int program_id, vertex_id, fragment_id;

        shader_obj(const char* vertex_shader_path, const char* fragment_shader_path);
        void use();
        void setBool(const char* key, bool val) const;
        void setInt(const char* key, int val) const;
        void setFloat(const char* key, float val) const;
        void setMatrix4f(const char* key, glm::mat4 mat) const;
        void blind_texture(const char* key, unsigned int pos);
        ~shader_obj();
    private:
        // utility function for checking shader compilation/linking errors.
        void check_compile_errors(unsigned int shader, const char* type);
};

class texture_obj{
    public:
        unsigned int texture_id;

        texture_obj(const char* file_name, GLenum color_format);
        // remember to use the shader at first
        void blind(unsigned int pos);
        
};

class vertex_array_obj{
    public:
        // Vertex Array ID
        unsigned int VAO_id;
        // Vertex Buffer ID
        unsigned int VBO_id;
        // Element Buffer
        unsigned int EBO_id;
        // Vertex number
        unsigned int v_cnt;
        // Element number
        unsigned int e_cnt;

        vertex_array_obj(unsigned int vertex_num, std::initializer_list<unsigned int> vertex_div, float* vertex_data,
                            unsigned int element_num, unsigned int* element_data, 
                            GLenum buffer_usage);
        ~vertex_array_obj();
        void draw_array(GLenum draw_mode, int beg, int num);
        void draw_element(GLenum draw_mode, int num);
};
