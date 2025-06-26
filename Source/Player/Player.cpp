#include "Player.h"

#include <SFML/Graphics.hpp>

#include <iomanip>
#include <iostream>
#include <sstream>

#include "../Input/Keyboard.h"
#include "../Renderer/RenderMaster.h"
#include "../World/World.h"

sf::Font f;

Player::Player()
    : Entity({2500, 125, 2500}, {0.f, 0.f, 0.f}, {0.3f, 1.f, 0.3f})
    , m_itemDown(sf::Keyboard::Down)
    , m_itemUp(sf::Keyboard::Up)
    , m_flyKey(sf::Keyboard::F)
    , m_num1(sf::Keyboard::Num1)
    , m_num2(sf::Keyboard::Num2)
    , m_num3(sf::Keyboard::Num3)
    , m_num4(sf::Keyboard::Num4)
    , m_num5(sf::Keyboard::Num5)
    , m_num6(sf::Keyboard::Num6)
    , m_num7(sf::Keyboard::Num7)
    , m_num8(sf::Keyboard::Num8)
    , m_num9(sf::Keyboard::Num9)
    , m_inv(sf::Keyboard::I)
    , m_slow(sf::Keyboard::LShift)
    , m_acceleration(glm::vec3(0.f))

{
    f.loadFromFile("Res/Fonts/rs.ttf");

    /* for (int i = 0; i < 5; i++) {
        m_items.emplace_back(Material::NOTHING, 0);
    }  */// should just create an inventory object

    for (float i = 0; i < 5; i++) {
        sf::Text t;
        t.setFont(f);
        t.setOutlineColor(sf::Color::Black);
        t.setCharacterSize(25);
        t.setPosition({20.0f, 20.0f * i + 100.0f});
        m_itemText.push_back(t);
    }
    m_posPrint.setFont(f);
    m_posPrint.setOutlineColor(sf::Color::Black);
    m_posPrint.setCharacterSize(25);
    m_posPrint.setPosition(20.0f, 20.0f * 6.0f + 100.0f);

    m_grass_icon = new sf::Texture();
    m_oak_bark_icon = new sf::Texture();
    m_oak_leaf_icon = new sf::Texture();
    m_stone_icon = new sf::Texture();
    m_dirt_icon = new sf::Texture();
    m_sand_icon = new sf::Texture();
    m_cactus_icon = new sf::Texture();
    m_rose_icon = new sf::Texture();
    m_tall_grass_icon = new sf::Texture();
    m_dead_shrub_icon = new sf::Texture();

    m_grass_icon->loadFromFile("Res/Icons/grass_icon.png");
    m_oak_bark_icon->loadFromFile("Res/Icons/oak_bark_icon.png");
    m_oak_leaf_icon->loadFromFile("Res/Icons/oak_leaf_icon.png");
    m_stone_icon->loadFromFile("Res/Icons/stone_icon.png");
    m_dirt_icon->loadFromFile("Res/Icons/dirt_icon.png");
    m_sand_icon->loadFromFile("Res/Icons/sand_icon.png");
    m_cactus_icon->loadFromFile("Res/Icons/cactus_icon.png");
    m_rose_icon->loadFromFile("Res/Icons/rose_icon.png");
    m_tall_grass_icon->loadFromFile("Res/Icons/tall_grass.png");
    m_dead_shrub_icon->loadFromFile("Res/Icons/dead_shrub.png");
}

Player::~Player()
{
    delete m_grass_icon;
    delete m_oak_bark_icon;
    delete m_oak_leaf_icon;
    delete m_stone_icon;
    delete m_dirt_icon;
    delete m_sand_icon;
    delete m_cactus_icon;
    delete m_rose_icon;
    delete m_tall_grass_icon;
    delete m_dead_shrub_icon;
}

void Player::addItem(const Material &material)
{
    m_inventory.addItem(&material);
}

ItemStack& Player::getHeldItems()
{
    if (m_heldItem >= 0 && m_heldItem < MAX_HOTBAR_SLOTS)
    {
        return m_inventory.getItemOfSlot(m_heldItem);
    }
}

Inventory& Player::getInventory()
{
    return m_inventory;
}

void Player::handleInput(const sf::Window &window, Keyboard &keyboard)
{
    keyboardInput(keyboard);
    mouseInput(window);

    if (m_itemDown.isKeyPressed()) {
        m_heldItem++;
        if (m_heldItem == (int)MAX_HOTBAR_SLOTS) {
            m_heldItem = 0;
        }
    }
    else if (m_itemUp.isKeyPressed()) {
        m_heldItem--;
        if (m_heldItem == -1) {
            m_heldItem = MAX_HOTBAR_SLOTS - 1;
        }
    }

    if (m_flyKey.isKeyPressed()) {
        m_isFlying = !m_isFlying;
    }

    if (m_num1.isKeyPressed()) {
        m_heldItem = 0;
    }
    if (m_num2.isKeyPressed()) {
        m_heldItem = 1;
    }
    if (m_num3.isKeyPressed()) {
        m_heldItem = 2;
    }
    if (m_num4.isKeyPressed()) {
        m_heldItem = 3;
    }
    if (m_num5.isKeyPressed()) {
        m_heldItem = 4;
    }
    if (m_num6.isKeyPressed()) {
        m_heldItem = 5;
    }
    if (m_num7.isKeyPressed()) {
        m_heldItem = 6;
    }
    if (m_num8.isKeyPressed()) {
        m_heldItem = 7;
    }
    if (m_num9.isKeyPressed()) {
        m_heldItem = 8;
    }

    if (m_inv.isKeyPressed()) {
        m_inventory.toggleVisibility();
    }

    if (m_slow.isKeyPressed())
    {
        m_isSneak = !m_isSneak;
    }
}

