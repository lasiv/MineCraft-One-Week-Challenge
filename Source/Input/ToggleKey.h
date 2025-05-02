#ifndef TOGGLEKEY_H_INCLUDED
#define TOGGLEKEY_H_INCLUDED

#include <SFML/Graphics.hpp>

/**
 * @class ToggleKey
 * @brief A class that manages a toggle key input.
 * 
 * @details
 * This class is used to debounce key presses for toggle functionality.
 * It uses a timer to ensure that the key press is registered only if a certain
 * delay has elapsed since the last key press. This is useful for preventing
 * multiple registrations of a key press when the key is held down.
 * 
 * @todo refactor the class to be named appropriately, as it is not a toggle key.
 */
class ToggleKey {
  public:
    ToggleKey(sf::Keyboard::Key);

    /**
     * @brief Checks if the key is pressed.
     * 
     * @return True if the key is pressed and the delay has elapsed, false otherwise.
     * 
     * @details
     * This function checks if the key is pressed and manages the delay between key presses.
     * It uses a timer to ensure that the key press is registered only if the delay has
     * elapsed.
     */
    bool isKeyPressed();

  private:
    sf::Keyboard::Key m_key;
    sf::Clock m_delayTimer;
};

#endif // TOGGLEKEY_H_INCLUDED
