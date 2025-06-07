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

void Player::update(float dt, World &world) {
    // --- Jump/Fly state transitions ---
    bool justJumped = false;

    // Flying cancels when touching ground
    if (m_isOnGround && m_isFlying) {
        m_isFlying = false;
    }

    // Cancel jump if we start flying or release jump input mid-air
    if (m_isJumping && (m_isFlying || (m_isOnGround && m_input.y != 1))) {
        m_isJumping = false;
    }

    // Initiate jump
    if (m_isOnGround && !m_isFlying && m_input.y == 1) {
        m_isJumping  = true;
        m_isOnGround = false;
        justJumped   = true;
        velocity.y   = JUMP_INIT;
    }
    // Apply gravity when not flying
    if (!m_isFlying) {
        // per-tick constants
        constexpr float a_v = FALLING_DRAG;            // 0.98
        constexpr float b_v = -GRAVITY_ACCEL * FALLING_DRAG; // -0.08*0.98
        // effective coefficients for dt
        float d     = dt / TICK;
        float a_eff = std::pow(a_v, d);
        float b_eff = (b_v / (1 - a_v)) * (1 - a_eff);
        velocity.y  = a_eff * velocity.y + b_eff;
    }

    // --- Horizontal parameters ---
    bool onGround = m_isOnGround;
    // friction multiplier a_h and accel per tick b_h depend on ground vs air
    float slip = onGround ? DEFAULT_SLIPPERINESS : AIR_SLIPPERINESS;
    float a_h  = (onGround ? slip * FRICTION_FACTOR : FRICTION_FACTOR);
    // total per-tick acceleration (b_h) along input
    float accelBase = onGround ? GROUND_ACCEL_BASE : AIR_ACCEL_BASE;
    float mov       = m_isSprinting ? MOVE_MULT_SPRINT : m_isSneaking ? MOVE_MULT_SNEAK : MOVE_MULT_WALK;
    float movMult   = (m_input.x && m_input.z)
                        ? (m_isSneaking ? DIR_MULT_SNEAK_45 : DIR_MULT_STRAFE_45)
                        : DIR_MULT_DEFAULT;
    float slipCube  = onGround ? cube(0.6f / slip) : 1.0f;
    // facing direction
    float yawRad = glm::radians(rotation.y);
    float sinYaw = glm::sin(yawRad), cosYaw = glm::cos(yawRad);
    float dtSin  = sinYaw * m_input.x + cosYaw * m_input.z;
    float dtCos  = sinYaw * m_input.z - cosYaw * m_input.x;
    // per-tick horizontal acceleration vector
    glm::vec2 b_tick( accelBase * mov * movMult * slipCube * dtSin,
                      accelBase * mov * movMult * slipCube * dtCos );
    // one-time jump boost (only if just pressed on this frame)
    static bool wasJump = false;
    bool justJumped = (!wasJump && m_input.y == 1 && onGround);
    wasJump = m_input.y == 1 && onGround;
    glm::vec2 boost(0.0f);
    if (justJumped && m_isSprinting) {
        boost = glm::vec2( JUMP_SPRINT_BOOST * sinYaw,
                          JUMP_SPRINT_BOOST * -cosYaw );
        velocity.y = JUMP_INIT; m_isOnGround = false; m_isJumping = true;
    }
    
    // effective horizontal coefficients for dt
    float d_h    = dt / TICK;
    float a_h_eff= std::pow(a_h, d_h);
    float b_h_eff= (1 - a_h_eff) / (1 - a_h);

    // update horizontal velocity
    glm::vec2 vel_xz{velocity.x, velocity.z};
    vel_xz = a_h_eff * vel_xz + b_tick * b_h_eff + boost;
    velocity.x = vel_xz.x;
    velocity.z = vel_xz.y;

    // --- Position update & collision ---
    if (position.y <= 0.0f && !m_isFlying) position.y = RESPAWN_HEIGHT;
    position.x += velocity.x * dt;
    collide(world, {velocity.x * dt, 0, 0}, dt);
    position.y += velocity.y * dt;
    collide(world, {0, velocity.y * dt, 0}, dt);
    position.z += velocity.z * dt;
    collide(world, {0, 0, velocity.z * dt}, dt);
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
