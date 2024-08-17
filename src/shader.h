#ifndef __SHADER__
#define __SHADER__

#include <unordered_map>
#include <string>
#include <GL/glew.h>

class Shader
{
public:
    Shader();
    ~Shader();

    bool add_vertex_shader(const std::string& path);
    bool add_geometry_shader(const std::string& path);
    bool add_fragment_shader(const std::string& path);
    bool add_compute_shader(const std::string& path);
    bool build_shader();

    void work(bool b_work = true) const;

private:
    enum struct ShaderType
    {
        VETEX_SHADER,
        GEOMETRY_SHADER,
        FRAGMENT_SHADER,
        COMPUTE_SHADER,
    };

    bool add_shader(ShaderType type, const std::string& path);
    GLint get_uniform_location(const std::string& name) const;

private:
    GLuint m_vertex_shader_id;
    GLuint m_geometry_shader_id;
    GLuint m_fragment_shader_id;
    GLuint m_compute_shader_id;
    GLuint m_program_id;
    mutable std::unordered_map<std::string, GLint> mp;  // uniform name -> uniform location
};


#endif // __SHADER__
