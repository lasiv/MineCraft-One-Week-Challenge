#include "Inventory.h"

#include "../Item/Material.h"

Inventory::Inventory()
{
    m_slots = std::vector<ItemStack>(MAX_INV_SLOTS, ItemStack(Material::NOTHING, 0));

    // Drawing

    m_rectangle.setSize(sf::Vector2f(450.f, 50.f));
    m_rectangle.setFillColor(sf::Color::Transparent);
    m_rectangle.setOutlineThickness(6.f);
    m_rectangle.setOutlineColor(sf::Color(69,69,69,255));

    m_inv_step = m_rectangle.getSize().x/9;

    m_guiSlots = std::vector<sf::RectangleShape>(MAX_INV_SLOTS);

    for(unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_guiSlots[i].setSize(sf::Vector2(m_inv_step-2, m_rectangle.getSize().y-2));
        m_guiSlots[i].setFillColor(sf::Color(69,69,69,150));
    }
}

void Inventory::draw(sf::RenderWindow &window, int activeSlot)
{
    m_rectangle.setPosition((window.getSize().x/2)-200.f, window.getSize().y - 60.f); //position must be set here to use window

    window.draw(m_rectangle);

    for(unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_guiSlots[i].setPosition(m_rectangle.getPosition().x+1 + m_inv_step*i, m_rectangle.getPosition().y+1);

        if (activeSlot == i)
        {
            m_guiSlots[i].setOutlineColor(sf::Color::White);
            m_guiSlots[i].setOutlineThickness(5.f);
        }
        else
        {
            m_guiSlots[i].setOutlineColor(sf::Color(69,69,69,255));
            m_guiSlots[i].setOutlineThickness(2.f);
        }

        window.draw(m_guiSlots[i]);
    }
    
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