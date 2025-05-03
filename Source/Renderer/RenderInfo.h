#ifndef RENDERINFO_H_INCLUDED
#define RENDERINFO_H_INCLUDED

/**
 * @struct RenderInfo
 * @brief Structure to hold rendering information.
 * 
 * @details
 * This structure holds the rendering information for a specific object
 * in the OpenGL context. It includes the Vertex Array Object (VAO) ID
 * and the count of indices used for rendering. The VAO is used to
 * encapsulate the vertex buffer objects and their attributes, while
 * the indices count is used to determine how many vertices to render
 * when drawing the object.
 */
struct RenderInfo {
    GLuint vao = 0;
    GLuint indicesCount = 0;

    /**
     * @brief Resets the rendering information.
     * 
     * @details
     * This method resets the rendering information by setting the
     * Vertex Array Object (VAO) ID to 0 and the indices count to 0.
     */
    inline void reset()
    {
        vao = 0;
        indicesCount = 0;
    }
};

#endif // RENDERINFO_H_INCLUDED
