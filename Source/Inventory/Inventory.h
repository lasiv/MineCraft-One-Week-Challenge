#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#define MAX_INV_SLOTS 9
#define MAX_HOTBAR_SLOTS 9

#include <SFML/Graphics.hpp>
#include "../Item/ItemStack.h"
#include <vector>

//class RenderMaster;

/**
 * @class Inventory
 * @brief Inventory for the Player
 * 
 * @details The Inventory class handles the input from the player and processes it, when the players inventory is open.
 * 
 * @note This class manages the inventory of the player (ItemStacks, merging, splitting).
 */
class Inventory
{
    public:
        Inventory();

        /**
         * @brief Draws the Inventory with given rendermaster
         * 
         * @param window the window where to draw the inventory
         * @param activeSlot the active slot of player
         * 
         * @details Draws the Inventory using the window in a specific area and an even layout
         */
        void initDraw(sf::RenderWindow &window);

        /**
         * @brief Updates the inventory, to add items
         * 
         * @details The update method is used by the Player Class to update the Inventory, whenever the player updates or interacts with the world
         */
        /* void update(); */

        /**
         * @brief 
         */
        /* void mouseInput(); */

        /**
         * @brief increases the number of an ItemStack
         * 
         * @param material a material of the struct Material
         * 
         * @details The addItem method is used to increment the numInStack of the Slot of given material. It is also used for changing the material to the given one, when no Stack with given material is found.
         */
        void addItem(const Material* material);

        /**
         * @brief toggles the visibility of the inv 
         */
        void toggleVisibility();

        /**
         * @brief get the current visibility
         * 
         * @return current visibility
         */
        bool getVisibility();

        /**
         * @brief returns the ItemStack of the specified slot in the inventory
         * 
         * @param slotNum the slot number to return the ItemStack of
         * 
         * @return the ItemStack of the inventory at the given number 
         */  
        ItemStack& getItemOfSlot(int slotNum);

        /**
         * @brief finds the first slot which has the id of nothing
         * 
         * @return number where the first ItemStack with the id of nothing
         */
        int getFirstFreeSlotNum();

        /**
         * @brief returns the member m_slots
         * 
         * @return reference to a cpp-vector with type ItemStack
         */
        std::vector<ItemStack>& getSlots();

        std::vector<sf::RectangleShape>& getGuiSlots();

        sf::RectangleShape& getRect();

        bool isGuiInit();

        float getGuiSlotStep();

    private:

        std::vector<ItemStack> m_slots;

        bool m_showInventory = true;
        bool m_guiInitialized = false;

        sf::RectangleShape m_rectangle;
        float m_GuiSlotStep;
        std::vector<sf::RectangleShape> m_guiSlots; 
};

#endif // INVENTORY_H_INCLUDED