#ifndef ARRAY2D_H_INCLUDED
#define ARRAY2D_H_INCLUDED

#include <algorithm>
#include <array>

/**
 * @class Array2D
 * @brief A 2D array class for storing elements of type T.
 * 
 * @details
 * This class provides a 2D array implementation using std::array.
 * It allows for easy access to elements using x and z coordinates.
 * The class also provides methods to get the maximum value in the array
 * and to set all elements to a specific value.
 */
template <typename T, int WIDTH> class Array2D {
    using Array = std::array<T, WIDTH * WIDTH>;

  public:
    /**
     * @brief Gets the element at the specified x and z coordinates.
     * 
     * @param x The x coordinate of the element.
     * @param z The z coordinate of the element.
     * 
     * @return A reference to the element at the specified coordinates.
     * 
     * @details
     * This method returns a reference to the element at the specified
     * x and z coordinates in the 2D array. The coordinates are used to
     * calculate the index in the underlying 1D array.
     */
    T &get(int x, int z)
    {
        return m_array[x * WIDTH + z];
    }

    /**
     * @brief Gets the element at the specified x and z coordinates (const).
     * 
     * @param x The x coordinate of the element.
     * @param z The z coordinate of the element.
     * 
     * @return A const reference to the element at the specified coordinates.
     * 
     * @details
     * This method returns a const reference to the element at the specified
     * x and z coordinates in the 2D array. The coordinates are used to
     * calculate the index in the underlying 1D array.
     */
    const T &get(int x, int z) const
    {
        return m_array[x * WIDTH + z];
    }

    /**
     * @brief Gets the maximum value in the array.
     * 
     * @return A reference to the maximum value in the array.
     * 
     * @details
     * This method returns a reference to the maximum value in the
     * 2D array. It uses std::max_element to find the maximum value
     * in the underlying 1D array.
     */
    T &getMaxValue()
    {
        return *std::max_element(m_array.begin(), m_array.end());
    }

    /**
     * @brief Sets all elements in the array to the specified value.
     * 
     * @param val The value to set all elements to.
     * 
     * @details
     * This method sets all elements in the 2D array to the specified
     * value. It uses std::fill to set all elements in the underlying
     * 1D array to the specified value.
     */
    void setAll(T val)
    {
        m_array.fill(val);
    }

  private:
    Array m_array;
};

#endif // ARRAY2D_H_INCLUDED
