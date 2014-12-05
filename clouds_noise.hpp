
#ifndef __clouds_noise_included__
#define __clouds_noise_included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"

using math3d::vector2_f;
using math3d::vector3_f;
using math3d::vector3;

//----------------------------------------------------------------------------------------------------------------------

class clouds_noise_3d
{
    vector3<unsigned>  _dimensions;
    uint8_t  *_noise = nullptr;
    unsigned  _layerSize = 0;

public:
    property_get (voxelRawData, _noise);


protected:
    void _generate();
    void _generatePart (unsigned x, unsigned y, unsigned t, unsigned width, unsigned height, unsigned depth);


public:
    clouds_noise_3d (vector3<unsigned> dimensions);
    ~clouds_noise_3d();
};

//----------------------------------------------------------------------------------------------------------------------

#endif
