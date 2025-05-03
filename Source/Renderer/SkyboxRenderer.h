#ifndef SKYBOXRENDERER_H_INCLUDED
#define SKYBOXRENDERER_H_INCLUDED

#include "../Model.h"
#include "../Shaders/SkyboxShader.h"
#include "../Texture/CubeTexture.h"

class Camera;

/**
 * @class SkyboxRenderer
 * @brief Class to render a skybox in the 3D world.
 * 
 * @details
 * This class is responsible for rendering a skybox in the 3D world. It uses
 * a cube texture to represent the skybox and a shader program to render
 * the skybox. The class provides methods to initialize the skybox and
 * render it using the camera's view and projection matrices. The skybox
 * is rendered as a cube that surrounds the camera, creating the illusion
 * of a distant background.
 */
class SkyboxRenderer {
  public:
    SkyboxRenderer();

    /**
     * @brief Renders the skybox using the provided camera.
     * 
     * @param camera The camera used for rendering.
     * 
     * @details
     * This method renders the skybox using the provided camera's view and
     * projection matrices. It first uses the shader program for rendering
     * the skybox. The Vertex Array Object (VAO) for the skybox is bound,
     * and the cube texture is bound for rendering. The view and projection
     * matrices are loaded into the shader program. Finally, the method
     * draws the elements of the skybox using the indices count from the
     * skybox model.
     */
    void render(const Camera &camera);

  private:
    Model m_skyCube;
    SkyboxShader m_shader;
    CubeTexture m_cubeTexture;
};

#endif // SKYBOXRENDERER_H_INCLUDED
