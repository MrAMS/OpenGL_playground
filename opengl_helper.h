#pragma once

#include <cstdlib>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>

class texture_obj;

class shader_obj{
    public:
        shader_obj(const char* vertex_shader_path, const char* fragment_shader_path);
        unsigned int program_id, vertex_id, fragment_id;
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
        texture_obj(const char* file_name, GLenum color_format);
        unsigned int texture_id;
        // remember to use the shader at first
        void blind(unsigned int pos);
        
};
