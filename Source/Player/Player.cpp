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
    : Entity({2500, 125, 2500}, {0.f, 0.f, 0.f}, {0.3f, 0.9f, 0.3f}) // collision will break when aabb box vertical dimension bigger 1.f
    , m_itemDown(sf::Keyboard::Down)
    , m_itemUp(sf::Keyboard::Up)
    , m_flyKey(sf::Keyboard::F)
    , m_num1(sf::Keyboard::Num1)
    , m_num2(sf::Keyboard::Num2)
    , m_num3(sf::Keyboard::Num3)
    , m_num4(sf::Keyboard::Num4)
    , m_num5(sf::Keyboard::Num5)
    , m_spawnPoint({2500, 125, 2500})
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

void Player::printdebug(World &world) const
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
                << m_input.z << ")  "
                << "bounce="
                << m_bounce
                << std::flush;
    
    // 2) sample the block at the player's feet
    int bx = int(std::floor(position.x - .5f)) + 1 ;
    int by = int(std::floor(position.y + box.dimensions.y)); 
    int bz = int(std::floor(position.z - .5f));

    // world.updateChunk(bx, by, bz);
    // world.setBlock(bx, by, bz, 7);

    // auto block = world.getBlock(bx, by, bz);
    // int blockID = block.id;  // your ChunkBlock.id member

    // std::cout << "BlockID=" << blockID
    //           << std::flush;
}

void Player::setPosition(glm::vec3 pos) {
    position = pos;
    m_nextPosition = pos;
}

