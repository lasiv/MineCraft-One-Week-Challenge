#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include "glm.h"

class Camera;

struct Entity;
struct Config;

/**
 * @brief Creates a model matrix for the given entity.
 * 
 * @param entity The entity to create the model matrix for.
 * 
 * @return The model matrix for the entity.
 * 
 * @details
 * This function creates a model matrix for the given entity by applying
 * rotation and translation transformations based on the entity's position
 * and rotation. The model matrix is used to transform the entity's vertices
 * from local space to world space.
 */
glm::mat4 makeModelMatrix(const Entity &entity);

/**
 * @brief Creates a view matrix for the given camera.
 * 
 * @param camera The camera to create the view matrix for.
 * 
 * @return The view matrix for the camera.
 * 
 * @details
 * This function creates a view matrix for the given camera by applying
 * rotation and translation transformations based on the camera's position
 * and rotation. The view matrix is used to transform the world coordinates
 * into camera coordinates.
 */
glm::mat4 makeViewMatrix(const Camera &camera);

/**
 * @brief Creates a projection matrix based on the given configuration.
 * 
 * @param config The configuration containing window dimensions and field of view.
 * 
 * @return The projection matrix.
 * 
 * @details
 * This function creates a projection matrix based on the given configuration,
 * which includes the window dimensions (width and height) and the field of view
 * (FOV) in degrees. The projection matrix is used to transform 3D coordinates
 * into 2D screen coordinates.
 */
glm::mat4 makeProjectionMatrix(const Config &config);

#endif // MATRIX_H_INCLUDED
