#ifndef ITEMSTACK_H_INCLUDED
#define ITEMSTACK_H_INCLUDED

#define MAX_STACK_SIZE 64

#include "Material.h"

/**
 * @class ItemStack
 * @brief Represents a stack of items in the player's inventory.
 * 
 * @details
 * The ItemStack class manages the quantity of a specific item type.
 * It allows adding and removing items from the stack, as well as checking
 * the current quantity. This is useful for inventory management in the game.
 */
class ItemStack {
  public:
    ItemStack(const Material &material, int amount);

    /**
     * @brief Adds a specified amount of items to the stack.
     * 
     * @param amount The number of items to add.
     * 
     * @return int The number of items left over if the stack is full.
     * 
     * @details
     * This method adds the specified amount of items to the stack.
     * If the stack exceeds its maximum capacity, the excess amount is returned.
     * The maximum stack size is defined by the Material class.
     */
    int add(int amount);

    /**
     * @brief Removes one item from the stack.
     * 
     * @details
     * This method decreases the number of items in the stack by one.
     * If the stack becomes empty, it resets the material to "nothing".
     */
    void remove();

    /**
     * @brief Gets the current number of items in the stack.
     * 
     * @return int The current quantity of items in the stack.
     * 
     * @details
     * This method returns the number of items currently in the stack.
     * It can be used to check how many items the player has of a specific type.
     */
    int getNumInStack() const;

    /**
     * @brief Gets the material type of the items in the stack.
     * 
     * @return const Material& A reference to the material of the items in the stack.
     * 
     * @details
     * This method returns a reference to the Material object associated with this stack.
     * It can be used to identify the type of item in the stack.
     */
    const Material &getMaterial() const;

  private:
    const Material *m_pMaterial = &Material::NOTHING;
    int m_numInStack = 0;
};

#endif // ITEMSTACK_H_INCLUDED
