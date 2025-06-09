#include "Inventory.h"

#include "../Item/Material.h"

Inventory::Inventory()
{
    for (unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_slots[i] = ItemStack(Material::NOTHING, 1);
    }
}

void Inventory::draw(RenderMaster &renderer)
{

}

void Inventory::update()
{

}

void Inventory::mouseInput()
{

}

void Inventory::addItem(const Material &material)
{
    Material::ID id = material.id;
    int leftOver;

    for (unsigned i = 0; i < MAX_INV_SLOTS; i++) 
    {
        if (m_slots[i].getMaterial().id == id) 
        {
            leftOver = m_slots[i].add(1);
        }
        if (leftOver)
        {
            getFirstFreeSlot() = {material, 1};
        }
        return;
    }
}

ItemStack &Inventory::getFirstFreeSlot()
{
    for (unsigned i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (m_slots[i].getMaterial().id == Material::ID::Nothing)
        {
            return m_slots[i];
        }
    }
}

ItemStack &Inventory::getItemOfSlot(int slotNum)
{
    return m_slots[slotNum];
}

void Inventory::toggleVisibility()
{
    m_showInventory = !m_showInventory;
}