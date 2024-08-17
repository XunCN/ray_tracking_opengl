#include <fstream>
#include <iostream>
#include "shader.h"

Shader::Shader():
    m_vertex_shader_id(0),
    m_geometry_shader_id(0),
    m_fragment_shader_id(0),
    m_compute_shader_id(0),
    m_program_id(0)
{
}

Shader::~Shader()
{
    if(m_program_id)
        glDeleteProgram(m_program_id);
}

bool Shader::add_vertex_shader(const std::string& path)
{
    return add_shader(ShaderType::VETEX_SHADER, path);
}

bool Shader::add_geometry_shader(const std::string& path)
{
    return add_shader(ShaderType::GEOMETRY_SHADER, path);
}

bool Shader::add_fragment_shader(const std::string& path)
{
    return add_shader(ShaderType::FRAGMENT_SHADER, path);
}

bool Shader::add_compute_shader(const std::string& path)
{
    return add_shader(ShaderType::COMPUTE_SHADER, path);
}

bool Shader::build_shader()
{
    if(m_program_id)
    {
        std::cout << "Shader already built\n";
        return true;
    }
    m_program_id = glCreateProgram();
    if(!m_program_id)
    {
        std::cerr << "Create shader program failed\n";
        return false;
    }
    if(m_vertex_shader_id)
        glAttachShader(m_program_id, m_vertex_shader_id);
    if(m_geometry_shader_id)
        glAttachShader(m_program_id, m_geometry_shader_id);
    if(m_fragment_shader_id)
        glAttachShader(m_program_id, m_fragment_shader_id);
    if(m_compute_shader_id)
        glAttachShader(m_program_id, m_compute_shader_id);

    glLinkProgram(m_program_id);
    GLint success;
    glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);
    if(!success)
    {
        char err_info[2048] = {0};
        glGetProgramInfoLog(m_program_id, sizeof(err_info), NULL, err_info);
        std::cerr << err_info << "\n";
        if(m_vertex_shader_id)
            glDetachShader(m_program_id, m_vertex_shader_id);
        if(m_geometry_shader_id)
            glDetachShader(m_program_id, m_geometry_shader_id);
        if(m_fragment_shader_id)
            glDetachShader(m_program_id, m_fragment_shader_id);
        if(m_compute_shader_id)
            glDetachShader(m_program_id, m_compute_shader_id);
        glDeleteProgram(m_program_id);
        m_program_id = 0;
        return false;
    }

    if(m_vertex_shader_id)
    {
        glDeleteShader(m_vertex_shader_id);
        m_vertex_shader_id = 0;
    }
    if(m_geometry_shader_id)
    {
        glDeleteShader(m_geometry_shader_id);
        m_geometry_shader_id = 0;
    }
    if(m_fragment_shader_id)
    {
        glDeleteShader(m_fragment_shader_id);
        m_fragment_shader_id = 0;
    }
    if(m_compute_shader_id)
    {
        glDeleteShader(m_compute_shader_id);
        m_compute_shader_id = 0;
    }

    return true;
}

void Shader::work(bool b_work) const
{
    glUseProgram(b_work ? m_program_id : 0);
}

bool Shader::add_shader(ShaderType type, const std::string& path)
{
    GLenum shader_type;
    GLuint* shader_id_ptr;
    switch(type)
    {
    case ShaderType::VETEX_SHADER:
        shader_type = GL_VERTEX_SHADER;
        shader_id_ptr = &m_vertex_shader_id;
        break;
    case ShaderType::GEOMETRY_SHADER:
        shader_type = GL_GEOMETRY_SHADER;
        shader_id_ptr = &m_geometry_shader_id;
        break;
    case ShaderType::FRAGMENT_SHADER:
        shader_type = GL_FRAGMENT_SHADER;
        shader_id_ptr = &m_fragment_shader_id;
        break;
    case ShaderType::COMPUTE_SHADER:
        shader_type = GL_COMPUTE_SHADER;
        shader_id_ptr = &m_compute_shader_id;
        break;
    default:
        std::cerr << "Add shader failed, unknow shader type: " << int(type) << "\n";
        return false;
    }

    std::ifstream is(path, std::ios_base::binary);
    if(!is)
    {
        std::cerr << "Open shader file failed: " << path << "\n";
        return false;
    }
    std::streampos beg = is.tellg();
    is.seekg(0, std::ios_base::end);
    std::streampos end = is.tellg();
    size_t size = end - beg;
    is.seekg(0, std::ios_base::beg);
    char* buffer = new char[size + 1];
    is.read(buffer, size);
    buffer[size] = '\0';

    if(*shader_id_ptr)
    {
        std::cout << "Shader type " << int(shader_type) << " already added, "
            << "old one will be removed.\n";
        glDeleteShader(*shader_id_ptr);
    }

    *shader_id_ptr = glCreateShader(shader_type);
    glShaderSource(*shader_id_ptr, 1, &buffer, NULL);
    glCompileShader(*shader_id_ptr);
    GLint success;
    glGetShaderiv(*shader_id_ptr, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        char err_info[2048] = {0};
        glGetShaderInfoLog(*shader_id_ptr, sizeof(err_info), NULL, err_info);
        std::cerr << err_info << "\n";
        delete[] buffer;
        glDeleteShader(*shader_id_ptr);
        *shader_id_ptr = 0;
        return false;
    }

    delete[] buffer;
    return true;
}



