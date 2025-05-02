#pragma once

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <array>

/**
 * @class Keyboard
 * @brief A class that manages keyboard input events.
 * 
 * @details
 * This class handles keyboard input events, allowing you to check the state of keys
 * and whether they were recently pressed or released. It uses SFML's event system
 * to update the state of keys and provides methods to query the state of specific keys.
 */
class Keyboard {
  public:
    Keyboard();

    /**
     * @brief Updates the keyboard state based on the given event.
     * 
     * @param e The event to process.
     * 
     * @details 
     * This function updates the internal state of the keyboard based on the event type.
     * It handles key press and release events, updating the corresponding key states.
     * The recently released key is also tracked for further processing.
     */
    void update(sf::Event e);

    /**
     * @brief Checks if a specific key is currently pressed down.
     * 
     * @param key The key to check.
     * 
     * @return True if the key is currently pressed down, false otherwise.
     * 
     * @details
     * This function checks the internal state of the keyboard to determine if a specific key
     * is currently pressed down. It returns true if the key is pressed, and false if it is not.
     * The function uses the sf::Keyboard::Key enum to identify the key.
     */
    bool isKeyDown(sf::Keyboard::Key key) const;

    /**
     * @brief Checks if a specific key was recently released.
     * 
     * @param key The key to check.
     * 
     * @return True if the key was recently released, false otherwise.
     * 
     * @details
     * This function checks if a specific key was recently released. It returns true if the
     * key was released in the last update, and false otherwise. The function uses the
     * sf::Keyboard::Key enum to identify the key.
     */
    bool keyReleased(sf::Keyboard::Key key) const;

  private:
    std::array<bool, sf::Keyboard::KeyCount> m_keys;
    sf::Keyboard::Key m_recentlyReleased;
};