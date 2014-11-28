
#include "statistics.hpp"
#include "scene.hpp"

//----------------------------------------------------------------------------------------------------------------------

statistics::statistics (render_window &renderWindow, resources &res, screen_overlay_layer::ptr overlayLayer) :
    _renderWindow (renderWindow),
    _resources (res),
    _overlayLayer (overlayLayer)
{
    _defaultFont = font::alloc ("bitmap-font.png", _resources);

    _frameTimeLabel = text_label::alloc (_defaultFont,
                                         math3d::vector2_f (0.003, 0.003),
                                         math3d::vector2_f (0.031, 0.04),
                                         mkstr ("-------------------"));

    _frameTimeLabel->setColor (color_rgb<float> (1, 1, 1));
    _overlayLayer->addOverlay (_frameTimeLabel);

    _avgFrameTimeLabel = text_label::alloc (_defaultFont,
                                            math3d::vector2_f (0.003, 0.023),
                                            math3d::vector2_f (0.025, 0.031),
                                            mkstr ("-------------------"));

    _avgFrameTimeLabel->setColor (color_rgb<float> (0.8, 0.8, 0.8));
    _overlayLayer->addOverlay (_avgFrameTimeLabel);

    _renderLabel = text_label::alloc (_defaultFont,
                                      math3d::vector2_f (0.003, 0.045),
                                      math3d::vector2_f (0.026, 0.036),
                                      mkstr ("-----------------------------"));

    _renderLabel->setColor (color_rgb<float> (0.9, 0.9, 0.5));
    _overlayLayer->addOverlay (_renderLabel);
}


void statistics::draw (graphics_renderer &renderer)
{
    if ((int)renderer.getFrameCount() % 10 == 0)
    {
        double frameTime = _renderWindow.getFrameTime();
        auto frameStatistics = renderer.getLastFrameStatistics();

        string frameTimeLabelText = mkstr ((unsigned) (1000.0 / frameTime), " (", std::setprecision (3), (float)frameTime, " ms)");
        _frameTimeLabel->changeText (frameTimeLabelText);

        string avgFrameTimeLabelText = mkstr ("on average: ", (unsigned) (1000.0 / frameTime), " (", std::setprecision (3), (float)frameTime, " ms)");
        _avgFrameTimeLabel->changeText (avgFrameTimeLabelText);

        string renderLabelText = mkstr (frameStatistics.getDrawCalls(), " draw calls");
        _renderLabel->changeText (renderLabelText);
    }

    _overlayLayer->draw (renderer);
}
