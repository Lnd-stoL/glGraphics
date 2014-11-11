
#include "statistics.hpp"

//----------------------------------------------------------------------------------------------------------------------

statistics::statistics (graphics_renderer &renderer, render_window &renderWindow, resources &res) :
        _renderer (renderer),
        _renderWindow (renderWindow),
        _resources (res)
{
    //font::ptr textFont = renderRes.requestFromFile<font> ("default-semibold.ttf");
    //sf::Text text ("CPU frame time: 1ms", *((sf::Font*)textFont.get()), 18);
    //text.setColor (sf::Color (220, 50, 50, 200));
    //text.setPosition (5.0f, 5.0f);

    //glBindBuffer (GL_ARRAY_BUFFER, 0);
    //glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}
