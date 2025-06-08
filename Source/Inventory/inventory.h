#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#define MAX_INV_SLOTS 36

#include <SFML/Graphics.hpp>
#include <Item/ItemStack.h>
#include <vector>

class RenderMaster;

class Inventory
{
    public:

        Inventory();

        ~Inventory();

        void draw(RenderMaster &renderer);

    private:

        std::vector<InvSlot> m_slots;
        u_int8_t m_maxslots = MAX_INV_SLOTS;

        bool m_showInventory = false;
};

class InvSlot
{
    public:

    private:

        ItemStack m_items;
};

#endif // INVENTORY_H_INCLUDED