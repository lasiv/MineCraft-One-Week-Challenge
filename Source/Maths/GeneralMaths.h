#ifndef GENERALMATHS_H_INCLUDED
#define GENERALMATHS_H_INCLUDED

/**
 * @brief Smooth interpolation function that uses smoothstep for interpolation.
 * 
 * @param bottomLeft The value at the bottom left corner.
 * @param topLeft The value at the top left corner.
 * @param bottomRight The value at the bottom right corner.
 * @param topRight The value at the top right corner.
 * @param xMin The minimum x value.
 * @param xMax The maximum x value.
 * @param zMin The minimum z value.
 * @param zMax The maximum z value.
 * @param x The x value to interpolate.
 * @param z The z value to interpolate.
 * 
 * @return The interpolated value.
 * 
 * @details
 * This function performs smooth interpolation between four corner values
 * (bottomLeft, topLeft, bottomRight, topRight) based on the provided x and z
 * coordinates. It uses the smoothstep function to create a smooth transition
 * between the values. The x and z coordinates are normalized based on the
 * provided minimum and maximum values (xMin, xMax, zMin, zMax).
 */
float bilinearInterpolation(float bottomLeft, float topLeft, float bottomRight,
                            float topRight, float xMin, float xMax, float zMin,
                            float zMax, float xToCalc, float yToCalc);

float smoothInterpolation(float bottomLeft, float topLeft, float bottomRight,
                          float topRight, float xMin, float xMax, float zMin,
                          float zMax, float x, float z);

#endif // GENERALMATHS_H_INCLUDED
