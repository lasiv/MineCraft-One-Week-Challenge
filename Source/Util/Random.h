#ifndef RANDOM_H_INCLUDED
#define RANDOM_H_INCLUDED

#include <ctime>
#include <random>

#include "Singleton.h"

/**
 * @class RandomSingleton
 * @brief A singleton class for generating random numbers.
 * 
 * @details
 * This class provides a singleton instance for generating random numbers
 * using a random engine. It uses the Mersenne Twister algorithm for
 * generating random numbers. The class provides a method to generate
 * random integers within a specified range.
 */
class RandomSingleton : public Singleton {
  public:
    /**
     * @brief Gets the singleton instance of RandomSingleton.
     * 
     * @return A reference to the singleton instance of RandomSingleton.
     * 
     * @details
     * This method returns a reference to the singleton instance of
     * RandomSingleton. It ensures that only one instance of the class
     * exists throughout the application.
     */
    static RandomSingleton &get();

    /**
     * @brief Generates a random integer within the specified range.
     * 
     * @param low The lower bound of the range (inclusive).
     * @param high The upper bound of the range (inclusive).
     * @return A random integer within the specified range.
     * 
     * @details
     * This method generates a random integer using a uniform distribution
     * within the specified range. The range is inclusive, meaning that
     * both the lower and upper bounds can be returned as possible values.
     * The method uses the Mersenne Twister algorithm for generating random
     * numbers, which provides a good balance between speed and randomness.
     */
    template <typename T> T intInRange(T low, T high)
    {
        static_assert(std::is_integral<T>::value, "Not integral type!");
        std::uniform_int_distribution<T> dist(low, high);
        return dist(m_randomEngine);
    }

  private:
    /**
     * @brief Private constructor for RandomSingleton.
     * 
     * @details
     * This constructor initializes the random engine with a seed based on
     * the current time. It also generates a few random integers in the
     * range of 0 to 5, which can be used to increase randomness.
     */
    RandomSingleton();

    std::mt19937 m_randomEngine;
};

/**
 * @class Random
 * @brief A class for generating random numbers.
 * 
 * @details
 * This class provides a random number generator using a specified random
 * engine. It allows generating random integers within a specified range.
 * The class can be used to create random numbers for various purposes,
 * such as game mechanics, procedural generation, etc.
 */
template <typename REngine = std::mt19937> class Random {
  public:
    /**
     * @brief Constructor for Random class.
     * 
     * @param n The seed for the random engine (default is current time).
     * 
     * @details
     * This constructor initializes the random engine with a seed based on
     * the current time. It also generates a few random integers in the
     * range of 0 to 5, which can be used to increase randomness.
     */
    Random(int n = std::time(nullptr))
    {
        m_randomEngine.seed(n);
        for (int i = 0; i < 5; i++)
            intInRange(i, i * 5);
    }

    /**
     * @brief Generates a random integer within the specified range.
     * 
     * @param low The lower bound of the range (inclusive).
     * @param high The upper bound of the range (inclusive).
     * @return A random integer within the specified range.
     * 
     * @details
     * This method generates a random integer using a uniform distribution
     * within the specified range. The range is inclusive, meaning that
     * both the lower and upper bounds can be returned as possible values.
     * The method uses the Mersenne Twister algorithm for generating random
     * numbers, which provides a good balance between speed and randomness.
     */
    template <typename T> T intInRange(T low, T high)
    {
        static_assert(std::is_integral<T>::value, "Not integral type!");
        std::uniform_int_distribution<T> dist(low, high);
        return dist(m_randomEngine);
    }

    /**
     * @brief Sets the seed for the random engine.
     * 
     * @param seed The seed value to set for the random engine.
     * 
     * @details
     * This method allows setting a custom seed for the random engine,
     * which can be useful for reproducibility in random number generation.
     */
    void setSeed(int seed)
    {
        m_randomEngine.seed(seed);
    }

  private:
    REngine m_randomEngine;
};

#endif // RANDOM_H_INCLUDED
