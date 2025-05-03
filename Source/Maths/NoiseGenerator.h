#ifndef NOISEGENERATOR_H_INCLUDED
#define NOISEGENERATOR_H_INCLUDED

/**
 * 
 */

/**
 * @struct NoiseParameters
 * @brief Structure to hold noise generation parameters.
 * 
 * @details
 * This structure holds the parameters used for generating noise in the
 * terrain generation process. It includes the number of octaves, amplitude,
 * smoothness, height offset, and roughness. 
 */
struct NoiseParameters {
    int octaves;
    int amplitude;
    int smoothness;
    int heightOffset;

    double roughness;
};

/**
 * @class NoiseGenerator
 * @brief Class to generate noise for terrain generation.
 * 
 * @details
 * This class is used to generate noise for terrain generation using a
 * pseudo-random number generator. It provides methods to get the height
 * at a specific point in the terrain based on the noise parameters.
 * The noise is generated using a combination of octaves, amplitude,
 * smoothness, and roughness. The height is calculated based on the
 * noise value and the height offset.
 */
class NoiseGenerator {
  public:
    NoiseGenerator(int seed);

    /**
     * @brief Gets the height of the terrain at the specified coordinates.
     * 
     * @param x The x coordinate.
     * @param z The z coordinate.
     * @param chunkX The chunk x coordinate.
     * @param chunkZ The chunk z coordinate.
     * 
     * @return The height value at the specified coordinates.
     * 
     * @details
     * This method calculates the height of the terrain at the specified
     * coordinates (x, z) within a chunk (chunkX, chunkZ). It uses noise
     * generation to create a height value based on the noise parameters.
     */
    double getHeight(int x, int z, int chunkX, int chunkZ) const noexcept;

    /**
     * @brief Sets the parameters for the noise generation.
     * 
     * @param params The noise parameters to set.
     * 
     * @details
     * This method sets the parameters for the noise generation, including
     * octaves, amplitude, smoothness, height offset, and roughness.
     */
    void setParameters(const NoiseParameters &params) noexcept;

  private:
    /**
     * @brief Gets noise value based on the provided integer seed.
     * 
     * @param n The integer seed value.
     * 
     * @return The noise value generated from the seed.
     * 
     * @details
     * This method generates a pseudo-random noise value based on the provided
     * integer seed. It uses bitwise operations and mathematical functions to
     * create a noise value that is consistent for the same seed.
     */
    double getNoise(int n) const noexcept;

    /**
     * @brief Gets noise value based on the provided double coordinates.
     * 
     * @param x The x coordinate.
     * @param z The z coordinate.
     * 
     * @return The noise value generated from the coordinates.
     * 
     * @details
     * This method generates a pseudo-random noise value based on the provided
     * double coordinates. It uses the getNoise(int n) method to generate the
     * noise value based on the integer representation of the coordinates.
     */
    double getNoise(double x, double z) const noexcept;

    /**
     * @brief Performs linear interpolation between two values.
     * 
     * @param a The first value.
     * @param b The second value.
     * @param z The interpolation factor.
     * 
     * @return The interpolated value.
     * 
     * @details
     * This method performs linear interpolation between two values (a and b)
     * based on the provided interpolation factor (z). It uses the cosine
     * function to create a smooth transition between the two values.
     */
    double lerp(double a, double b, double z) const noexcept;

    /**
     * @brief Generates noise value based on the provided x and z coordinates.
     * 
     * @param x The x coordinate.
     * @param z The z coordinate.
     * 
     * @return The noise value generated from the coordinates.
     * 
     * @details
     * This method generates noise value based on the provided x and z coordinates
     * using bilinear interpolation. It calculates the surrounding values and
     * performs linear interpolation to create a smooth transition between the
     * values.
     */
    double noise(double x, double z) const noexcept;

    NoiseParameters m_noiseParameters;

    int m_seed;
};

#endif // NOISEGENERATOR_H_INCLUDED
