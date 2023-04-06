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

void shader_obj::setBool(const char* key, bool val) const{
    setInt(key, (int)val);
}

void shader_obj::setInt(const char* key, int val) const{
    glUniform1i(glGetUniformLocation(program_id, key), val);
}

void shader_obj::setFloat(const char* key, float val) const{
    glUniform1f(glGetUniformLocation(program_id, key), val); 
}

void shader_obj::setMatrix4f(const char* key, glm::mat4 mat) const{
    glUniformMatrix4fv(glGetUniformLocation(program_id, key), 1, GL_FALSE, glm::value_ptr(mat));
}

void shader_obj::blind_texture(const char* key, unsigned int pos){
    use();
    setInt(key, pos);
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
    glDeleteProgram(program_id);
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
                            GLenum buffer_usage, GLenum draw_type){

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
    glDeleteVertexArrays(1, &VAO_id);
    glDeleteBuffers(1, &VBO_id);
    if(e_cnt != 0)
        glDeleteBuffers(1, &EBO_id);
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
