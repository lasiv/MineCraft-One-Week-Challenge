#include "FPSCounter.h"

#include "../Renderer/RenderMaster.h"
#include "../Player/Player.h"
#include "../World/World.h"

#include <iostream>
#include <sstream> 
#include <iomanip>

/// @todo add fps counter to the config file and render it to the screen
FPSCounter::FPSCounter()
{
    enabled = true;
    debugging = false;

    m_text.setPosition(sf::Vector2f(10.f,10.f));
    //m_text.move(10, 10);
    m_text.setOutlineColor(sf::Color::Black);
    m_text.setOutlineThickness(2);

    if (!m_font.loadFromFile("Res/Fonts/rs.ttf")) 
    {
        std::cerr << "FEHLER: Font konnte nicht geladen werden!\n";
    }

    m_text.setFont(m_font);
    m_text.setCharacterSize(25);
}

void FPSCounter::update()
{
    m_frameCount++;

    if(enabled)
    {
        if (m_delayTimer.getElapsedTime().asSeconds() > 0.5)
        {
            m_fps = m_frameCount / m_fpsTimer.restart().asSeconds();
            m_frameCount = 0;
            m_delayTimer.restart();
        
            // Only show this output in debug mode
            if(debugging)
            {
                std::cout << m_fps << '\n';
            }
        }
    }
}

void FPSCounter::draw(sf::RenderWindow &window, World &world, Player &player)
{
    //std::cout << "FPSCounter::draw called, fps: " << m_fps << "\n";
    std::stringstream ss;

    ss << std::fixed << std::setprecision(2)
                << "Pos(" 
                << player.position.x << ", "
                << player.position.y << ", "
                << player.position.z << ")\n"
                << "Chunk("
                << world.getChunkXZ(std::floor(player.position.x), std::floor(player.position.z)).x << ", "
                << world.getChunkXZ(std::floor(player.position.x), std::floor(player.position.z)).z << ")\n"
                << "ChunkBlock("
                << world.getBlockXZ(std::floor(player.position.x), std::floor(player.position.z)).x << ", "
                << world.getBlockXZ(std::floor(player.position.x), std::floor(player.position.z)).z << ")\n"
                << "Rot("
                << player.rotation.x << ", "
                << player.rotation.y << ", "
                << player.rotation.z << ")\n"
                << "Vel("
                << player.velocity.x << ", "
                << player.velocity.y << ", "
                << player.velocity.z << ")" << std::endl;

    m_text.setString(ss.str() + "FPS: " + std::to_string(static_cast<int>(m_fps)));

    window.draw(m_text);
}
