
#include "render_window.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

render_window::render_window (unsigned width, unsigned height, const string &title)
    : _window (sf::VideoMode (width, height), title, sf::Style::Default, sf::ContextSettings (24))
{
    _window.setFramerateLimit (60);
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
                //ReshapeFunc (event.size.width, event.size.height);
                //glViewport(0, 0, event.size.width, event.size.height);
            }

            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) _window.close();
            }
        }

        _frameUpdateEvent (*this);


        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);

        glClearColor (1, 1, 1, 1);
        glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        _frameDrawEvent (*this);

        glFlush();
        _window.display();
    }
}
