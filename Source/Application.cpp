#include "Application.h"
#include "States/PlayState.h"
#include "World/Block/BlockDatabase.h"
#include <iostream>

Application::Application(const Config& config)
    : m_context(config)
    , m_camera(config)
    , m_config(config)
{
    BlockDatabase::get();
    pushState<StatePlay>(*this, config);
}

float g_timeElapsed = 0;

void Application::runLoop()
{
    sf::Clock dtTimer;
    sf::Clock dt;
    sf::Vector2i win_center;

    sf::Time m;

    // Grab the context window and force it to a certain position.
    // This prevents the window from sticking to the bottom of the visible screen like it does
    // in some Linux distros. Especially Arch.

    // If the window is small, use these parameters
    //if (m_context.window.getSize().x <= 640)
    //{
    //    win_center = {(float)sf::VideoMode::getDesktopMode().width / 3.5f,
    //                  (float)sf::VideoMode::getDesktopMode().height / 4.0f};
    //}
    //else // Else force it to the upper-leftgit p
    //{
    //    win_center = {0, 0};
    //}

    m_context.window.setPosition(win_center);
    
    /// @todo Make vsync a config option
    m_context.window.setVerticalSyncEnabled(true);

    while (m_context.window.isOpen() && !m_states.empty())
    {
        auto deltaTime = dtTimer.restart();
        auto& state = *m_states.back();

        state.handleInput();
        state.update(deltaTime.asSeconds());
        m_camera.update();

        m_context.window.clear();

        state.render(m_masterRenderer);
        m_masterRenderer.finishRender(m_context.window, m_camera);

        m_context.window.pushGLStates();
        state.drawUI(m_context.window);
        m_context.window.popGLStates();

        m_context.window.display();

        handleEvents();
        if (m_isPopState)
        {
            m_isPopState = false;
            m_states.pop_back();
        }

        m = dt.restart();

        g_timeElapsed += m.asSeconds();
    }
}

void Application::handleEvents()
{
    sf::Event e;
    while (m_context.window.pollEvent(e))
    {
        m_states.back()->handleEvent(e);
        switch (e.type)
        {
            case sf::Event::Closed:
                m_context.window.close();
                break;

            case sf::Event::KeyPressed:
                switch (e.key.code)
                {
                    case sf::Keyboard::Escape:
                        m_context.window.close();
                        break;

                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void Application::popState()
{
    m_isPopState = true;
}

void Application::turnOffMouse()
{
    m_context.window.setMouseCursorVisible(false);
}

void Application::turnOnMouse()
{
    m_context.window.setMouseCursorVisible(true);
}
