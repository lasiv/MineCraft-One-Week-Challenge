#include "Inventory.h"

#include "../Item/Material.h"

Inventory::Inventory()
{
    m_slots = std::vector<ItemStack>(MAX_INV_SLOTS, ItemStack(Material::NOTHING, 0));
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
    int leftOver = 0;

    for (unsigned i = 0; i < getSlots().size(); i++) 
    {
        if (m_slots[i].getMaterial().id == id) 
        {
            leftOver = m_slots[i].add(1);
        }
        if (leftOver && getFirstFreeSlotNum() != -1)
        {
            m_slots[getFirstFreeSlotNum()] = {material, 1};
        }
        return;
    }
}

int Inventory::getFirstFreeSlotNum()
{
    for (unsigned i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (m_slots[i].getMaterial().id == Material::ID::Nothing)
        {
            return i;
        }
    }
    return -1;
}

std::vector<ItemStack>& Inventory::getSlots()
{
    return m_slots;
}

ItemStack& Inventory::getItemOfSlot(int slotNum)
{
    if (slotNum >= 0 && slotNum < MAX_INV_SLOTS)
    {
        return m_slots[slotNum];
    }
}

void Inventory::toggleVisibility()
{
    m_showInventory = !m_showInventory;
}