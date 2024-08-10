#ifndef __TEXTURE__
#define __TEXTURE__

#include <string>
#include <GL/glew.h>

class Texture
{
public:
    enum class ChannelType
    {
        NONE,
        GRAY,
        RGB,
        RGBA,
        CHANNEL_TYPE_COUNT
    };
    enum class DataType
    {
        NONE,
        UNSIGNED_BYTE,
        FLOAT,
        DATA_TYPE_COUNT
    };
    enum class Access
    {
        READ,
        WRITE,
        READ_WRITE,
    };
public:
    Texture(unsigned width, unsigned height, ChannelType channel_type, DataType data_type,
        void* data = nullptr, ChannelType internal_channel_type = ChannelType::NONE,
        DataType internal_data_type = DataType::NONE);
    ~Texture();

    unsigned get_id() const { return m_id; }
    void activate(unsigned slot);
    bool set_access_for_shader(Access access);
    // return data size in byte
    size_t get_size(unsigned* pwidth = nullptr, unsigned* pheight = nullptr,
        ChannelType* pchannel_type = nullptr, DataType* pdata_type = nullptr);
    void resize(unsigned width, unsigned height, void* buffer = nullptr);
    bool get_data(void* buffer, int buffer_size, int x = -1, int y = -1, int width = -1, int height = -1);
    bool set_data(void* buffer, int x = -1, int y = -1, int width = -1, int height = -1);
    bool save_as_ppm(const std::string& file_path);

private:
    inline int get_internal_type(ChannelType ct, DataType dt)
    {
        static int table[size_t(ChannelType::CHANNEL_TYPE_COUNT) - 1][size_t(DataType::DATA_TYPE_COUNT) - 1] = {
            GL_R8, GL_R32F,
            GL_RGB8, GL_RGB32F,
            GL_RGBA8, GL_RGBA32F
        };
        return table[size_t(ct) - 1][size_t(dt) - 1];
    }

    GLenum get_gl_channel_type();
    GLenum get_gl_data_type();
private:
    GLuint m_id;
    unsigned m_width;
    unsigned m_height;
    int m_slot;
    ChannelType m_channel_type;
    DataType m_data_type;
};



#endif // __TEXTURE__
