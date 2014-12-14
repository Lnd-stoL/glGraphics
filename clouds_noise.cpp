
#include "clouds_noise.hpp"

#include <thread>

using std::thread;
using math_ex::nextRandomFloat;

//----------------------------------------------------------------------------------------------------------------------

clouds_noise_3d::clouds_noise_3d (vector3<unsigned> dimensions) : _dimensions (dimensions)
{
    _noise = new uint8_t[dimensions.x() * dimensions.y() * dimensions.z()];
    _layerSize = dimensions.x() * dimensions.y();

    debug::log::println ("generating clouds ...");
    _generate();
    debug::log::println_mrk ("completed");
}


clouds_noise_3d::~clouds_noise_3d()
{
    if (_noise)  delete[] _noise;
}

//----------------------------------------------------------------------------------------------------------------------

float fade (float t)                      { return t * t * t * (t * (t * 6 - 15) + 10); }
float lerp (float t, float a, float b)    { return a + t * (b - a); }


float grad (int hash, float x, float y, float z)
{
    int h = hash & 15;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;

    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}


int p[512];
int permutation[] = { 151,160,137,91,90,15,
        131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
        190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
        88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
        77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
        102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
        135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
        5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
        129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
        251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
        49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
        138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};


inline int int_floor (float x)
{
    int i = (int) x;
    return i - ( i > x );
}


float noise (float x, float y, float z)
{
    int fx = int_floor (x);
    int fy = int_floor (y);
    int fz = int_floor (z);

    int X = fx & 255,
    Y = fy & 255,
    Z = fz & 255;
    x -= fx;
    y -= fy;
    z -= fz;
    double u = fade(x),
            v = fade(y),
            w = fade(z);
    int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z,
            B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),
                                grad(p[BA  ], x-1, y  , z   )),
                        lerp(u, grad(p[AB  ], x  , y-1, z   ),
                             grad(p[BB  ], x-1, y-1, z   ))),
                lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),
                             grad(p[BA+1], x-1, y  , z-1 )),
                     lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                          grad(p[BB+1], x-1, y-1, z-1 ))));
}

//----------------------------------------------------------------------------------------------------------------------

