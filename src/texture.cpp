#include <iostream>
#include <fstream>
#include <stdexcept>

#include "texture.h"

inline int saturate(int value, int min = 0, int max = 255)
{
    return value > max ? max : value < min ? min : value;
}

Texture::Texture(unsigned width, unsigned height, ChannelType channel_type, DataType data_type,
    void* data, ChannelType internal_channel_type, DataType internal_data_type) :
    m_width(width),
    m_height(height),
    m_slot(-1),
    m_channel_type(channel_type),
    m_data_type(data_type)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = get_gl_channel_type();
    if(format == GL_NONE)
        throw std::runtime_error("channel_type not supported!");

    GLenum type = get_gl_data_type();
    if(type == GL_NONE)
        throw std::runtime_error("data_type not supported!");

    if(internal_channel_type == ChannelType::NONE)
        internal_channel_type = channel_type;
    if(internal_data_type == DataType::NONE)
        internal_data_type = data_type;

    int inner_format = get_internal_type(internal_channel_type, internal_data_type);
    glTexImage2D(GL_TEXTURE_2D, 0, inner_format,
        width, height, 0, format, type, data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_id);
}

void Texture::activate(unsigned slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_id);
    m_slot = slot;
}

bool Texture::set_access_for_shader(Access access)
{
    int mode;
    switch(access)
    {
    case Access::READ:
        mode = GL_READ_ONLY;
        break;
    case Access::WRITE:
        mode = GL_WRITE_ONLY;
        break;
    case Access::READ_WRITE:
        mode = GL_READ_WRITE;
        break;
    default:
        return false;
    }
    if(m_slot < 0)
        activate(0);
    glBindImageTexture(m_slot, m_id, 0, GL_FALSE, 0, mode, get_internal_type(m_channel_type, m_data_type));
    return true;
}

size_t Texture::get_size(unsigned* pwidth, unsigned* pheight, ChannelType* pchannel_type, DataType* pdata_type)
{
    unsigned channels;
    switch(m_channel_type)
    {
    case ChannelType::GRAY:
        channels = 1;
        break;
    case ChannelType::RGB:
        channels = 3;
        break;
    case ChannelType::RGBA:
        channels = 4;
        break;
    default:
        return -1;
    }
    unsigned element_size;
    switch(m_data_type)
    {
    case DataType::UNSIGNED_BYTE:
        element_size = sizeof(unsigned char);
        break;
    case DataType::FLOAT:
        element_size = sizeof(float);
        break;
    default:
        return -1;
    }

    if(pwidth)
        *pwidth = m_width;
    if(pheight)
        *pheight = m_height;
    if(pchannel_type)
        *pchannel_type = m_channel_type;
    if(pdata_type)
        *pdata_type = m_data_type;

    return size_t(m_width) * m_height * channels * element_size;
}

void Texture::resize(unsigned width, unsigned height, void* buffer)
{
    m_width = width;
    m_height = height;

    glTexImage2D(GL_TEXTURE_2D, 0, get_internal_type(m_channel_type, m_data_type),
        width, height, 0, get_gl_channel_type(), get_gl_data_type(), buffer);
}

bool Texture::get_data(void* buffer, int buffer_size, int x, int y, int width, int height)
{
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(width < 0) width = m_width;
    if(height < 0) height = m_height;
    if(x + width > m_width || y + height > m_height)
        return false;

    glGetTextureSubImage(m_id, 0, x, y, 0, width, height, 1,
        get_gl_channel_type(), get_gl_data_type(), buffer_size, buffer);
    return true;
}

bool Texture::set_data(void* buffer, int x, int y, int width, int height)
{
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(width < 0) width = m_width;
    if(height < 0) height = m_height;
    if(x + width > m_width || y + height > m_height)
        return false;

    glTextureSubImage2D(m_id, 0, x, y, width, height,
        get_gl_channel_type(), get_gl_data_type(), buffer);
    return true;
}

bool Texture::set_data(glm::vec3 color, int x, int y, int width, int height)
{
    if(m_data_type != DataType::FLOAT)
    {
        std::cerr << "texture data type is not float, can not set float vec3 data.\n";
        return false;
    }
    if(x < 0) x = 0;
    if(y < 0) y = 0;
    if(width < 0) width = m_width;
    if(height < 0) height = m_height;
    if(x + width > m_width || y + height > m_height)
        return false;
    float* buffer = new float[width * height * 3];
    for(int i = 0; i < width * height; ++i)
    {
        int idx = i * 3;
        buffer[idx++] = color.r;
        buffer[idx++] = color.g;
        buffer[idx++] = color.b;
    }
    glTextureSubImage2D(m_id, 0, x, y, width, height,
        get_gl_channel_type(), get_gl_data_type(), buffer);
    delete[] buffer;
    return true;
}

// ppm p6 RGB 0-255
bool Texture::save_as_ppm(const std::string& file_path)
{
    if(!m_id)
        return false;

    std::ofstream fs(file_path, std::ios::binary | std::ios::out);
    if(!fs)
        return false;

    int buffer_size = get_size();
    int pixels = m_width * m_height;
    void* buf = new char[buffer_size];

    fs << "P6\n" << std::to_string(m_width) << " " << std::to_string(m_height) << "\n255\n";

    // glGetTextureImage(m_id, 0, get_gl_channel_type(), get_gl_data_type(), buffer_size, buf);
    get_data(buf, buffer_size);
    if(DataType::UNSIGNED_BYTE == m_data_type)
    {
        unsigned char* pc = (unsigned char*)buf;
        for(int i = 0; i < pixels; ++i)
        {
            fs << saturate(*pc++);
            fs << saturate(*pc++);
            fs << saturate(*pc++);
            if(ChannelType::RGBA == m_channel_type)
                pc++;
        }
    }
    else if(DataType::FLOAT == m_data_type)
    {
        float* pf = (float*)buf;
        for(int i = 0; i < pixels; ++i)
        {
            fs << char(saturate(int((*pf++) * 255.999f)));
            fs << char(saturate(int((*pf++) * 255.999f)));
            fs << char(saturate(int((*pf++) * 255.999f)));
            if(ChannelType::RGBA == m_channel_type)
                pf++;
        }
    }
    else
        return false;

    delete[] buf;
    return true;
}

GLenum Texture::get_gl_channel_type()
{
    switch(m_channel_type)
    {
    case ChannelType::GRAY:
        return GL_RED;
    case ChannelType::RGB:
        return GL_RGB;
    case ChannelType::RGBA:
        return GL_RGBA;
    }
    return GL_NONE;
}

GLenum Texture::get_gl_data_type()
{
    switch(m_data_type)
    {
    case Texture::DataType::UNSIGNED_BYTE:
        return GL_UNSIGNED_BYTE;
    case Texture::DataType::FLOAT:
        return GL_FLOAT;
    }
    return GL_NONE;
}
