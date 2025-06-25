#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#define MAX_INV_SLOTS 36
#define MAX_HOTBAR_SLOTS 9

#include <SFML/Graphics.hpp>
#include "../Item/ItemStack.h"
#include <vector>

class RenderMaster;

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
         * @param renderer the rendermaster which processes rendering
         * 
         * @details Draws the Inventory using the rendermaster in a specific area and an even layout
         */
        void draw(RenderMaster &renderer);

        /**
         * @brief Updates the inventory, to add items
         * 
         * @details The update method is used by the Player Class to update the Inventory, whenever the player updates or interacts with the world
         */
        void update();

        /**
         * @brief 
         */
        void mouseInput();

        /**
         * @brief increases the number of an ItemStack
         * 
         * @param material a material of the struct Material
         * 
         * @details The addItem method is used to increment the numInStack of the Slot of given material. It is also used for changing the material to the given one, when no Stack with given material is found.
         */
        void addItem(const Material* material);

        void toggleVisibility();

        ItemStack& getItemOfSlot(int slotNum);

        int getFirstFreeSlotNum();

        std::vector<ItemStack>& getSlots();

    private:

        std::vector<ItemStack> m_slots;

        bool m_showInventory = false;
};

#endif // INVENTORY_H_INCLUDED