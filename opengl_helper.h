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

class camera_obj{
    public:
        glm::vec3 position, up, front;
        float pitch, yaw;

        // field of view
        float fov = 45.0f;
        float max_fov = 0;

        float screen_w_div_h;

        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 model;

        // input key sensitivity
        float sensitivity;

        enum dir{LEFT, RIGHT, UP, DOWN};

        camera_obj(float screen_w_div_h_, glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f),
                    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f), float yaw_ = -90.0f, float pitch_ = 0,
                    float sensitivity_=0.05, float fov_=45.0f, float max_fov_=55.0f);

        void calc_view();
        void calc_projection();

        void update_shader_uniform(const shader_obj& shader, const char* view_key, const char* proj_key, const char* model_key);
        
        void change_pos(enum dir move_dir, float step);
        void change_pitch_yaw(float x_offset, float y_offset);
        // change camera's field of view
        void change_fov(float target_fov);

        // input position by keyboard
        void input_pos(enum dir move_dir, float delta_time);
        // input camera's pitch & yaw by mouse
        void input_pitch_yaw(double cur_x, double cur_y);
        // input camera's pitch & yaw by scroll
        void input_fov(double scroll);
        
};
