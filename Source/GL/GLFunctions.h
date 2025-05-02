#ifndef GLFUNCTIONS_H_INCLUDED
#define GLFUNCTIONS_H_INCLUDED

#include <glad/glad.h>

namespace GL {

/**
 * @brief Wrapper function for glDrawElements.
 * 
 * @param indicesCount The number of indices to draw.
 * 
 * @details
 * This function calls glDrawElements with the specified number of indices.
 * It is a convenience function to simplify the drawing process.
 * 
 * The function uses the GL_TRIANGLES mode and GL_UNSIGNED_INT type for the indices.
 */
void drawElements(GLuint indicesCount) noexcept;

/**
 * @brief Wrapper function for glBindVertexArray.
 * 
 * @param vao The Vertex Array Object to bind.
 * 
 * @details
 * This function binds the specified Vertex Array Object (VAO).
 * It is a convenience function to simplify the binding process.
 */
void bindVAO(GLuint vao) noexcept;

namespace Enum {
enum Texture { Tex2D = GL_TEXTURE_2D, TexCubeMap = GL_TEXTURE_CUBE_MAP };
}
} // namespace GL

#endif // GLFUNCTIONS_H_INCLUDED
