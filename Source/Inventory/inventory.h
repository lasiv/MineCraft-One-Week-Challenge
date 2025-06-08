#ifndef INVENTORY_H_INCLUDED
#define INVENTORY_H_INCLUDED

#define MAX_INV_SLOTS 36
#define MAX_STACK_SIZE 64

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

        void changeVisibility();

    private:

        std::vector<InvSlot> m_slots;
        u_int8_t m_maxslots = MAX_INV_SLOTS;

        bool m_showInventory = false;
};

class InvSlot
{
    public:

        InvSlot();

        ~InvSlot();

        void addItem(const Material &material);

    private:

        ItemStack m_items;

        u_int m_maxStack = MAX_STACK_SIZE;
};

#endif // INVENTORY_H_INCLUDED