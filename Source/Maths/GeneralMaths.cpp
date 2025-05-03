#include "GeneralMaths.h"

#include <iostream>

/**
 * @brief Clamps a value between a lower and upper limit.
 * 
 * @param x The value to clamp.
 * @param lowerlimit The lower limit.
 * @param upperlimit The upper limit.
 * 
 * @return The clamped value.
 * 
 * @details
 * This function clamps the provided value x between the specified lower and
 * upper limits. If x is less than the lower limit, it is set to the lower limit.
 * If x is greater than the upper limit, it is set to the upper limit. The
 * result is the clamped value, which is guaranteed to be within the specified
 * range [lowerlimit, upperlimit].
 * 
 * @note
 * This function is unused
 * 
 * @todo potentially add this to header for global use or remove if not needed
 */
float clamp(float x, float lowerlimit, float upperlimit);

/**
 * @brief Smoothstep function that interpolates between two edges.
 * 
 * @param edge0 The lower edge of the interpolation.
 * @param edge1 The upper edge of the interpolation.
 * @param x The value to interpolate.
 * 
 * @return The interpolated value.
 * 
 * @details
 * This function performs smooth interpolation between two edges (edge0 and edge1)
 * based on the provided value x. It uses a polynomial function to create a
 * smooth transition between the two edges. The value x is scaled, biased, and
 * saturated to the range [0, 1] before applying the polynomial function.
 * The result is a smooth transition between edge0 and edge1.
 */
float smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = x * x * (3 - 2 * x);
    // Evaluate polynomial
    return (edge0 * x) + (edge1 * (1 - x));
}


float clamp(float x, float lowerlimit, float upperlimit)
{
    if (x < lowerlimit)
        x = lowerlimit;
    if (x > upperlimit)
        x = upperlimit;
    return x;
}

float smoothInterpolation(float bottomLeft, float topLeft, float bottomRight,
                          float topRight, float xMin, float xMax, float zMin,
                          float zMax, float x, float z)
{
    float width = xMax - xMin, height = zMax - zMin;
    float xValue = 1 - (x - xMin) / width;
    float zValue = 1 - (z - zMin) / height;

    // std::cout << xValue << std::endl;

    float a = smoothstep(bottomLeft, bottomRight, xValue);
    float b = smoothstep(topLeft, topRight, xValue);
    return smoothstep(a, b, zValue);
}

float bilinearInterpolation(float bottomLeft, float topLeft, float bottomRight,
                            float topRight, float xMin, float xMax, float zMin,
                            float zMax, float x, float z)
{
    float width = xMax - xMin, height = zMax - zMin,

          xDistanceToMaxValue = xMax - x, zDistanceToMaxValue = zMax - z,

          xDistanceToMinValue = x - xMin, zDistanceToMinValue = z - zMin;

    return 1.0f / (width * height) *
           (bottomLeft * xDistanceToMaxValue * zDistanceToMaxValue +
            bottomRight * xDistanceToMinValue * zDistanceToMaxValue +
            topLeft * xDistanceToMaxValue * zDistanceToMinValue +
            topRight * xDistanceToMinValue * zDistanceToMinValue);
}
