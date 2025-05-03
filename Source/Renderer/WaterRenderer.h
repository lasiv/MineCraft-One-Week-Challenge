#ifndef WATERRENDERER_H_INCLUDED
#define WATERRENDERER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <vector>

#include "../Shaders/WaterShader.h"

struct RenderInfo;
class ChunkMesh;
class Camera;

/**
 * @class WaterRenderer
 * @brief Class to render water in the world.
 * 
 * @details
 * This class is responsible for rendering water in the world. It manages
 * the rendering information for each water chunk and uses a shader program
 * to render the water. 
 */
class WaterRenderer {
  public:
    /**
     * @brief Adds a chunk mesh to the renderer.
     * 
     * @param mesh The chunk mesh to add.
     * 
     * @details
     * This method adds a chunk mesh to the renderer's list of water chunks to be
     * rendered. The mesh is passed by reference, and the method stores a pointer
     * to the render information of the mesh. This allows the renderer to
     * efficiently render the mesh later without needing to copy the entire
     * mesh data.
     */
    void add(const ChunkMesh &mesh);

    /**
     * @brief Renders the water chunks using the provided camera.
     * 
     * @param camera The camera used for rendering.
     * 
     * @details
     * This method renders the water chunks using the provided camera's view and
     * projection matrices. It first checks if there are any chunks to render.
     * If there are no chunks, the method returns early. It then enables blending
     * and disables face culling to optimize rendering. The shader program is
     * activated, and the texture atlas is bound for rendering. The projection
     * and view matrices are loaded into the shader program. Finally, the method
     * iterates through the list of water chunks, binds the Vertex Array Object
     * (VAO) for each chunk, and draws the elements using the indices count.
     * After rendering, the list of chunks is cleared to prepare for the next
     * rendering cycle.
     */
    void render(const Camera &camera);

  private:
    std::vector<const RenderInfo *> m_chunks;

    WaterShader m_shader;
};

#endif // WATERRENDERER_H_INCLUDED
