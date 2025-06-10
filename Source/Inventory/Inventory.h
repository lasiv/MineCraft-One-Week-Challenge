#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#define MAX_INV_SLOTS 36

#include <SFML/Graphics.hpp>
#include "../Item/ItemStack.h"
#include <vector>

class RenderMaster;

class Inventory
{
    public:

        Inventory();

        /* ~Inventory(); */

        void draw(RenderMaster &renderer);

        void update();

        void mouseInput();

        void addItem(const Material &material_id);

        void toggleVisibility();

        ItemStack &getItemOfSlot(int slotNum);

        ItemStack &getFirstFreeSlot();

    private:

        std::vector<ItemStack> m_slots;

        bool m_showInventory = false;
};

#endif // INVENTORY_H_INCLUDED