void clouds_noise_3d::_generate()
{
    for (int i=0; i < 256 ; i++) p[256+i] = p[i] = permutation[i];

    unsigned concurrentThreadsSupported = thread::hardware_concurrency();
    if (concurrentThreadsSupported == 0)  concurrentThreadsSupported = 3;
    unsigned overallThreadsCount = concurrentThreadsSupported;
    unsigned spawnThreadsCount = concurrentThreadsSupported - 1;

    unsigned overallComplexity = _dimensions.x() * _dimensions.y() * _dimensions.z();
    unsigned partComplexity = overallComplexity / overallThreadsCount;

    unsigned partWidth  = 0;
    unsigned partHeight = 0;
    unsigned partDepth  = 0;

    if (_layerSize >= partComplexity)
    {
        if (_dimensions.x() >= partComplexity)
        {
            partWidth = partComplexity;
            partHeight = 1;
        }

        else
        {
            unsigned stringsCount = partComplexity / _dimensions.x();
            if (partComplexity % _dimensions.x())  ++stringsCount;

            partHeight = stringsCount;
            partWidth = _dimensions.x();
        }

        partDepth = 1;
    }

    else
    {
        partWidth  = _dimensions.x();
        partHeight = _dimensions.y();

        unsigned numLayers = (partComplexity / _layerSize);
        if (partComplexity % _layerSize)  ++numLayers;
        partDepth  = numLayers;
    }


    unsigned partTileWidth  = _dimensions.x() / partWidth;
    if (_dimensions.x() % partWidth)  ++partTileWidth;

    unsigned partTileHeight = _dimensions.y() / partHeight;
    if (_dimensions.y() % partHeight)  ++partTileHeight;

    unsigned partTileDepth  = _dimensions.z() / partDepth;
    if (_dimensions.z() % partDepth)  ++partTileDepth;


    unsigned tileLayerSize = partTileHeight * partTileWidth;
    vector<unique_ptr<thread>>  threads (spawnThreadsCount);
    debug::log::println (mkstr ("starting ", threads.size(), " threads"));

    for (unsigned t0 = 0; t0 < partTileDepth; ++t0)
    {
        for (unsigned y0 = 0; y0 < partTileHeight; ++y0)
        {
            unsigned threadsArrayOffset = t0 * tileLayerSize + y0 * partTileWidth;
            for (unsigned x0 = 0; x0 < partTileWidth; ++x0)
            {
                partWidth  = std::min (partWidth,  _dimensions.x() - x0);
                partHeight = std::min (partHeight, _dimensions.y() - y0);
                partDepth  = std::min (partDepth,  _dimensions.z() - t0);

                auto partCalcFunction = [this, x0, y0, t0, partWidth, partHeight, partDepth] ()
                {
                    _generatePart (x0 * partWidth, y0 * partHeight, t0 * partDepth, partWidth, partHeight, partDepth);
                };

                unsigned threadIndex = threadsArrayOffset + x0;
                if (threadIndex == spawnThreadsCount)  partCalcFunction();
                else  threads[threadIndex] = unique_ptr<thread> (new thread (partCalcFunction));
            }
        }
    }

    for (unsigned i = 0; i < threads.size(); ++i)
    {
        threads[i]->join();
    }

    /*
    float *oldData = _noise;
    float *newData = new float[_dimensions.getX() * _dimensions.getY() * _dimensions.getZ()];

    int step = 32;
    vector3_f fetchCoordsDisp[8] = {
            {0, 0, 0},
            {step, 0, 0},
            {0, step, 0},
            {step, step, 0},
            {0, 0, step},
            {step, 0, step},
            {0, step, step},
            {step, step, step},
    };

    for (int t = 0; t < _dimensions.getZ(); ++t)
    {
        for (int y = 0; y < _dimensions.getY(); y += 1)
        {
            for (int x = 0; x < _dimensions.getX(); x += 1)
            {
                int nearestX = (x / step) * step;
                int nearestY = (y / step) * step;
                int nearestT = (t / step) * step;

                _noise = oldData;

                float fetchVals[8] = {};
                vector3_f nearestPoint (nearestX, nearestY, nearestT);
                for (unsigned i = 0; i < 8; ++i)  fetchVals[i] = _index (nearestPoint + fetchCoordsDisp[i]);

                float linearCoeffs[8] = {};
                float linearNorm = 0;
                float val = 0;
                for (unsigned i = 0; i < 8; ++i)
                {
                    vector3_f fetchCoords = (nearestPoint + fetchCoordsDisp[i]);
                    float len = step * sqrt (3) - (fetchCoords - vector3_f (x, y, t)).length();
                    linearCoeffs[i] = len;
                    linearNorm += len;
                    val += len * fetchVals[i];
                }

                val /= linearNorm;

                _noise = newData;
                _index (x, y, t, val);
            }
        }

        _noise = newData;
    }*/
}


void clouds_noise_3d::_generatePart (unsigned x0, unsigned y0, unsigned t0, unsigned width, unsigned height, unsigned depth)
{
    unsigned octavesCount = 6;
    float maxAmp = std::pow (0.5, octavesCount - 2);

    for (unsigned t = t0; t < t0 + depth; ++t)
    {
        for (unsigned y = y0; y < y0 + height; ++y)
        {
            unsigned rawDataOffset = t * _layerSize + y * _dimensions.x();
            for (unsigned x = x0; x < x0 + width; ++x)
            {
                //float nextRand = nextRandomFloat();

                float nextAmp = maxAmp;
                float summAmp = 0;
                float value = 0;
                float nextFreq = 0.25;

                for (unsigned i = 0; i < octavesCount; ++i)
                {
                    summAmp += nextAmp;
                    value += nextAmp * noise (float (x) * nextFreq, float (y) * nextFreq, float (t) * nextFreq * 16);

                    nextFreq /= 2;
                    nextAmp *= 2;
                }
                value /= 1.5f;

                float seamlessBlurRadiusX = float (0.06 * _dimensions.x());
                float seamlessBlurRadiusY = float (0.06 * _dimensions.y());

                if (x < seamlessBlurRadiusX)  value *= sqrt (float (x) / seamlessBlurRadiusX);
                if (y < seamlessBlurRadiusY)  value *= sqrt (float (y) / seamlessBlurRadiusY);

                if (x > _dimensions.x() - seamlessBlurRadiusX)  value *= sqrt (float (_dimensions.x() - x) / seamlessBlurRadiusX);
                if (y > _dimensions.y() - seamlessBlurRadiusY)  value *= sqrt (float (_dimensions.y() - y) / seamlessBlurRadiusY);


                _noise[rawDataOffset + x] = (uint8_t) (std::max (0.08f, (std::min (value * value, 0.999f))) * 255);
            }
        }
    }
}
