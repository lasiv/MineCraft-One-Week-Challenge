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

void Player::setPosition(glm::vec3 pos) {
    position = pos;
    m_nextPosition = pos;
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

void Player::update(float dt, World &world) {
    static float alpha = 0.0f;
    float last_alpha = alpha;
    alpha += dt / TICK;

    for (; alpha > 1.f; alpha--) {
        last_alpha = 0.f;
        calculate(world);
    }

    float k = (alpha - last_alpha)/(1.f - last_alpha);

    position += (m_nextPosition - position) * k;

    box.update(position);
}

void Player::calculate(World &world) {
    // --- Jump/Fly state transitions ---
    bool justJumped = false;

    // Flying cancels when touching ground
    if (m_isOnGround && m_isFlying) {
        m_isFlying = false;
    }

    if (m_isFlying) {
        velocity.y = velocity.y * DEFAULT_SLIPPERINESS * FRICTION_FACTOR + m_input.y * GROUND_ACCEL_BASE;
    }
    else {
        // Initiate jump
        if (m_isOnGround && m_input.y == 1) {
            m_isOnGround = false;
            justJumped   = true;
            velocity.y   = JUMP_INIT;
        }
        // Apply gravity when not flying
        else {
            velocity.y = (velocity.y - GRAVITY_ACCEL) * FALLING_DRAG;
        }
    }

    // --- Horizontal movement parameters ---
    const bool onGround = m_isOnGround;
    const float slip       = onGround ? DEFAULT_SLIPPERINESS : AIR_SLIPPERINESS;
    const float friction   = slip * FRICTION_FACTOR;
    const float accelBase  = onGround ? GROUND_ACCEL_BASE : AIR_ACCEL_BASE;

    // Movement multipliers
    const float mov       = m_isSprinting ? MOVE_MULT_SPRINT
                                : m_isSneaking ? MOVE_MULT_SNEAK
                                               : MOVE_MULT_WALK;
    const float movMult   = (m_input.x && m_input.z)
                                ? (m_isSneaking ? DIR_MULT_SNEAK_45
                                               : DIR_MULT_STRAFE_45)
                                : DIR_MULT_DEFAULT;
    const float slipCube  = onGround ? cube(0.6f / slip) : 1.0f;

    // Facing/input directions
    const float yawRad  = glm::radians(rotation.y);
    const float sinYaw  = glm::sin(yawRad);
    const float cosYaw  = glm::cos(yawRad);
    const float dtSin   = sinYaw * m_input.x + cosYaw * m_input.z;
    const float dtCos   = sinYaw * m_input.z - cosYaw * m_input.x;

    // One-time sprint-jump boost
    const float boostX  = (justJumped && m_isSprinting) ? JUMP_SPRINT_BOOST * sinYaw : 0.0f;
    const float boostZ  = (justJumped && m_isSprinting) ? JUMP_SPRINT_BOOST * -cosYaw : 0.0f;

    // Apply friction (momentum)
    float momentumX = velocity.x * friction;
    float momentumZ = velocity.z * friction;

    // Compute acceleration contribution
    const float accelFactor = accelBase * mov * movMult * slipCube;
    const float accX        = accelFactor * dtSin;
    const float accZ        = accelFactor * dtCos;

    // Final horizontal velocity
    velocity.x = momentumX + accX + boostX;
    velocity.z = momentumZ + accZ + boostZ;

    // --- Position update and collision ---
    if (m_nextPosition.y <= 0.0f && !m_isFlying) {
        m_nextPosition.y = RESPAWN_HEIGHT;
    }

    bool wasOnGround = m_isOnGround;
    m_isOnGround = false;

    auto testPosition = position;
    testPosition.x += velocity.x;
    testPosition.y -= 0.5f;
    bool stopx = isFalling(world, testPosition, {0.f, velocity.y, 0.f}) && m_isSneaking && wasOnGround != m_isOnGround;
    if (stopx) std::cout << "stopx" << std::flush;
    
    testPosition = position;
    testPosition.z += velocity.z;
    testPosition.y -= 0.5f;
    bool stopz = isFalling(world, testPosition, {0.f, velocity.y, 0.f}) && m_isSneaking && wasOnGround != m_isOnGround;
    if (stopz) std::cout << "stopz" << std::flush;
    
    if (!stopx) m_nextPosition.x += velocity.x;
    collide(world, { velocity.x, 0.0f, 0.0f });
    
    if (!stopz) m_nextPosition.z += velocity.z;
    collide(world, { 0.0f, 0.0f, velocity.z });
    
    m_nextPosition.y += velocity.y;
    collide(world, { 0.0f, velocity.y, 0.0f });

    printdebug();
}

void Player::printdebug() const
{
    // compute horizontal speed and clamp to 2 decimals
    float hSpeed = glm::length(glm::vec2(velocity.x, velocity.z)) / TICK;

    // "\033[2K" = ANSI “erase entire line”, then '\r' returns to start
    std::cout << "\033[2K\r"
                << std::fixed << std::setprecision(2)
                << "Pos(" 
                << position.x << ", "
                << position.y << ", "
                << position.z << ")  "
                << "Rot("
                << rotation.x << ", "
                << rotation.y << ", "
                << rotation.z << ")  "
                << "Vel("
                << velocity.x << ", "
                << velocity.y << ", "
                << velocity.z << ")  "
                << "HS=" << hSpeed << "  "
                << "In("
                << m_input.x << ", "
                << m_input.y << ", "
                << m_input.z << ")"
                << std::flush;
}

bool Player::isFalling(World &world, const glm::vec3 &testPosition, const glm::vec3 &vel) const{
    // 1) If we’re not moving downward, we’re not falling
    if (vel.y >= 0.0f)
        return false;

    // 2) Which block‐row would we hit first when falling?
    int faceY = int(std::floor(testPosition.y - box.dimensions.y));

    // 3) Compute X/Z span of the player’s feet at testPosition
    int minX = int(std::floor(testPosition.x - box.dimensions.x));
    int maxX = int(std::floor(testPosition.x + box.dimensions.x));
    int minZ = int(std::floor(testPosition.z - box.dimensions.z));
    int maxZ = int(std::floor(testPosition.z + box.dimensions.z));

    // 4) If any block directly under foot is solid, we’re supported → not falling
    for (int x = minX; x <= maxX; ++x) {
        for (int z = minZ; z <= maxZ; ++z) {
            auto block = world.getBlock(x, faceY, z);
            if (block != 0 && block.getData().isCollidable) {
                return false;  // found support
            }
        }
    }

    // 5) No support found → we are falling
    return true;
}

bool Player::nextBlockAir(World &world, const glm::vec3 &vel) {
    
    int y = (int) (position.y - box.dimensions.x);

    //
    return true;
}

void Player::collide(World &world, const glm::vec3 &vel)
{
    if (glm::abs(vel.x) == 0.0f && glm::abs(vel.z) == 0.0f && std::abs(vel.y) == 0.0f) return;
    for (int x = m_nextPosition.x - box.dimensions.x; x < m_nextPosition.x + box.dimensions.x; x++)
        for (int y = m_nextPosition.y - box.dimensions.y; y < m_nextPosition.y + 0.7; y++)
            for (int z = m_nextPosition.z - box.dimensions.z; z < m_nextPosition.z + box.dimensions.z; z++) {
                auto block = world.getBlock(x, y, z);

                if (block == 0 || !block.getData().isCollidable) continue;

                if (vel.y > 0) {
                    m_nextPosition.y = y - box.dimensions.y;
                    velocity.y = 0;
                }
                else if (vel.y < 0) {
                    m_isOnGround = true;
                    m_nextPosition.y = y + box.dimensions.y + 1;
                    velocity.y = 0;
                }

                if (vel.x > 0) {
                    m_nextPosition.x = x - box.dimensions.x;
                    velocity.x = 0;
                }
                else if (vel.x < 0) {
                    m_nextPosition.x = x + box.dimensions.x + 1;
                    velocity.x = 0;
                }

                if (vel.z > 0) {
                    m_nextPosition.z = z - box.dimensions.z;
                    velocity.z = 0;
                }
                else if (vel.z < 0) {
                    m_nextPosition.z = z + box.dimensions.z + 1;
                    velocity.z = 0;
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
    m_input.y = space - (m_isFlying && shift);
    m_input.z = d - a;

    m_isSneaking = !m_isFlying && shift;
    m_isSprinting = ctrl && !m_isSneaking ;
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
