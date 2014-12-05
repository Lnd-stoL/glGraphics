
#include "statistics.hpp"
#include "graphics_renderer.hpp"

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
                                         mkstr ("---------------------"));

    _frameTimeLabel->labelColor (color_rgb<float> (1, 1, 1));
    _overlayLayer->addOverlay (_frameTimeLabel);

    _avgFrameTimeLabel = text_label::alloc (_defaultFont,
                                            math3d::vector2_f (0.003, 0.023),
                                            math3d::vector2_f (0.025, 0.031),
                                            mkstr ("----------------------"));

    _avgFrameTimeLabel->labelColor (color_rgb<float> (0.8, 0.8, 0.8));
    _overlayLayer->addOverlay (_avgFrameTimeLabel);

    _renderLabel = text_label::alloc (_defaultFont,
                                      math3d::vector2_f (0.003, 0.045),
                                      math3d::vector2_f (0.026, 0.036),
                                      mkstr ("-------------------------------"));

    _renderLabel->labelColor (color_rgb<float> (0.9, 0.9, 0.5));
    _overlayLayer->addOverlay (_renderLabel);

    _trianglesLabel = text_label::alloc (_defaultFont,
                                         math3d::vector2_f (0.003, 0.065),
                                         math3d::vector2_f (0.026, 0.036),
                                         mkstr ("-------------------------------"));

    _trianglesLabel->labelColor (color_rgb<float> (0.9, 0.9, 0.5));
    _overlayLayer->addOverlay (_trianglesLabel);
}


void statistics::draw (graphics_renderer &renderer)
{
    if ((unsigned) renderer.frameCount() % _updatePeriodFrames == 0)
    {
        double frameTime = _renderWindow.frameTime();
        double avgFrameTime = _renderWindow.averageFrameTime();
        auto frameStatistics = renderer.lastFrameStatistics();

        auto frametimeStr = ((std::stringstream&)(std::ostringstream() << std::setprecision (3) << (float) frameTime)).str();
        auto avgFrametimeStr = ((std::stringstream&)(std::ostringstream() << std::setprecision (3) << (float) avgFrameTime)).str();

        string frameTimeLabelText = mkstr (int (std::floor (1000.0 / frameTime)), " (", frametimeStr, " ms)");
        _frameTimeLabel->changeText (frameTimeLabelText);

        string avgFrameTimeLabelText = mkstr ("on average: ", int (std::floor (1000.0 / avgFrameTime)),
                                              " (", avgFrametimeStr, " ms)");
        _avgFrameTimeLabel->changeText (avgFrameTimeLabelText);

        string renderLabelText = mkstr (frameStatistics.drawCalls(), " draw calls");
        _renderLabel->changeText (renderLabelText);

        string triangleCountLabelText = mkstr (frameStatistics.triangleCount() / 1000, " ktris");
        _trianglesLabel->changeText (triangleCountLabelText);
    }

    _overlayLayer->draw (renderer);
}
