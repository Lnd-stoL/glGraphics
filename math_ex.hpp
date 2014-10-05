
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
}

//----------------------------------------------------------------------------------------------------------------------

#endif
