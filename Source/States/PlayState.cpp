#include "PlayState.h"

#include "../Application.h"
#include "../Maths/Ray.h"
#include "../Renderer/RenderMaster.h"
#include "../World/Event/PlayerDigEvent.h"

#include <iostream>
#include <sstream>
#include <iomanip>

StatePlay::StatePlay(Application &app, const Config &config)
    : StateBase(app)
    , m_world(app.getCamera(), config, m_player)
{
    app.getCamera().hookEntity(m_player);

    m_debugText.setPosition(sf::Vector2f(10.f,35.f));
    //m_text.move(10, 10);
    m_debugText.setOutlineColor(sf::Color::Black);
    m_debugText.setOutlineThickness(2);

    if (!m_font.loadFromFile("Res/Fonts/rs.ttf")) 
    {
        std::cerr << "FEHLER: Font konnte nicht geladen werden!\n";
    }

    m_debugText.setFont(m_font);
    m_debugText.setCharacterSize(25);

    m_crosshair.setRadius(3);
    m_crosshair.setOrigin(3, 3);
    m_crosshair.setPosition(app.getWindow().getSize().x/2, app.getWindow().getSize().y/2);
}

void StatePlay::handleEvent(sf::Event e)
{
    m_keyboard.update(e);
}

void StatePlay::handleInput()
{
    m_player.handleInput(m_pApplication->getWindow(), m_keyboard);

    static sf::Clock timer;
    glm::vec3 lastPosition;

    // Ray is cast as player's 'vision'
    Ray ray({m_player.position.x, m_player.position.y + 0.6f, m_player.position.z},
            m_player.rotation);

    bool face = false;

    // Determine what the player is trying to do
    if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        face = true;
    }
    else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        return; // Neither button pressed
    }

    if (timer.getElapsedTime().asSeconds() > 0.2f) {
        if (ray.cast(m_world, 6.0f, face)) {
            timer.restart();

            // m_rayEnd is now the block to interact with
            m_world.addEvent<PlayerDigEvent>(
                face ? sf::Mouse::Right : sf::Mouse::Left,
                ray.getEnd(),
                m_player
            );
        }
    }

}

void StatePlay::update(float deltaTime)
{
    if (m_player.position.x < 0)
        m_player.position.x = 0;
    if (m_player.position.z < 0)
        m_player.position.z = 0;

    m_fpsCounter.update();
    m_player.update(deltaTime, m_world);
    m_world.update(m_pApplication->getCamera());
}

void StatePlay::render(RenderMaster &renderer)
{
    m_world.renderWorld(renderer, m_pApplication->getCamera());
}

static const char* getDirection(glm::vec3 direction) {
    float angle = fmodf(direction.y, 360.0f);
    if (angle < 0) angle += 360.0f;  // Normalize to [0, 360)

    if (angle >= 315.0f || angle < 45.0f) {
        return "North (-z)";
    } else if (angle >= 45.0f && angle < 135.0f) {
        return "East (+x)";
    } else if (angle >= 135.0f && angle < 225.0f) {
        return "South (+z)";
    } else { // 225 <= angle < 315
        return "West (-x)";
    }
}


void StatePlay::drawDebugInfo(sf::RenderWindow &window)
{
    std::stringstream ss;

    Ray ray = Ray({0,0,0}, m_player.rotation);

    ss << std::fixed << std::setprecision(2)

                << "Pos(" 
                << m_player.position.x << ", "
                << m_player.position.y << ", "
                << m_player.position.z << ")\n"
                << "Chunk("
                << m_world.getChunkXZ(std::floor(m_player.position.x), std::floor(m_player.position.z)).x << ", "
                << m_world.getChunkXZ(std::floor(m_player.position.x), std::floor(m_player.position.z)).z << ")\n"
                << "ChunkBlock("
                << m_world.getBlockXZ(std::floor(m_player.position.x), std::floor(m_player.position.z)).x << ", "
                << m_world.getBlockXZ(std::floor(m_player.position.x), std::floor(m_player.position.z)).z << ")\n"
                << "Rot("
                << m_player.rotation.x << ", "
                << m_player.rotation.y << ", "
                << m_player.rotation.z << ")\n"
                << "Facing: " << getDirection(m_player.rotation) << "\n"
                << "RotVec("
                << ray.getDirection().x << ", "
                << ray.getDirection().y << ", "
                << ray.getDirection().z << ")\n"
                << "Vel("
                << m_player.velocity.x << ", "
                << m_player.velocity.y << ", "
                << m_player.velocity.z << ")\n"  
                
                << " 1: (Id: " << m_player.getInventory().getItemOfSlot(0).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(0).getNumInStack() << ")\n"
                << " 2: (Id: " << m_player.getInventory().getItemOfSlot(1).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(1).getNumInStack() << ")\n"
                << " 3: (Id: " << m_player.getInventory().getItemOfSlot(2).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(2).getNumInStack() << ")\n"
                << " 4: (Id: " << m_player.getInventory().getItemOfSlot(3).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(3).getNumInStack() << ")\n"
                << " 5: (Id: " << m_player.getInventory().getItemOfSlot(4).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(4).getNumInStack() << ")\n"
                << " 6: (Id: " << m_player.getInventory().getItemOfSlot(5).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(5).getNumInStack() << ")\n"
                << " 7: (Id: " << m_player.getInventory().getItemOfSlot(6).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(6).getNumInStack() << ")\n"
                << " 8: (Id: " << m_player.getInventory().getItemOfSlot(7).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(7).getNumInStack() << ")\n"
                << " 9: (Id: " << m_player.getInventory().getItemOfSlot(8).getMaterial().id << " Num: " << m_player.getInventory().getItemOfSlot(8).getNumInStack()

                << std::endl;

    m_debugText.setString(ss.str());

    window.draw(m_debugText);
}

void StatePlay::drawUI(sf::RenderWindow &window)
{
    static bool drawDebug = false;
    static ToggleKey debugKey(sf::Keyboard::F3);

    if (debugKey.isKeyPressed()) {
        drawDebug = !drawDebug;
        //std::cout << "drawDebug toggled: " << drawDebug << "\n";
    }

    if (drawDebug) {
        drawDebugInfo(window);
        m_fpsCounter.draw(window);
    }

    if (m_player.getInventory().getVisibility() == true)
    {
        m_player.drawInventory(window);
    }

    window.draw(m_crosshair);

}

void StatePlay::onOpen()
{
    m_pApplication->turnOffMouse();
}
