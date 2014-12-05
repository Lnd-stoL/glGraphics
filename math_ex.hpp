
#ifndef __math_ex__included__
#define __math_ex__included__

//----------------------------------------------------------------------------------------------------------------------

#include <cmath>

//----------------------------------------------------------------------------------------------------------------------

namespace math_ex
{
    template<typename Op_T>
    inline Op_T square (Op_T op)
    {
        return op * op;
    }


    template<typename Op_T>
    inline Op_T squareRoot (Op_T op)
    {
        return std::sqrt (op);
    }


    inline float nextRandomFloat()
    {
        return float (rand()) /  RAND_MAX;
    }


    inline float linearStep (float from, float to, float val)
    {
        if (val <= from)  return 0;
        if (val >= to)    return 1;

        return (val - from) / (from - to);
    }
}

//----------------------------------------------------------------------------------------------------------------------

#endif
