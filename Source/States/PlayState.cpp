#include "PlayState.h"

#include "../Application.h"
#include "../Maths/Ray.h"
#include "../Renderer/RenderMaster.h"
#include "../World/Event/PlayerDigEvent.h"

#include <iostream>

StatePlay::StatePlay(Application &app, const Config &config)
    : StateBase(app)
    , m_world(app.getCamera(), config, m_player)
{
    app.getCamera().hookEntity(m_player);
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
    for (Ray ray({m_player.position.x, m_player.position.y + 0.6f,
                  m_player.position.z},
                 m_player.rotation); // Corrected for camera offset
         ray.getLength() < 6; ray.step(0.05f)) {
        int x = static_cast<int>(ray.getEnd().x);
        int y = static_cast<int>(ray.getEnd().y);
        int z = static_cast<int>(ray.getEnd().z);

        auto block = m_world.getBlock(x, y, z);
        auto id = (BlockId)block.id;
        
        /// @todo add the keyboard handling to Player object
        if (id != BlockId::Air && id != BlockId::Water) {
            if (timer.getElapsedTime().asSeconds() > 0.2) {
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    timer.restart();
                    // The player "digs" the block up
                    m_world.addEvent<PlayerDigEvent>(sf::Mouse::Left,
                                                     ray.getEnd(), m_player);
                    break;
                }
                else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                    timer.restart();
                    // Block is placed by player
                    m_world.addEvent<PlayerDigEvent>(sf::Mouse::Right,
                                                     lastPosition, m_player);
                    break;
                }
            }
        }
        lastPosition = ray.getEnd();
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
    static sf::Clock dt;

    static bool drawGUI = false;
    static ToggleKey drawKey(sf::Keyboard::F3);

    if (drawKey.isKeyPressed()) {
        drawGUI = !drawGUI;
    }

    if (drawGUI) {
        m_fpsCounter.draw(renderer);
        m_player.draw(renderer);
    }

    m_world.renderWorld(renderer, m_pApplication->getCamera());
}

void StatePlay::onOpen()
{
    m_pApplication->turnOffMouse();
}