/**
 * @todo fix accelleration feeling floaty
 *       multiply damping and m_acceleration by accelleration constant to keep max speed
 *       check accelleration based on different cases like falling and on ground etc
 */

void Player::update(float dt, World &world)
{
    velocity += m_acceleration;
    // debug print accelleration and velocity
    std::cout << "\033[2K\r" // Clear current line and return cursor to start
    << std::fixed << std::setprecision(3)
    << "acc: (" << m_acceleration.x << ", " << m_acceleration.y << ", " << m_acceleration.z << ")  "
    << "vel: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")"
    << std::flush;

    m_acceleration = {0, 0, 0};

    if (!m_isFlying) {
        if (!m_isOnGround) {
            velocity.y -= 40 * dt;
        }
        m_isOnGround = false;
    }

    if (position.y <= 0 && !m_isFlying) {
        position.y = 300;
    }

    position.x += velocity.x * dt;
    collide(world, {velocity.x, 0, 0}, dt);

    position.y += velocity.y * dt;
    collide(world, {0, velocity.y, 0}, dt);

    position.z += velocity.z * dt;
    collide(world, {0, 0, velocity.z}, dt);

    box.update(position);
    velocity.x *= 0.95f;
    velocity.z *= 0.95f;
    if (m_isFlying) {
        velocity.y *= 0.95f;
    }
}

void Player::collide(World &world, const glm::vec3 &vel, float dt)
{
    for (int x = position.x - box.dimensions.x;
         x < position.x + box.dimensions.x; x++)
        for (int y = position.y - box.dimensions.y; y < position.y + 0.7; y++)
            for (int z = position.z - box.dimensions.z;
                 z < position.z + box.dimensions.z; z++) {
                auto block = world.getBlock(x, y, z);

                if (block != 0 && block.getData().isCollidable) {
                    if (vel.y > 0) {
                        position.y = y - box.dimensions.y;
                        velocity.y = 0;
                    }
                    else if (vel.y < 0) {
                        m_isOnGround = true;
                        position.y = y + box.dimensions.y + 1;
                        velocity.y = 0;
                    }

                    if (vel.x > 0) {
                        position.x = x - box.dimensions.x;
                    }
                    else if (vel.x < 0) {
                        position.x = x + box.dimensions.x + 1;
                    }

                    if (vel.z > 0) {
                        position.z = z - box.dimensions.z;
                    }
                    else if (vel.z < 0) {
                        position.z = z + box.dimensions.z + 1;
                    }
                }
            }
}

/// @todo Move this (comment by hopson)
float speed = 0.2f;
float accelleration = 1.f;

/// @todo add movement keys to config
void Player::keyboardInput(Keyboard &keyboard)
{
    if (keyboard.isKeyDown(sf::Keyboard::W)) {
        float s = speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            s *= 5;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::RShift) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            s *= 0.35;
        m_acceleration.x += -glm::cos(glm::radians(rotation.y + 90)) * s * accelleration;
        m_acceleration.z += -glm::sin(glm::radians(rotation.y + 90)) * s * accelleration;
    }
    if (keyboard.isKeyDown(sf::Keyboard::S)) {
        m_acceleration.x += glm::cos(glm::radians(rotation.y + 90)) * speed * accelleration;
        m_acceleration.z += glm::sin(glm::radians(rotation.y + 90)) * speed * accelleration;
    }
    if (keyboard.isKeyDown(sf::Keyboard::A)) {
        m_acceleration.x += -glm::cos(glm::radians(rotation.y)) * speed * accelleration;
        m_acceleration.z += -glm::sin(glm::radians(rotation.y)) * speed * accelleration;
    }
    if (keyboard.isKeyDown(sf::Keyboard::D)) {
        m_acceleration.x += glm::cos(glm::radians(rotation.y)) * speed * accelleration;
        m_acceleration.z += glm::sin(glm::radians(rotation.y)) * speed * accelleration;
    }

    if (keyboard.isKeyDown(sf::Keyboard::Space)) {
        jump();
    }
    else if (keyboard.isKeyDown(sf::Keyboard::LShift) && m_isFlying) {
        m_acceleration.y -= speed * 3;
    }
}

