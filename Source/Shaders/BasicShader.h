#ifndef BASICSHADER_H_INCLUDED
#define BASICSHADER_H_INCLUDED

#include "Shader.h"

/**
 * @class BasicShader
 * @brief A shader class for rendering basic objects.
 * 
 * @details
 * This class is derived from the Shader class and is used to load and manage
 * the vertex and fragment shaders for basic rendering. It provides methods
 * to load projection and model matrices into the shader program.
 */
class BasicShader : public Shader {
  public:
    BasicShader(const std::string &vertexFile = "Basic",
                const std::string &fragmentFile = "Basic");

    void loadProjectionViewMatrix(const glm::mat4 &pvMatrix);
    void loadModelMatrix(const glm::mat4 &matrix);

  protected:
    virtual void getUniforms() override;

  private:
    GLuint m_locationProjectionViewMatrix;
    GLuint m_locationModelMatrix;
};

#endif // BASICSHADER_H_INCLUDED
