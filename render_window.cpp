
#include "render_window.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

render_window::render_window (unsigned width, unsigned height, const string &title)
    : _window (sf::VideoMode (width, height), title, sf::Style::Default, sf::ContextSettings (24, 0, 4))
{
    _window.setFramerateLimit (60);

    //glewExperimental = GL_TRUE;
    glewInit();
}


void render_window::runEventLoop()
{
    while (_window.isOpen())
    {
        sf::Event event;
        while (_window.pollEvent (event))
        {
            if (event.type == sf::Event::Closed)  _window.close();

            else if (event.type == sf::Event::Resized)
            {
                _handleWindowResize (event.size.width, event.size.height);
            }

            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) _window.close();
                if (event.key.code == sf::Keyboard::F12)
                {
                    auto tt = time (nullptr);
                    saveScreenshot (mkstr ("screenshots/screenshot-", tt, ".jpg"));
                }
            }
        }

        _frameUpdateEvent (*this);

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        _frameDrawEvent (*this);

        _window.display();
    }
}


void render_window::_handleWindowResize (unsigned width, unsigned height)
{
    //glFrustum(-1, 1, -1, 1, 1, 100000);
    _sizeChangedEvent (width, height);
    glViewport (0, 0, width, height);
}


double render_window::getAspectRatio() const
{
    return _window.getSize().x / _window.getSize().y;
}


void render_window::saveScreenshot (const string &fileToSave)
{
    _window.capture().saveToFile (fileToSave);
}


void render_window::clear()
{
    glClearColor (1, 1, 1, 1);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


unsigned render_window::getWidth() const
{
    return _window.getSize().x;
}


unsigned render_window::getHeight() const
{
    return _window.getSize().y;
}