/// @todo add sensitivity and mouselock key to config
void Player::mouseInput(const sf::Window &window)
{
    static bool useMouse = true;
    static ToggleKey useMouseKey(sf::Keyboard::L);

    if (useMouseKey.isKeyPressed()) {
        useMouse = !useMouse;
    }

    if (!useMouse) {
        return;
    }

    static float const BOUND = 89.f;
    static auto lastMousePosition = sf::Mouse::getPosition(window);
    auto change = sf::Mouse::getPosition() - lastMousePosition;
    
    rotation.y += change.x * 0.05f;
    rotation.x += change.y * 0.05f;

    if (rotation.x > BOUND)
        rotation.x = BOUND;
    else if (rotation.x < -BOUND)
        rotation.x = -BOUND;

    if (rotation.y > 360)
        rotation.y = 0;
    else if (rotation.y < 0)
        rotation.y = 360;

    auto cx = static_cast<int>(window.getSize().x / 2);
    auto cy = static_cast<int>(window.getSize().y / 2);

    sf::Mouse::setPosition({cx, cy}, window);

    lastMousePosition = sf::Mouse::getPosition();
}

void Player::draw(RenderMaster &master)
{
    for (unsigned i = 0; i < m_inventory.getSlots().size(); i++) 
    {
        sf::Text &t = m_itemText[i];
        if (i == (unsigned)m_heldItem) 
        {
            t.setFillColor(sf::Color::Red);
        }
        else 
        {
            t.setFillColor(sf::Color::White);
        }
        t.setString((m_inventory.getItemOfSlot(i).getMaterial().name) + " " +
                    std::to_string(m_inventory.getItemOfSlot(i).getNumInStack()) + " ");
        // master.drawSFML(t);
    }
    std::ostringstream stream;
    stream << " X: " << position.x << " Y: " << position.y
           << " Z: " << position.z << " Grounded " << std::boolalpha
           << m_isOnGround;

    m_posPrint.setString(stream.str());
}

void Player::jump()
{
    if (!m_isFlying) {
        if (m_isOnGround) {

            m_isOnGround = false;
            m_acceleration.y += speed * 50;
        }
    }
    else {
        m_acceleration.y += speed * 3;
    }
}

int Player::getHeldItem()
{
    return m_heldItem;
}

void Player::drawInventory(sf::RenderWindow &window)
{
    // initialize drawing
    if(m_inventory.isGuiInit() == false)
    {
        m_inventory.initDraw(window);
    }

    // draw base rectangle
    window.draw(m_inventory.getRect());

    unsigned int highlightSlotIndex;

    sf::RectangleShape icon;
    sf::Text amount;

    // drawing slots
    for(unsigned int i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (m_heldItem == i)
        {
            highlightSlotIndex = i;
            m_inventory.getGuiSlots()[highlightSlotIndex].setOutlineColor(sf::Color::White);
            m_inventory.getGuiSlots()[highlightSlotIndex].setOutlineThickness(5.f);
            continue;
        }
        else
        {
            m_inventory.getGuiSlots()[i].setOutlineColor(sf::Color(69,69,69,255));
            m_inventory.getGuiSlots()[i].setOutlineThickness(2.f);
        }
    
        window.draw(m_inventory.getGuiSlots()[i]);
    }

    window.draw(m_inventory.getGuiSlots()[highlightSlotIndex]);

    // drawing icons
    for (int i = 0; i < MAX_INV_SLOTS; i++)
    {
        if (m_inventory.getSlots()[i].getMaterial().id != Material::Nothing)
        {
            icon.setTexture(getIconFromId(m_inventory.getSlots()[i].getMaterial().id));
            icon.setPosition(m_inventory.getGuiSlots()[i].getPosition());
            icon.setSize(m_inventory.getGuiSlots()[i].getSize());
            icon.setFillColor(sf::Color(255,255,255,255));

            amount.setPosition(m_inventory.getGuiSlots()[i].getPosition().x +5, m_inventory.getGuiSlots()[i].getPosition().y+15);
            amount.setScale(m_inventory.getGuiSlots()[i].getScale());
            amount.setFont(f);
            amount.setString(std::to_string(m_inventory.getSlots()[i].getNumInStack()));

            window.draw(icon);
            window.draw(amount);
        }
    }
}

const sf::Texture* Player::getIconFromId(const Material::ID& id)
{
    switch(id)
    {
        case Material::Grass:
            return m_grass_icon;
        case Material::Sand:
            return m_sand_icon;
        case Material::Stone:
            return m_stone_icon;
        case Material::OakBark:
            return m_oak_bark_icon;
        case Material::OakLeaf:
            return m_oak_leaf_icon;
        case Material::Cactus:
            return m_cactus_icon;
        case Material::Dirt:
            return m_dirt_icon;
        case Material::Rose:
            return m_rose_icon;
        case Material::TallGrass:
            return m_tall_grass_icon;
        case Material::DeadShrub:
            return m_dead_shrub_icon;
    }
}