#include "Inventory.h"

#include "../Item/Material.h"

Inventory::Inventory()
{
    m_slots = std::vector<ItemStack>(MAX_INV_SLOTS, ItemStack(Material::NOTHING, 0));
}

void Inventory::draw(sf::RenderWindow &window)
{

}

void Inventory::update()
{

}

/* void Inventory::mouseInput()
{

} */

void Inventory::addItem(const Material* material)
{
    Material::ID id = material->id;
    //int leftOver = 0;

    for (unsigned i = 0; i < MAX_INV_SLOTS; i++) 
    {
        if (m_slots[i].getMaterial().id == id) 
        {
            /*int leftOver =*/m_slots[i].add(1);

            return;
        }
        else if (m_slots[i].getMaterial().id == Material::ID::Nothing) 
        {
            m_slots[i].setMaterial(material);
            return;
        }
    }

    /* int firstFree = getFirstFreeSlotNum();
    
    if (leftOver && firstFree != -1)
    {
        m_slots[firstFree].setMaterial(material);
    } */
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

bool Inventory::getVisibility()
{
    return m_showInventory;
}