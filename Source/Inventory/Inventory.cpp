#include "Inventory.h"

#include "../Item/Material.h"

Inventory::Inventory()
{
    m_slots = std::vector<ItemStack>(MAX_INV_SLOTS, ItemStack(Material::NOTHING, 0));
}

void Inventory::initDraw(sf::RenderWindow &window)
{
    m_rectangle.setPosition((window.getSize().x/2)-200.f, window.getSize().y - 60.f);
    m_rectangle.setSize(sf::Vector2f(450.f, 50.f));
    m_rectangle.setFillColor(sf::Color::Transparent);
    m_rectangle.setOutlineThickness(6.f);
    m_rectangle.setOutlineColor(sf::Color(69,69,69,255));

    window.draw(m_rectangle);

    m_GuiSlotStep = m_rectangle.getSize().x/9;

    m_guiSlots = std::vector<sf::RectangleShape>(MAX_INV_SLOTS);

    for(unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        m_guiSlots[i].setPosition(m_rectangle.getPosition().x+1 + m_GuiSlotStep*i, m_rectangle.getPosition().y+1);
        m_guiSlots[i].setSize(sf::Vector2(m_GuiSlotStep-2, m_rectangle.getSize().y-2));
        m_guiSlots[i].setFillColor(sf::Color(69,69,69,150));

        window.draw(m_guiSlots[i]);
    }

    m_guiInitialized = true;
}

/* void Inventory::update()
{

} */

/* void Inventory::mouseInput()
{

} */

void Inventory::addItem(const Material* material)
{
    Material::ID id = material->id;
    int leftOver = 0;

    for (unsigned i = 0; i < MAX_INV_SLOTS; i++) 
    {
        if (m_slots[i].getMaterial().id == id && m_slots[i].getNumInStack() != m_slots[i].getMaterial().maxStackSize) 
        {
            leftOver = m_slots[i].add(1);
            int firstFree = getFirstFreeSlotNum();
    
            if (leftOver > 0 && firstFree != -1)
            {
                m_slots[firstFree].setMaterial(material);
                leftOver = 0;
            }

            return;
        }
        else if (m_slots[i].getMaterial().id == Material::ID::Nothing) 
        {
            m_slots[i].setMaterial(material);
            return;
        }
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

std::vector<sf::RectangleShape>& Inventory::getGuiSlots()
{
    return m_guiSlots;
}

sf::RectangleShape& Inventory::getRect()
{
    return m_rectangle;
}

void Inventory::toggleVisibility()
{
    m_showInventory = !m_showInventory;
}

bool Inventory::getVisibility()
{
    return m_showInventory;
}

bool Inventory::isGuiInit()
{
    return m_guiInitialized;
}

float Inventory::getGuiSlotStep()
{
    return m_GuiSlotStep;
}