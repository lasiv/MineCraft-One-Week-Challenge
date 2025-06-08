#include "inventory.h"

// implementations for Inventory class

Inventory::Inventory()
{
    for (unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_slots[i] = InvSlot();
    }
}

/* Inventory::~Inventory()
{

} */

void Inventory::draw(RenderMaster &renderer)
{

}

void Inventory::update()
{

}

void mouseInput()
{
    
}

void Inventory::toggleVisibility()
{
    m_showInventory = !m_showInventory;
}