void Player::setSpawn(glm::vec3 pos) {
    m_spawnPoint = pos;
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

// known problems: linear interpolation between ticks makes sudden changes feel floaty, might change by using past changes to blend the changes better.
void Player::update(float dt, World &world) {

    static float alpha = 0.0f;
    float last_alpha = alpha;
    alpha += dt / TICK;

    for (; alpha > 1.f; alpha--) {
        last_alpha = 0.f;
        calculate(world);
        move(world);
        printdebug(world);
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
        velocity.y = velocity.y * DEFAULT_SLIPPERINESS * BASE_FRICTION + m_input.y * GROUND_ACCEL_BASE;
    }
    else if (m_isInWater) {
        velocity.y = (velocity.y - WATER_GRAVITY) * WATER_DRAG + m_input.y * WATER_ACCEL_BASE;
    }
    else {
        // Initiate bounce
        if (m_isOnGround && m_bounce > BOUNCE_TRESHOLD) {
            velocity.y = m_bounce; // m_bounce already is the velocity as calculated in the tick before
        }
        // Initiate jump
        else if (m_isOnGround && m_input.y == 1) {
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
    const float slip       = m_slipperiness;
    const float friction   = slip * (m_isInWater ? WATER_FRICTION : BASE_FRICTION);
    const float accelBase  = m_isOnGround ? GROUND_ACCEL_BASE : AIR_ACCEL_BASE;

    // Movement multipliers
    const float mov       = m_isSprinting ? MOVE_MULT_SPRINT
                                : m_isSneaking ? MOVE_MULT_SNEAK
                                               : MOVE_MULT_WALK;
    const float movMult   = (m_input.x && m_input.z)
                                ? (m_isSneaking ? DIR_MULT_SNEAK_45
                                               : DIR_MULT_STRAFE_45)
                                : DIR_MULT_DEFAULT
                            * (m_isFlying ? 2 : 1);
    const float slipCube  = m_isOnGround ? cube(0.6f / slip) : 1.0f;

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

    
}

//

void Player::move(World &world) {
    // sneaking detection will only work, if the hitbox is above 1 bot lower then two blocks in height and less than a block in width each direction, as this is the intended size for the player figure
    // using engine conventions: +x is east, +z ist south
    // has some small inconsistancy in some edge cases i cant figure out with sneaking
    // could be more optimized with 0 velocity checks and no collision when edge sneak was detected etc

    if (position.y > RESPAWN_HEIGHT || position.y < 0.f) {
        m_nextPosition = m_spawnPoint;
        return;
    }
    
    LocalAABB localbox = LocalAABB(position, velocity, box);
    
    // check collision y level
    localbox.movey(velocity.y);
    localbox.getBlocks(world);
    bool movDown = (velocity.y < 0);
    
    m_isOnGround = false;
    // loop horizontal slice at array y 1 or 3 (head or feet)
    for (int i = 2 * (movDown ? 1 : 3); i < 16; i += 1 + ((i % 2) * 6)) { 

        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

        if (block != BlockId::Air && block.getData().isCollidable) {

            m_isOnGround = movDown;

            velocity.y = 0;
            
            int blocky = (*(&localbox.coords[0][0][0] + i)).y;

            if (movDown) localbox.setMinY((float)blocky + 1.f);
            else localbox.setMaxY((float)blocky);

            break;
        }
    }


    // check collision and sneaking edge x direction
    localbox.movex(velocity.x);
    localbox.getBlocks(world);

    bool movEast = (velocity.x > 0);
    bool collide = false;
    
    // sneaking
    if (m_isSneaking && m_isOnGround) {
        bool ground = false;
        // loop horizontal slice at array y 0 (below feet)
        for (int i = 0; i < 16; i += 1 + ((i % 2) * 6)) {

            ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

            if (block != BlockId::Air && block.getData().isCollidable) {
                ground = true;
                break;
            }
        }
        if (!ground) {

            velocity.y = 0;

            if (movEast) localbox.setMinX(std::floor(localbox.min.x - 0.001f) - 0.001f);
            else localbox.setMaxX(std::floor(localbox.max.x + 0.001f) + 1.001f);
        }
    }
    
    // collision
    // loop vertical slice at x 0 or 1 (dependent on direction) excluding array y 0
    for (int i = 8 * (movEast ? 1 : 0) + 2; i < 8 + (movEast ? 8 : 0); ++i) {

        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

        if (block != BlockId::Air && block.getData().isCollidable) {

            velocity.x = 0;

            int blockx = (*(&localbox.coords[0][0][0] + i)).x;

            if (movEast) localbox.setMaxX((float)blockx - 0.001f);
            else localbox.setMinX((float)blockx + 1.f);

            break;
        }
    }

    // check collision and sneaking edge z direction
    localbox.movez(velocity.z);
    localbox.getBlocks(world);

    bool movSouth = (velocity.z > 0);

    // sneaking
    if (m_isSneaking && m_isOnGround) {
        bool ground = false;
        // loop horizontal slice at array y 0 (below feet)
        for (int i = 0; i < 16; i += 1 + ((i % 2) * 6)) {

            ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

            if (block != BlockId::Air && block.getData().isCollidable) {
                ground = true;
                break;
            }
        }
        if (!ground) {
            
            velocity.y = 0;

            if (movSouth) localbox.setMinZ(std::floor(localbox.min.z - 0.001f) - 0.001f);
            else localbox.setMaxZ(std::floor(localbox.max.z + 0.001f) + 1.001f);
        }
    }

    // collision
    // loop vertical slice at z 0 or 1 (dependent on direction) excluding array y 0
    for (int i = (movSouth ? 1 : 0) + 2; i < 16; i += 2 + ((i % 8 > 5) ? 2 : 0)) {
        
        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);
        
        if (block != BlockId::Air && block.getData().isCollidable) {

            velocity.z = 0;

            int blockz = (*(&localbox.coords[0][0][0] + i)).z;

            if (movSouth) localbox.setMaxZ((float)blockz - 0.001f);
            else localbox.setMinZ((float)blockz + 1.f);

            break;
        }
    }
    
    // set slipperiness and bounce
    m_slipperiness = DEFAULT_SLIPPERINESS;
    for (int i = m_isOnGround ? 0 : 2; i < 16; i += 1 + ((i % 2) * 6)) {

        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

        float slip = block.getData().slip / 100.f;
        if (slip < m_slipperiness) m_slipperiness = slip;
    }

    m_bounce = (movDown && m_isOnGround) ? 1.f : 0.f;
    for (int i = 0; i < 16; i += 1 + ((i % 2) * 6)) {

        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

        float bounce = block.getData().bounce;
        if (block == BlockId::Air || !block.getData().isCollidable) continue;
        if (bounce < m_bounce) m_bounce = bounce;
    }
    if (m_bounce > BOUNCE_TRESHOLD) {
         m_bounce *= glm::abs(localbox.velocity.y);
    } 

    // apply position
    m_nextPosition = localbox.position;

    // check water
    // moves position to avoid jittering at surface swimming
    if (movDown) localbox.movey(WATER_ENTRY_HEIGHT); 
    localbox.getBlocks(world);
    m_isInWater = false;
    // checks horizontal slice at array y 1 (feet)
    for (int i = 2; i < 16; i += 1 + ((i % 2) * 6)) {

        ChunkBlock block = *(&localbox.blocks[0][0][0] + i);

        if (block == BlockId::Water) {

            m_isInWater = true;
            m_isOnGround = false;

            break;
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
