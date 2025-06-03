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
    , m_acceleration(glm::vec3(0.f))

{
    f.loadFromFile("Res/Fonts/rs.ttf");

    for (int i = 0; i < 5; i++) {
        m_items.emplace_back(Material::NOTHING, 0);
    }

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
}

void Player::addItem(const Material &material)
{
    Material::ID id = material.id;

    for (unsigned i = 0; i < m_items.size(); i++) {
        if (m_items[i].getMaterial().id == id) {
            /*int leftOver =*/m_items[i].add(1);

            return;
        }
        else if (m_items[i].getMaterial().id == Material::ID::Nothing) {
            m_items[i] = {material, 1};
            return;
        }
    }
}

ItemStack &Player::getHeldItems()
{
    return m_items[m_heldItem];
}

void Player::handleInput(const sf::Window &window, Keyboard &keyboard)
{
    keyboardInput(keyboard);
    mouseInput(window);

    if (m_itemDown.isKeyPressed()) {
        m_heldItem++;
        if (m_heldItem == (int)m_items.size()) {
            m_heldItem = 0;
        }
    }
    else if (m_itemUp.isKeyPressed()) {
        m_heldItem--;
        if (m_heldItem == -1) {
            m_heldItem = m_items.size() - 1;
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
}

/**
 * @todo fix accelleration feeling floaty
 *       multiply damping and m_acceleration by accelleration constant to keep max speed
 *       check accelleration based on different cases like falling and on ground etc
 */

void Player::update(float dt, World &world)
{
    static float slip = DEFAULT_SLIPPERINESS;

    // debug print accelleration and velocity
    // std::cout << "\033[2K\r" // Clear current line and return cursor to start
    // << std::fixed << std::setprecision(3)
    // << "acc: (" << m_acceleration.x << ", " << m_acceleration.y << ", " << m_acceleration.z << ")  "
    // << "vel: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")"
    // << std::flush;

    // touch the ground while flying stops the flying
    if (m_isOnGround && m_isFlying) m_isFlying = false;

    // jumping

    // disable jumping when touching beginning to fly
    if (m_isJumping && m_isFlying) m_isJumping = false;
    // disable jumping when touching the ground
    if (m_isJumping && m_isOnGround && m_input.y != 1) m_isJumping = false;
    // enable jumping when on the ground, not flying and y input is up
    if (m_isOnGround && !m_isFlying && m_input.y == 1) {
        m_isJumping = true;
        m_isOnGround = false;
        velocity.y = JUMP_INIT;
    }
    // normally calculate gravity
    else {
        velocity.y = (velocity.y - GRAVITY_ACCEL) * FALLING_DRAG;
    }
    
    // moving

    float last_slip = slip;
    slip = DEFAULT_SLIPPERINESS; // put current block slipperiness here
    float accel = m_isJumping ? GROUND_ACCEL_BASE : AIR_ACCEL_BASE;
    float mov = MOVE_MULT_WALK;
    if (m_isSprinting) mov = MOVE_MULT_SPRINT;
    else if (m_isSneaking) mov = MOVE_MULT_SNEAK;
    float mov_mult = DIR_MULT_DEFAULT;
    if (m_input.x && m_input.z) {
        if (m_isSneaking) mov_mult = DIR_MULT_SNEAK_45;
        else mov_mult = DIR_MULT_STRAFE_45;
    }
    float boost = (m_isSprinting && m_isJumping) ? JUMP_SPRINT_BOOST : 0.f;

    float Ft_sin = glm::sin(glm::radians(rotation.y));
    float Ft_cos = glm::cos(glm::radians(rotation.y));
    float Dt_sin  = Ft_sin * m_input.x + Ft_cos * m_input.z;
    float Dt_cos  = Ft_sin * m_input.z - Ft_cos * m_input.x;

    velocity.x = velocity.x * last_slip * FRICTION_FACTOR + accel * mov * mov_mult /** cube(.6f/slip)*/ * Dt_sin /*+ boost * (-Ft_sin)*/;
    velocity.z = velocity.z * last_slip * FRICTION_FACTOR + accel * mov * mov_mult /** cube(.6f/slip)*/ * Dt_cos /*+ boost * (-Ft_cos)*/;





    // velocity += m_acceleration;
    // m_acceleration = {0, 0, 0};

    // if (!m_isFlying) {
        // if (!m_isOnGround) {
            // velocity.y -= 40 * dt;
        // }
        // m_isOnGround = false;
    // }

    if (position.y <= 0 && !m_isFlying) {
        position.y = RESPAWN_HEIGHT;
    }

    position.x += 20 * velocity.x * dt;
    collide(world, {velocity.x, 0, 0}, dt);

    position.y += 20 * velocity.y * dt;
    collide(world, {0, velocity.y, 0}, dt);

    position.z += 20 * velocity.z * dt;
    collide(world, {0, 0, velocity.z}, dt);

    box.update(position);
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

/// @todo add movement keys to config
void Player::keyboardInput(Keyboard &keyboard)
{   bool w = keyboard.isKeyDown(sf::Keyboard::W);
    bool a = keyboard.isKeyDown(sf::Keyboard::A);
    bool s = keyboard.isKeyDown(sf::Keyboard::S);
    bool d = keyboard.isKeyDown(sf::Keyboard::D);
    bool space = keyboard.isKeyDown(sf::Keyboard::Space);
    bool shift = keyboard.isKeyDown(sf::Keyboard::LShift);
    bool ctrl = keyboard.isKeyDown(sf::Keyboard::LControl);

    m_input.x = w - s;
    m_input.y = (m_isOnGround ? space : false) - (m_isFlying ? shift : false);
    m_input.z = d - a;

    m_isSneaking = shift;
    m_isSprinting = ctrl && !shift ;
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
    for (unsigned i = 0; i < m_items.size(); i++) {
        sf::Text &t = m_itemText[i];
        if (i == (unsigned)m_heldItem) {
            t.setFillColor(sf::Color::Red);
        }
        else {
            t.setFillColor(sf::Color::White);
        }
        t.setString((m_items[i].getMaterial().name) + " " +
                    std::to_string(m_items[i].getNumInStack()) + " ");
        // master.drawSFML(t);
    }
    std::ostringstream stream;
    stream << " X: " << position.x << " Y: " << position.y
           << " Z: " << position.z << " Grounded " << std::boolalpha
           << m_isOnGround;

    m_posPrint.setString(stream.str());
}

// void Player::jump()
// {
//     if (!m_isFlying) {
//         if (m_isOnGround) {

//             m_isOnGround = false;
//             m_acceleration.y += speed * 50;
//         }
//     }
//     else {
//         m_acceleration.y += speed * 3;
//     }
// }
