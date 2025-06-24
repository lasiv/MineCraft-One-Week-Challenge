#include "FPSCounter.h"

#include "../Renderer/RenderMaster.h"

#include <iostream>

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

void FPSCounter::draw(sf::RenderWindow &window)
{
    //std::cout << "FPSCounter::draw called, fps: " << m_fps << "\n";
    m_text.setString("FPS: " + std::to_string(static_cast<int>(m_fps))); // falls du es hier lassen willst
    //window.pushGLStates();
    //window.resetGLStates();
    window.draw(m_text);
    //window.popGLStates();
}
