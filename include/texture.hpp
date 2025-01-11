#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <string>

/**
 * Handles the creation, binding, and configuration of 2D textures in OpenGL.
 *
 * @note Default wrap mode is GL_REPEAT and default filtering mode is GL_LINEAR
 * (mipmap for min filter).
 */
class Texture {
public:
    unsigned int ID;
    int width, height, nrChannels;

    Texture(const std::string &path) {
        stbi_set_flip_vertically_on_load(true);
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(
                GL_TEXTURE_2D, 0, format, width, height,
                0, format, GL_UNSIGNED_BYTE, data
            );
            glGenerateMipmap(GL_TEXTURE_2D);
        } else {
            std::cout << "Failed to load texture: " << path << std::endl;
        }
        stbi_image_free(data);
    }

    void bind() const {
        glBindTexture(GL_TEXTURE_2D, ID);
    }

    /**
     * @brief Sets the wrapping mode for the texture.
     *
     * Possible modes are GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, and
     * GL_CLAMP_TO_BORDER. Visual:
     * https://learnopengl.com/img/getting-started/texture_wrapping.png
     *
     * @param wrapS width
     * @param wrapT (optional) height. Takes the value of wrapS if not
     * supplied.
     */
    void setWrap(GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT) {
        bind();
        if (wrapT == wrapS) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapS);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
        }
    }

    /**
     * @brief Sets the filtering mode for the texture.
     * 
     * Possible modes are GL_NEAREST and GL_LINEAR. Use nearest neighbor for
     * pixelated effect and linear interpolation for smoothing.
     *
     * @param minFilter Controls how texture looks when it is smaller than the
     * surface it is being applied to.
     * @param magFilter (optional) Controls how texture looks when it is larger than the
     * surface it is being applied to. Takes value of minFilter if not supplied.
     */
    void setFilter(GLenum filterMin = GL_NEAREST, GLenum filterMag = GL_NEAREST) {
        bind();
        if (filterMag == filterMin) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMin);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMag);
        }
    }
};

#endif