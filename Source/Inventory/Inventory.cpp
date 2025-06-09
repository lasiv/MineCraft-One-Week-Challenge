#include "Inventory.h"

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

void Inventory::addItem(const Material &material)
{
    Material::ID id = material.id;

    for (unsigned i = 0; i < m_slots.size(); i++) 
    {
        if (m_slots[i].getMaterial().id == id) 
        {
            /*int leftOver =*/m_slots[i].add(1);

            return;
        }
        else if (m_slots[i].getMaterial().id == Material::ID::Nothing) 
        {
            m_slots[i] = {material, 1};
            return;
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