#include "RenderMaster.h"

#include <SFML/Graphics.hpp>
#include <iostream>

#include "../Application.h"
#include "../Context.h"
#include "../World/Chunk/ChunkMesh.h"
#include "../World/Chunk/ChunkSection.h"

/**
 * @brief Renders the world using OpenGL.
 */

/**
 * @brief Draws a chunk section.
 * 
 * @param chunk The chunk section to draw.
 * 
 * @details
 * This method adds the chunk's solid, water, and flora meshes to their respective
 * renderers. It checks if each mesh has faces before adding it to the renderer.
 */
void RenderMaster::drawChunk(const ChunkSection &chunk)
{
    const auto &solidMesh = chunk.getMeshes().solidMesh;
    const auto &waterMesh = chunk.getMeshes().waterMesh;
    const auto &floraMesh = chunk.getMeshes().floraMesh;

    if (solidMesh.faces > 0)
        m_chunkRenderer.add(solidMesh);

    if (waterMesh.faces > 0)
        m_waterRenderer.add(waterMesh);

    if (floraMesh.faces > 0)
        m_floraRenderer.add(floraMesh);
}

/**
 * @brief Draws the skybox.
 * 
 * @details
 * This method sets a flag to indicate that the skybox should be drawn.
 * The actual rendering of the skybox is handled in the finishRender method.
 */
void RenderMaster::drawSky()
{
    m_drawBox = true;
}

/**
 * @brief Finishes the rendering process.
 * 
 * @param window The SFML window to display the rendered content.
 * @param camera The camera used for rendering.
 * 
 * @details
 * This method clears the screen, enables depth testing and face culling,
 * and renders the chunk, water, flora, and skybox (if applicable).
 */
void RenderMaster::finishRender(sf::RenderWindow &window, const Camera &camera)
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    m_chunkRenderer.render(camera);
    m_waterRenderer.render(camera);
    m_floraRenderer.render(camera);

    if (m_drawBox) {
        glDisable(GL_CULL_FACE);
        m_skyboxRenderer.render(camera);
        m_drawBox = false;
    }

    window.display();
}
