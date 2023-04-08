#include "opengl_helper.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

shader_obj::shader_obj(const char* vertex_shader_path, const char* fragment_shader_path){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // 打开文件
        vShaderFile.open(vertex_shader_path);
        fShaderFile.open(fragment_shader_path);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();       
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();     
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "[File ERROR] Fail to open shader file" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    // vertex shader
    vertex_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id, 1, &vShaderCode, NULL);
    glCompileShader(vertex_id);
    check_compile_errors(vertex_id, "VERTEX");
    // fragment Shader
    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, &fShaderCode, NULL);
    glCompileShader(fragment_id);
    check_compile_errors(fragment_id, "FRAGMENT");
    // shader Program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);
    check_compile_errors(program_id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}

void shader_obj::use(){
    glUseProgram(program_id);
}

int shader_obj::get_uniform_loc(const char* key) const{
    return glGetUniformLocation(program_id, key);
}
void shader_obj::set_bool(const char* key, bool val) const{
    set_int(key, (int)val);
}

void shader_obj::set_int(const char* key, int val) const{
    glUniform1i(get_uniform_loc(key), val);
}

void shader_obj::set_float(const char* key, float val) const{
    glUniform1f(get_uniform_loc(key), val); 
}

void shader_obj::set_matrix(const char* key, const glm::mat4 &mat) const{
    glUniformMatrix4fv(get_uniform_loc(key), 1, GL_FALSE, glm::value_ptr(mat));
}

void shader_obj::set_vec(const char* key, const glm::vec3 &val) const{
    glUniform3fv(get_uniform_loc(key), 1, &val[0]); 
}

void shader_obj::set_vec(const char* key, const float x, const float y, const float z) const{
    glUniform3f(get_uniform_loc(key), x, y, z);
}

void shader_obj::set_vec(const char* key, const glm::vec4 &val) const{
    glUniform4fv(get_uniform_loc(key), 1, &val[0]); 
}

void shader_obj::set_vec(const char* key, const float x, const float y, const float z, const float w) const{
    glUniform4f(get_uniform_loc(key), x, y, z, w);
}

void shader_obj::blind_texture(const char* key, unsigned int pos){
    use();
    set_int(key, pos);
}

void shader_obj::check_compile_errors(unsigned int shader, const char* type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("[%s Shader ERROR] %s\n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("[%s Program ERROR] %s\n", type, infoLog);
        }
    }
}    

shader_obj::~shader_obj(){
    //glDeleteProgram(program_id);
}

texture_obj::texture_obj(const char* file_name, GLenum color_format){
    int width, height, nrCh;
    unsigned char* data = stbi_load(file_name, &width, &height, &nrCh, 0);
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        printf("[OK] Texture %s %d*%d %dchs readed.\n", file_name, height, width, nrCh);
    }else
        printf("[File ERROR] Fail to load texture.\n");
    stbi_image_free(data);
}

void texture_obj::blind(unsigned int pos){
    glActiveTexture(GL_TEXTURE0+pos);
    glBindTexture(GL_TEXTURE_2D, texture_id);
}

vertex_array_obj::vertex_array_obj(unsigned int vertex_num, std::initializer_list<unsigned int> vertex_div, float* vertex_data,
                            unsigned int element_num, unsigned int* element_data, 
                            GLenum buffer_usage){

    v_cnt = vertex_num;
    e_cnt = element_num;
    unsigned int vertex_per_size = 0;
    for(const unsigned int &item : vertex_div)
        vertex_per_size += item;
    // Vertex Array
    glGenVertexArrays(1, &VAO_id);
    
    glBindVertexArray(VAO_id);

    // Vertex Buffer 
    glGenBuffers(1, &VBO_id);

    // Element Buffer
    if(element_num != 0)
        glGenBuffers(1, &EBO_id);
        

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_per_size*vertex_num, vertex_data, buffer_usage);

    if(element_num != 0){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*element_num, element_data, buffer_usage);
    }

    int i=0, j=0;
    for(const unsigned int &item : vertex_div){
        glVertexAttribPointer(i, item, GL_FLOAT, GL_FALSE, vertex_per_size*sizeof(float), (void*)(j*sizeof(float)));
        glEnableVertexAttribArray(i);
        i+=1;
        j+=item;
    }
    
    glBindVertexArray(0);

}

vertex_array_obj::~vertex_array_obj(){
    /*
    glDeleteVertexArrays(1, &VAO_id);
    glDeleteBuffers(1, &VBO_id);
    if(e_cnt != 0)
        glDeleteBuffers(1, &EBO_id);
    */
}

void vertex_array_obj::draw_array(GLenum draw_mode, int beg, int num){
    glBindVertexArray(VAO_id);
    glDrawArrays(draw_mode, beg, num);
}

void vertex_array_obj::draw_element(GLenum draw_mode, int num){
    if(e_cnt == 0){
        printf("[Draw ERROR]\n No available element buffer to draw\n");
        return;
    }
    glBindVertexArray(VAO_id);
    glDrawElements(draw_mode, num, GL_UNSIGNED_INT, 0);
}

camera_obj::camera_obj(float screen_w_div_h_, glm::vec3 position_,
                    glm::vec3 up_, float yaw_, float pitch_,
                    float sensitivity_, float fov_, float max_fov_)
                    :screen_w_div_h(screen_w_div_h_), position(position_), up(up_), yaw(yaw_), pitch(pitch_), sensitivity(sensitivity_), fov(fov_), max_fov(max_fov_){
    
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    calc_projection();
    calc_view();
    model = glm::mat4(1.0f);
}

void camera_obj::calc_view(){
    view = glm::lookAt(position, position + front, up);
}

void camera_obj::calc_projection(){
    projection = glm::perspective(glm::radians(fov), screen_w_div_h, 0.1f, 100.0f);
}

void camera_obj::update_shader_uniform(const shader_obj& shader, const char* view_key, const char* proj_key, const char* model_key){
    shader.set_matrix(view_key, view);
    shader.set_matrix(proj_key, projection);
    shader.set_matrix(model_key, model);
}

void camera_obj::change_pos(enum dir move_dir, float step){
    if (move_dir == UP)
        position += step * front;
    if (move_dir == DOWN)
        position -= step * front;
    if (move_dir == LEFT)
        position -= glm::normalize(glm::cross(front, up)) * step;
    if (move_dir == RIGHT)
        position += glm::normalize(glm::cross(front, up)) * step;
}

void camera_obj::change_pitch_yaw(float x_offset, float y_offset){
    yaw   += x_offset;
    pitch += y_offset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
}

void camera_obj::change_fov(float target_fov){
    fov = target_fov;
}

void camera_obj::input_pos(enum dir move_dir, float delta_time){
    float speed = static_cast<float>(2.5 * delta_time);
    change_pos(move_dir, speed);
}

void camera_obj::input_pitch_yaw(double cur_x, double cur_y){
    static bool is_first = true;
    static float last_x, last_y;
    if(is_first)
    {
        last_x = cur_x;
        last_y = cur_y;
        is_first = false;
    }

    float xoffset = cur_x - last_x;
    float yoffset = last_y - cur_y; 
    last_x = cur_x;
    last_y = cur_y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    change_pitch_yaw(xoffset, yoffset);
}

void camera_obj::input_fov(double scroll){
    if(fov >= 1.0f && fov <= max_fov)
        fov -= scroll;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= max_fov)
        fov = max_fov;
}
