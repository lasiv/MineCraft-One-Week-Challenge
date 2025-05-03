#ifndef FLORARENDERER_H_INCLUDED
#define FLORARENDERER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>

#include "../Shaders/FloraShader.h"

struct RenderInfo;
class ChunkMesh;
class Camera;

/**
 * @class FloraRenderer
 * @brief Class to render flora in the world.
 * 
 * @details
 * This class is responsible for rendering flora in the world. It manages
 * the rendering information for each flora chunk and uses a shader program
 * to render the flora. The class provides methods to add flora chunks
 * and render them using the camera's view and projection matrices.
 */
class FloraRenderer {
  public:
    /**
     * @brief Adds a chunk mesh to the renderer.
     * 
     * @param mesh The chunk mesh to add.
     * 
     * @details
     * This method adds a chunk mesh to the renderer's list of flora chunks to be
     * rendered. The mesh is passed by reference, and the method stores a pointer
     * to the render information of the mesh. This allows the renderer to
     * efficiently render the mesh later without needing to copy the entire
     * mesh data.
     */
    void add(const ChunkMesh &mesh);
    
    /**
     * @brief Renders the flora chunks using the provided camera.
     * 
     * @param camera The camera used for rendering.
     * 
     * @details
     * This method renders the flora chunks using the provided camera's view
     * and projection matrices. It first checks if there are any chunks to render.
     * If there are no chunks, the method returns early. It then disables blending
     * and enables face culling to optimize rendering. The shader program is
     * activated, and the texture atlas is bound for rendering. The projection
     * and view matrices are loaded into the shader program. Finally, the method
     * iterates through the list of flora chunks, binds the Vertex Array Object
     * (VAO) for each chunk, and draws the elements using the indices count.
     * After rendering, the list of chunks is cleared to prepare for the next
     * rendering cycle.
     */
    void render(const Camera &camera);

  private:
    std::vector<const RenderInfo *> m_chunks;

    FloraShader m_shader;
};

#endif // FLORARENDERER_H_INCLUDED
