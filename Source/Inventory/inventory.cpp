#include "inventory.h"

#include "../Item/Material.h"

// implementations for Inventory class

Inventory::Inventory()
{
    for (unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_slots[i] = ItemStack(Material::NOTHING, 1);
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

void Inventory::mouseInput()
{

}

void Inventory::addItem()
{

}

void Inventory::toggleVisibility()
{
    m_showInventory = !m_showInventory;
}