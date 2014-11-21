
#ifndef __math3D__included__
#define __math3D__included__

//----------------------------------------------------------------------------------------------------------------------

#include <stdexcept>
#include <iomanip>
#include <limits>

#include "oo_extensions.hpp"
#include "math_ex.hpp"
#include "debug.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace math3D
{

    template<typename numeric_t>
    class angle
    {
        numeric_t _angleRad = numeric_t();

    public:
        static constexpr numeric_t pi = (numeric_t) 3.14159265358979323846;


    public:
        angle() {  }
        angle (numeric_t angleNum) : _angleRad (angleNum) {  }


        operator numeric_t()
        {
            return _angleRad;
        }


        void normalize()
        {
            if (_angleRad > 2*pi)
            {
                numeric_t coeff = _angleRad / (2*pi);
                coeff = (int) coeff;

                // TODO: implement it
            }
        }


        numeric_t getRad() const
        {
            return _angleRad;
        }


        template<typename another_numeric_t>
        angle<another_numeric_t> convertType() const
        {
            return angle<another_numeric_t> (_angleRad);
        }


        void operator += (const angle<numeric_t> rot)
        {
            _angleRad += rot._angleRad;
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class interval
    {
    public:
        typedef interval<numeric_t> this_t;

    protected:
        numeric_t _begin = numeric_t();
        numeric_t _end   = numeric_t();

    public:
        property_get (From, _begin)
        property_get (To,   _end)


    public:
        interval()
        {  }

        interval (numeric_t from, numeric_t to) : _begin (from), _end (to)
        {  }


        numeric_t length() const
        {
            return std::abs (_end - _begin);
        }


        void sort()
        {
            if (_begin > _end) inverse();
        }


        numeric_t average() const
        {
            return borderSumm() / 2;
        }


        numeric_t borderSumm() const
        {
            return _begin + _end;
        }


        void inverse()
        {
            std::swap (_begin, _end);
        }


        bool intersects (const this_t &operand) const
        {
            return _end >= operand._begin && _begin <= operand._end;
        }


        this_t intersection (const this_t &operand) const
        {
            this_t temp (*this);
            temp.intersect (operand);
            return temp;
        }


        void intersect (const this_t &operand)
        {
            _begin = std::max (_begin, operand._begin);
            _end   = std::min (_end, operand._end);

            if (_end < _begin) _end = _begin;
        }


        template<typename another_numeric_t>
        interval<another_numeric_t> convertType() const
        {
            return interval<another_numeric_t> (_begin, _end);
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class vector2
    {
    public:
        typedef vector2<numeric_t>  this_t;

    protected:
        numeric_t _x, _y;

    public:
        property_rw (X, _x)
        property_rw (Y, _y)


    public:
        vector2() : _x (numeric_t()),
                    _y (numeric_t())
        {  }


        vector2 (numeric_t x_, numeric_t y_) : _x (x_),
                                               _y (y_)
        {  }


        vector2 (numeric_t x_) : _x (x_),
                                 _y (x_)
        {  }


        static this_t zero()
        {
            return this_t (0, 0);
        }


        static this_t ident()
        {
            return this_t (1, 1);
        }


        void add (const this_t &operand)
        {
            _x += operand._x;
            _y += operand._y;
        }


        void subtract (const this_t &operand)
        {
            _x -= operand._x;
            _y -= operand._y;
        }


        void scale (numeric_t operand)
        {
            _x *= operand;
            _y *= operand;
        }


        void divide (numeric_t operand)
        {
            _x /= operand;
            _y /= operand;
        }


        numeric_t squaredLength() const
        {
            return math_ex::square (_x) + math_ex::square (_y);
        }


        numeric_t length() const
        {
            return math_ex::squareRoot (squaredLength());
        }


        void normalize()
        {
            numeric_t len = length();
            divide (len);
        }


        this_t normalized() const
        {
            this_t temp (*this);
            temp.normalize();

            return std::move (temp);
        }


        numeric_t dotProduct (const this_t &operand) const
        {
            return (_x * operand._x) + (_y * operand._y);
        }


        void inverse()
        {
            _x = -_x;
            _y = -_y;
        }


        this_t inversed() const
        {
            this_t temp (*this);
            temp.inverse();

            return std::move (temp);
        }


        void inverseScale()
        {
            _x = 1 / _x;
            _y = 1 / _y;
        }


        this_t inverseScaled() const
        {
            this_t temp (*this);
            temp.inverseScale();

            return std::move (temp);
        }


        void operator+= (const this_t &operand)
        {
            add (operand);
        }


        void operator-= (const this_t &operand)
        {
            subtract (operand);
        }


        void operator*= (numeric_t operand)
        {
            scale (operand);
        }


        void operator/= (numeric_t operand)
        {
            divide (operand);
        }


        this_t operator+ (const this_t &operand) const
        {
            this_t temp (*this);
            temp.add (operand);

            return std::move (temp);
        }


        this_t operator- (const this_t &operand) const
        {
            this_t temp (*this);
            temp.subtract (operand);

            return std::move (temp);
        }


        this_t operator* (numeric_t operand) const
        {
            this_t temp (*this);
            temp.scale (operand);

            return std::move (temp);
        }


        numeric_t operator* (const this_t &operand) const
        {
            return dotProduct (operand);
        }


        this_t operator/ (numeric_t operand) const
        {
            this_t temp (*this);
            temp.divide (operand);

            return std::move (temp);
        }


        this_t operator-() const
        {
            return inversed();
        }


        bool operator== (const this_t &operand) const
        {
            return _x == operand._x && _y == operand._y;
        }


        bool operator!= (const this_t &operand) const
        {
            return !(this->operator== (operand));
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class vector3
    {
    public:
        typedef vector3<numeric_t>  this_t;

    protected:
        numeric_t _x, _y, _z;


    public:
        property_rw (X, _x)
        property_rw (Y, _y)
        property_rw (Z, _z)


    public:
        vector3() : _x (numeric_t()),
                    _y (numeric_t()),
                    _z (numeric_t())
        {  }


        vector3 (numeric_t x_, numeric_t y_, numeric_t z_) : _x (x_),
                                                             _y (y_),
                                                             _z (z_)
        {  }


        vector3 (numeric_t x_) : _x (x_),
                                 _y (x_),
                                 _z (x_)
        {  }


        template<typename another_numeric_t>
        vector3<another_numeric_t> convertType() const
        {
            return vector3<another_numeric_t> ((another_numeric_t) _x, (another_numeric_t) _y, (another_numeric_t) _z);
        }


        static this_t zero()
        {
            return this_t (0, 0, 0);
        }


        static this_t ident()
        {
            return this_t (1, 1, 1);
        }


        void add (const this_t &operand)
        {
            _x += operand._x;
            _y += operand._y;
            _z += operand._z;
        }


        void subtract (const this_t &operand)
        {
            _x -= operand._x;
            _y -= operand._y;
            _z -= operand._z;
        }


        void scale (numeric_t operand)
        {
            _x *= operand;
            _y *= operand;
            _z *= operand;
        }


        void divide (numeric_t operand)
        {
            _x /= operand;
            _y /= operand;
            _z /= operand;
        }


        numeric_t squaredLength() const
        {
            return math_ex::square (_x) + math_ex::square (_y) + math_ex::square (_z);
        }


        numeric_t length() const
        {
            auto sqLength = squaredLength();
            if (abs (sqLength - 1) < std::numeric_limits<numeric_t>::epsilon() * 2)  return 1;

            return math_ex::squareRoot (sqLength);
        }


        void normalize()
        {
            numeric_t len = length();

            if (len < std::numeric_limits<numeric_t>::epsilon())  return;
            if (len == 1)  return;

            divide (len);
        }


        this_t normalized() const
        {
            this_t temp (*this);
            temp.normalize();

            return std::move (temp);
        }


        numeric_t dotProduct (const this_t &operand) const
        {
            return (_x * operand._x) + (_y * operand._y) + (_z * operand._z);
        }


        this_t crossProduct (const this_t &operand) const
        {
            return this_t ((_y * operand._z) - (_z*operand._y),
                           (_z * operand._x) - (_x*operand._z),
                           (_x * operand._y) - (_y*operand._x));
        }


        void inverse()
        {
            _x = -_x;
            _y = -_y;
            _z = -_z;
        }


        this_t inversed() const
        {
            this_t temp (*this);
            temp.inverse();

            return std::move (temp);
        }


        void inverseScale()
        {
            _x = 1 / _x;
            _y = 1 / _y;
            _z = 1 / _z;
        }


        this_t inverseScaled() const
        {
            this_t temp (*this);
            temp.inverseScale();

            return std::move (temp);
        }


        void operator+= (const this_t &operand)
        {
            add (operand);
        }


        void operator-= (const this_t &operand)
        {
            subtract (operand);
        }


        void operator*= (numeric_t operand)
        {
            scale (operand);
        }


        void operator/= (numeric_t operand)
        {
            divide (operand);
        }


        this_t operator+ (const this_t &operand) const
        {
            this_t temp (*this);
            temp.add (operand);

            return std::move (temp);
        }


        this_t operator- (const this_t &operand) const
        {
            this_t temp (*this);
            temp.subtract (operand);

            return std::move (temp);
        }


        this_t operator* (numeric_t operand) const
        {
            this_t temp (*this);
            temp.scale (operand);

            return std::move (temp);
        }


        numeric_t operator* (const this_t &operand) const
        {
            return dotProduct (operand);
        }


        this_t operator/ (numeric_t operand) const
        {
            this_t temp (*this);
            temp.divide (operand);

            return std::move (temp);
        }


        this_t operator-() const
        {
            return inversed();
        }


        bool operator== (const this_t &operand) const
        {
            return _x == operand._x && _y == operand._y && _z == operand._z;
        }


        bool operator!= (const this_t &operand) const
        {
            return !(this->operator== (operand));
        }


        string asString() const
        {
            return mkstr ("( ", _x, " ", _y, " ", _z, " )");
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t, int sideN>
    class matrix :
        public oo_extensions::i_as_string
    {
    public:
        typedef matrix<numeric_t, sideN> this_t;

    protected:
        numeric_t _matrix[sideN][sideN] = {};

    public:
        property_get (Side, sideN)


    protected:
        inline void _testBounds (int row, int col) const
        {
            if (row < 0 || col < 0 || row >= sideN || col >= sideN)
                throw std::range_error (mkstr (" invalid matrix indexing: ", col, " ", row, " are out of 0 ", sideN));
        }

    public:
        inline numeric_t get (int row, int col) const
        {
            _testBounds (row, col);
            return _matrix[row][col];
        }

        inline void set (int row, int col, numeric_t value)
        {
            _testBounds (row, col);
            _matrix[row, col] = value;
        }

        inline numeric_t& at (int row, int col)
        {
            _testBounds (row, col);
            return _matrix[row][col];
        }

        inline const numeric_t* raw() const
        {
            return &_matrix[0][0];
        }

        inline numeric_t* raw()
        {
            return &_matrix[0][0];
        }


    public:
        matrix() {  }


        void multiply (const this_t &operand)
        {
            //TODO: Common multiply
        }


        template<typename element_t>
        void copyFrom (const matrix<element_t, sideN> &source)
        {
            for (int i = 0; i < sideN; ++i)
                for (int j = 0; j < sideN; ++j)
                    _matrix[i][j] = source.get (i, j);
        }


        std::string asString() const
        {
            std::stringstream ss;
            ss << '\n';

            for (int i = 0; i < sideN; ++i)
            {
                for (int j = 0; j < sideN; ++j) ss << std::setw (8) << std::fixed << _matrix[i][j] << " \t";
                ss << '\n';
            }

            return ss.str();
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class matrix_4x4 : public matrix<numeric_t, 4>
    {
    public:
        typedef matrix<numeric_t, 4> base_t;

    public:
        matrix_4x4() {  }

        matrix_4x4 (numeric_t d0, numeric_t d1, numeric_t d2, numeric_t d3)
        {
            base_t::_matrix[0][0] = d0;
            base_t::_matrix[1][1] = d1;
            base_t::_matrix[2][2] = d2;
            base_t::_matrix[3][3] = d3;
        }


        matrix_4x4 (numeric_t scalar)
        {
            base_t::_matrix[0][0] = scalar;
            base_t::_matrix[1][1] = scalar;
            base_t::_matrix[2][2] = scalar;
            base_t::_matrix[3][3] = scalar;
        }


        void setCol (int col, numeric_t x1, numeric_t x2, numeric_t x3, numeric_t x4)
        {
            setCol3 (col, x1, x2, x3);
            base_t::_matrix[3][col] = x4;
        }

        void setRow3 (int row, numeric_t x1, numeric_t x2, numeric_t x3)
        {
            base_t::_matrix[row][0] = x1;
            base_t::_matrix[row][1] = x2;
            base_t::_matrix[row][2] = x3;
        }


        void setCol3 (int col, numeric_t x1, numeric_t x2, numeric_t x3)
        {
            base_t::_matrix[0][col] = x1;
            base_t::_matrix[1][col] = x2;
            base_t::_matrix[2][col] = x3;
        }


        void scaleDiag (numeric_t s1, numeric_t s2, numeric_t s3, numeric_t s4)
        {
            scaleDiag3 (s1, s2, s3);
            base_t::_matrix[3][3] *= s4;
        }


        void scaleDiag3 (numeric_t s1, numeric_t s2, numeric_t s3)
        {
            base_t::_matrix[0][0] *= s1;
            base_t::_matrix[1][1] *= s2;
            base_t::_matrix[2][2] *= s3;
        }


        void scaleCol3 (int col, numeric_t s)
        {
            base_t::_matrix[0][col] *= s;
            base_t::_matrix[1][col] *= s;
            base_t::_matrix[2][col] *= s;
        }


        void scaleRow3 (int row, numeric_t s)
        {
            base_t::_matrix[row][0] *= s;
            base_t::_matrix[row][1] *= s;
            base_t::_matrix[row][2] *= s;
        }


        void scaleRow (int row, numeric_t s)
        {
            scaleRow3 (row, s);
            base_t::_matrix[row][3] *= s;
        }


        void copyRow (int row, int toRow, numeric_t scale = 1)
        {
            base_t::_matrix[toRow][0] = base_t::_matrix[row][0] * scale;
            base_t::_matrix[toRow][1] = base_t::_matrix[row][1] * scale;
            base_t::_matrix[toRow][2] = base_t::_matrix[row][2] * scale;
            base_t::_matrix[toRow][3] = base_t::_matrix[row][3] * scale;
        }


        vector3<numeric_t> rowVec3 (int row)
        {
            return vector3<numeric_t> (base_t::_matrix[row][0], base_t::_matrix[row][1], base_t::_matrix[row][2]);
        }


        void fastTransformMultiply (const matrix_4x4<numeric_t> &op)
        {
        #define m   base_t::_matrix
        #define opm op._matrix

            numeric_t  m0, m1, m2, m3;
            for (int row = 0; row <= 2; ++row)
            {
                m0 = m[row][0]; m1 = m[row][1]; m2 = m[row][2]; m3 = m[row][3];

                m[row][0] = m0 * opm[0][0] + m1 * opm[1][0] + m2 * opm[2][0] /*+ m3 * opm[3][0]*/;
                m[row][1] = m0 * opm[0][1] + m1 * opm[1][1] + m2 * opm[2][1] /*+ m3 * opm[3][1]*/;
                m[row][2] = m0 * opm[0][2] + m1 * opm[1][2] + m2 * opm[2][2] /*+ m3 * opm[3][2]*/;
                m[row][3] = m0 * opm[0][3] + m1 * opm[1][3] + m2 * opm[2][3] + m3 /*opm[3][3]*/;
            }

        #undef m
        #undef opm
        }


        void multiply (const matrix_4x4<numeric_t> &op)
        {
        #define m   base_t::_matrix
        #define opm op._matrix

            numeric_t  m0, m1, m2, m3;
            for (int row = 0; row <= 3; ++row)
            {
                m0 = m[row][0]; m1 = m[row][1]; m2 = m[row][2]; m3 = m[row][3];

                m[row][0] = m0 * opm[0][0] + m1 * opm[1][0] + m2 * opm[2][0] + m3 * opm[3][0];
                m[row][1] = m0 * opm[0][1] + m1 * opm[1][1] + m2 * opm[2][1] + m3 * opm[3][1];
                m[row][2] = m0 * opm[0][2] + m1 * opm[1][2] + m2 * opm[2][2] + m3 * opm[3][2];
                m[row][3] = m0 * opm[0][3] + m1 * opm[1][3] + m2 * opm[2][3] + m3 * opm[3][3];
            }

        #undef m
        #undef opm
        }


        template<typename another_numeric_t>
        matrix_4x4<another_numeric_t> convertType() const
        {
            matrix_4x4<another_numeric_t> result;
            result.copyFrom (*this);
            return std::move (result);
        }
    };

//---------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class quaternion
    {
    public:
        typedef quaternion<numeric_t> this_t;

    protected:
        vector3<numeric_t> _im;
        numeric_t          _re;


    public:
        inline numeric_t getX() const  { return _im.getX(); }
        inline numeric_t getY() const  { return _im.getY(); }
        inline numeric_t getZ() const  { return _im.getZ(); }
        inline numeric_t getW() const  { return _re; }

        property_rw  (Re, _re)
        property_get (Im, _im)


    public:
        quaternion() : _im (vector3<numeric_t>::zero()),
                       _re (1)
        { }


        quaternion (const vector3<numeric_t> &im, numeric_t re) : _im (im),
                                                                  _re (re)
        { }


        quaternion (numeric_t x, numeric_t y, numeric_t z, numeric_t w) : _im (x, y, z),
                                                                          _re (w)
        { }


        static this_t ident()
        {
            return this_t (vector3<numeric_t>::zero(), 1);
        }


        bool isIdent() const
        {
            return *this == ident();
        }


        void add (const quaternion<numeric_t> &operand)
        {
            _im += operand._im;
            _re += operand._re;
        }


        void scale (numeric_t operand)
        {
            _im *= operand;
            _re *= operand;
        }


        void divide (numeric_t operand)
        {
            _im /= operand;
            _re /= operand;
        }


        numeric_t squaredLength() const
        {
            return math_ex::square (_im.getX()) +
                   math_ex::square (_im.getY()) +
                   math_ex::square (_im.getZ()) +
                   math_ex::square (_re);
        }


        numeric_t length() const
        {
            return math_ex::squareRoot (squaredLength());
        }


        void normalize()
        {
            numeric_t len = length();
            divide (len);
        }


        this_t normalized() const
        {
            this_t temp (*this);
            temp.normalize();

            return temp;
        }


        void conjugate()
        {
            _im.inverse();
        }


        this_t conjugated() const
        {
            this_t temp (*this);
            temp.conjugate();

            return temp;
        }


        void inverse()
        {
            conjugate();
            divide (squaredLength());
        }


        this_t inversed() const
        {
            this_t temp (*this);
            temp.inverse();

            return temp;
        }


        numeric_t dotProduct (const this_t &operand) const
        {
            return (_im * operand._im) + (_re * _re);
        }


        this_t product (const this_t &operand) const
        {
            return this_t (_im.crossProduct (operand._im) + operand._im * _re + _im * operand._re,
                           _re * operand._re - _im * operand._im);
        }


        void operator+= (const this_t &operand)
        {
            add (operand);
        }


        this_t operator+ (const this_t &operand) const
        {
            this_t temp (*this);
            temp.add (operand);

            return temp;
        }


        void operator*= (numeric_t operand)
        {
            scale (operand);
        }


        this_t operator* (numeric_t operand) const
        {
            this_t temp (*this);
            temp.scale (operand);

            return temp;
        }


        template<typename another_numeric_t>
        quaternion<another_numeric_t> convertType() const
        {
            return quaternion<another_numeric_t> (_im.convertType<another_numeric_t>(), (another_numeric_t) _re);
        }
    };


//---------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t, typename quaternion_t = quaternion<numeric_t>>
    class rotation
    {
    public:
        struct axis_angle
        {
            vector3<numeric_t> axis;
            numeric_t          angle;
        };

    public:
        typedef rotation<numeric_t, quaternion_t> this_t;


    protected:
        quaternion_t _quaternion;
        bool _ident = true;

    public:
        property_get (IsIdent, _ident)


    public:
        rotation() : _quaternion (quaternion_t::ident())
        { }


        rotation (vector3<numeric_t> axis, numeric_t angle) : _ident (false)
        {
            axis.normalize();
            numeric_t half_angle = angle / 2;
            numeric_t sin_a = (numeric_t) std::sin (half_angle);

            _quaternion = quaternion_t (axis * sin_a, (numeric_t) std::cos (half_angle));
        }


        /*rotation (angle<numeric_t>, angle<numeric_t>, angle<numeric_t>) : _ident (false)
        {

        }*/


        rotation (vector3<numeric_t> u, vector3<numeric_t> v)
        {
            numeric_t dot_uv = (u * v);
            numeric_t norm_u_norm_v = sqrt (u.squaredLength() * v.squaredLength());
            numeric_t real_part = norm_u_norm_v + dot_uv;

            vector3<numeric_t> w;
            if (real_part < 1.e-6f * norm_u_norm_v)
            {
                real_part = 0;
                w = std::abs (u.getX()) > std::abs (u.getZ()) ?
                        vector3<numeric_t> (-u.getY(), u.getX(), 0) :
                        vector3<numeric_t> (0, -u.getZ(), u.getY());
            }
            else w = u.crossProduct (v);

            _quaternion = quaternion_t (w, real_part);
            _quaternion.normalize();
        }


        rotation (quaternion_t quaternion_) : _quaternion (quaternion_), _ident (false)
        { }


        axis_angle asAxisAngle()
        {
            if (_quaternion.getRe() > 1) _quaternion.normalize();
            numeric_t angle = 2 * std::acos (_quaternion.getRe());
            numeric_t s = math_ex::squareRoot (1 - math_ex::square (_quaternion.getRe()));

            vector3<numeric_t> axis (1, 0, 0);

            if (s > 0.000001)
            {
                axis = _quaternion.getIm();
                axis.scale (1 / s);
            }

            axis_angle aa;
            aa.angle = angle;
            aa.axis = axis;

            return aa;
        }


        void combine (const rotation &rot)
        {
            if (_ident)
            {
                if (rot._ident) return;
                _quaternion = rot._quaternion;
                _ident = false;
                return;
            }

            _quaternion = rot._quaternion.product (_quaternion);
            _ident = false;
        }


        template<typename another_numeric_t>
        rotation<another_numeric_t> convertType() const
        {
            return rotation<another_numeric_t> (_quaternion.convertType<another_numeric_t>());
        }


        void inverse()
        {
            if (_ident) return;
            _quaternion.inverse();
        }


        this_t inversed() const
        {
            this_t tmp (*this);
            tmp.inverse();

            return std::move (tmp);
        }


        vector3<numeric_t> apply (const vector3<numeric_t> &vec) const
        {
            if (_ident) return vec;

            quaternion_t pseudoQuat (vec, 0);
            auto conjugated = _quaternion.conjugated();
            auto result = _quaternion.product (pseudoQuat);

            return result.product (conjugated).getIm();
        }


        void write2Matrix (matrix_4x4<numeric_t> &transformMatrix) const
        {
            numeric_t wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
            numeric_t s  = 2.0 / _quaternion.length();

            x2 = _quaternion.getX() * s;
            y2 = _quaternion.getY() * s;
            z2 = _quaternion.getZ() * s;

            xx = _quaternion.getX() * x2;
            xy = _quaternion.getX() * y2;
            xz = _quaternion.getX() * z2;

            yy = _quaternion.getY() * y2;
            yz = _quaternion.getY() * z2;
            zz = _quaternion.getZ() * z2;

            wx = _quaternion.getW() * x2;
            wy = _quaternion.getW() * y2;
            wz = _quaternion.getW() * z2;

            transformMatrix.setCol3 (0,     1 - (yy + zz),   xy - wz,         xz + wy      );
            transformMatrix.setCol3 (1,     xy + wz,         1 - (xx + zz),   yz - wx      );
            transformMatrix.setCol3 (2,     xz - wy,         yz + wx,         1 - (xx + yy));
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class projection :
        public oo_extensions::virtual_copyable<projection<numeric_t>>
    {
    protected:
        numeric_t           _aspect       = numeric_t();
        interval<numeric_t> _viewInterval = interval<numeric_t>();

    public:
        property_get (Aspect,       _aspect)
        property_get (ViewInterval, _viewInterval)


    public:
        typedef projection<numeric_t> this_t;
        declare_ptr (this_t)
        projection() { }

        projection (numeric_t aspect, interval<numeric_t> viewInterval) :
                _aspect       (aspect),
                _viewInterval (viewInterval)
        {  }

        virtual void changeAspect (numeric_t aspect)  { _aspect = aspect; }
        virtual void _implicitlyLeftMultiply (matrix_4x4<numeric_t> &wcMatrix) const = 0;

        virtual matrix_4x4<numeric_t> asMatrix() const = 0;
        virtual matrix_4x4<numeric_t> asInverseMatrix() const = 0;
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class perspective_projection : public projection<numeric_t>
    {
        angle<numeric_t> _fovy = angle<numeric_t>();
        numeric_t _a = 0, _b = 0, _c = 0, _d = 0, _e = 0;

    public:
        property_get (Fov, _fovy)

    protected:
        void _recalcCoefficients()
        {
            numeric_t f = 1.0 / std::tan (_fovy.getRad() / 2.0);
            _a = f / base_t::_aspect;
            _b = f;
            _e = -1;

            numeric_t viewLength = base_t::_viewInterval.length();
            _c = -base_t::_viewInterval.borderSumm() / viewLength;
            _d = - (2 * base_t::_viewInterval.getFrom() * base_t::_viewInterval.getTo()) / viewLength;
        }


    public:
        typedef projection<numeric_t> base_t;
        typedef perspective_projection<numeric_t> this_t;

        declare_ptr_alloc (this_t)
        perspective_projection() { }

        perspective_projection (angle<numeric_t> fovy, numeric_t aspect, interval<numeric_t> viewInterval) :
            _fovy         (fovy),
            projection<numeric_t> (aspect, viewInterval)
        {
            _recalcCoefficients();
        }


        virtual void changeAspect (numeric_t aspect)
        {
            base_t::changeAspect (aspect);
            _recalcCoefficients();
        }


        virtual void _implicitlyLeftMultiply (matrix_4x4<numeric_t> &wcMatrix) const
        {
            wcMatrix.scaleRow (0, _a);
            wcMatrix.scaleRow (1, _b);

            wcMatrix.copyRow (2, 3, _e);

            wcMatrix.scaleRow (2, _c);
            wcMatrix.at (2, 3) += _d;
        }


        template<typename another_numeric_t>
        perspective_projection<another_numeric_t> convertType() const
        {
            return perspective_projection<another_numeric_t> (_fovy.convertType<another_numeric_t>(), (another_numeric_t) base_t::_aspect,
                                                              base_t::_viewInterval.convertType<another_numeric_t>());
        }


        virtual projection<numeric_t>* copy() const
        {
            return new perspective_projection<numeric_t> (_fovy, base_t::_aspect, base_t::_viewInterval);
        }


        virtual matrix_4x4<numeric_t> asMatrix() const
        {
            matrix_4x4<numeric_t> mat (_a, _b, _c, 0);
            mat.at (2, 3) = _d;
            mat.at (3, 2) = _e;

            return mat;
        }


        virtual matrix_4x4<numeric_t> asInverseMatrix() const
        {
            numeric_t f = -_c / (_d * _e);
            matrix_4x4<numeric_t> mat (numeric_t (1) / _a, numeric_t (1) / _b, 0, f);
            mat.at (2, 3) = numeric_t (1) / _e;
            mat.at (3, 2) = numeric_t (1) / _d;

            return mat;
        }
    };


//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class orthographic_projection : public projection<numeric_t>
    {
        numeric_t  _height = numeric_t();

    public:
        property_get (Height, _height)


    public:
        typedef projection<numeric_t> base_t;
        typedef orthographic_projection<numeric_t> this_t;
        declare_ptr_alloc (this_t)
        orthographic_projection() { }

        orthographic_projection (numeric_t height, numeric_t aspect, interval<numeric_t> viewInterval) :
                _height               (height),
                projection<numeric_t> (aspect, viewInterval)
        {  }


        virtual void _implicitlyLeftMultiply (matrix_4x4<numeric_t> &wcMatrix) const
        {
            numeric_t w = (base_t::_aspect * _height);
            numeric_t ir = 2.0 / w;
            numeric_t it = 2.0 * (base_t::_aspect / w);

            wcMatrix.scaleRow (0, ir);
            wcMatrix.scaleRow (1, it);

            numeric_t viewLength = base_t::_viewInterval.length();
            wcMatrix.scaleRow (2, -2.0 / viewLength);
            wcMatrix.at (2, 3) -= base_t::_viewInterval.borderSumm() / viewLength;
        }


        virtual projection<numeric_t>* copy() const
        {
            return new orthographic_projection<numeric_t> (_height, base_t::_aspect, base_t::_viewInterval);
        }


        virtual matrix_4x4<numeric_t> asMatrix() const
        {
            return matrix_4x4<numeric_t>();
        }


        virtual matrix_4x4<numeric_t> asInverseMatrix() const
        {
            return matrix_4x4<numeric_t>();
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class transform
    {
    public:
        typedef transform<numeric_t> this_t;

    protected:
        vector3<numeric_t>  _translation  = vector3<numeric_t>::zero();
        rotation<numeric_t> _rotation     = rotation<numeric_t>();
        vector3<numeric_t>  _scale        = vector3<numeric_t>::ident();

        bool _identScale = true;
        bool _inversed = false;

        mutable matrix_4x4<numeric_t> _cachedMatrix;
        mutable bool _matrixCalculated = false;

    public:
        property_get_ref (Translation, _translation)
        property_get_ref (Rotation,    _rotation)
        property_get_ref (Scale,       _scale)


    private:
        void _calculateMatrix() const
        {
            if (!_inversed)
            {
                _cachedMatrix.setCol (3, _translation.getX(), _translation.getY(), _translation.getZ(), 1.0);
                _rotation.write2Matrix (_cachedMatrix);

                if (!_identScale)
                {
                    _cachedMatrix.scaleCol3 (0, _scale.getX());
                    _cachedMatrix.scaleCol3 (1, _scale.getY());
                    _cachedMatrix.scaleCol3 (2, _scale.getZ());
                }
            }

            else
            {
                _rotation.write2Matrix (_cachedMatrix);

                _cachedMatrix.at (0, 3) = _cachedMatrix.rowVec3 (0) * _translation;
                _cachedMatrix.at (1, 3) = _cachedMatrix.rowVec3 (1) * _translation;
                _cachedMatrix.at (2, 3) = _cachedMatrix.rowVec3 (2) * _translation;
                _cachedMatrix.at (3, 3) = 1;

                if (!_identScale) //TODO: Strange issue with ident scale
                {
                    //debug::log::println (mkstr (_scale.getX(), " ", _scale.getY(), " ", _scale.getZ()));
                    _cachedMatrix.scaleCol3 (0, _scale.getX());
                    _cachedMatrix.scaleCol3 (1, _scale.getY());
                    _cachedMatrix.scaleCol3 (2, _scale.getZ());
                }
            }

            _matrixCalculated = true;
        }


    public:
        transform()
        {
        }


        transform (vector3<numeric_t> translation, rotation<numeric_t> rot,
                   vector3<numeric_t> scale = vector3<numeric_t>::ident()) : _translation (translation),
                                                                             _rotation    (rot),
                                                                             _scale       (scale),
                                                                             _identScale  (scale == vector3<numeric_t>::ident())
        {  }


        static this_t ident()
        {
            return this_t();
        }


        const matrix_4x4<numeric_t>& asMatrix() const
        {
            if (!_matrixCalculated)  _calculateMatrix();
            return _cachedMatrix;
        }


        void inverse()
        {
            _inversed = !_inversed;

            _translation.inverse();
            _rotation.inverse();
            _scale.inverseScale();

            _identScale = _scale == vector3<numeric_t>::ident();
            _matrixCalculated = false;
        }


        this_t inversed() const
        {
            this_t tmp (*this);
            tmp.inverse();

            return tmp;
        }


        template<typename another_numeric_t>
        transform<another_numeric_t> convertType() const
        {
            return transform<another_numeric_t> (_translation.convertType<another_numeric_t>(),
                                                 _rotation.convertType<another_numeric_t>(),
                                                 _scale.convertType<another_numeric_t>());
        }


        void add (const this_t& theOtherTransform)
        {
            _translation += theOtherTransform._translation;
            _rotation.combine (theOtherTransform._rotation);

            if (!theOtherTransform._identScale)
            {
                _scale = vector3<numeric_t> (_scale.getX() * theOtherTransform._scale.getX(),
                                             _scale.getY() * theOtherTransform._scale.getY(),
                                             _scale.getZ() * theOtherTransform._scale.getZ());
            }

            _matrixCalculated = false;
        }


        void operator+= (const this_t& theOtherTransform)
        {
            add (theOtherTransform);
        }


        void translate (const vector3<numeric_t> &deltaPos)
        {
            _translation += deltaPos;
            _matrixCalculated = false;
        }


        void rotate (const rotation<numeric_t> &deltaRot)
        {
            _rotation.combine (deltaRot);
            _matrixCalculated = false;
        }


        void changeRotation (const rotation<numeric_t> &rot)
        {
            _rotation = rot;
            _matrixCalculated = false;
        }


        void changeTranslation (const vector3<numeric_t> &pos)
        {
            _translation = pos;
            _matrixCalculated = false;
        }


        void scale (const vector3<numeric_t> &scaleVec)
        {
            _scale.setX (_scale.getX() * scaleVec.getX());
            _scale.setY (_scale.getY() * scaleVec.getY());
            _scale.setZ (_scale.getZ() * scaleVec.getZ());

            _identScale = false;
            _matrixCalculated = false;
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class object2screen_transform
    {
        transform<numeric_t>   _worldTransform  = transform<numeric_t>();
        transform<numeric_t>   _cameraInverseTransform = transform<numeric_t>();
        projection<numeric_t>  *_projection;                                 // not owned here

        matrix_4x4<numeric_t> _cachedWCMatrix;
        matrix_4x4<numeric_t> _cachedMatrix;


    public:
        property_get_ref (WorldTransform,  _worldTransform)
        property_get_ref (CameraInverseTransform, _cameraInverseTransform)
        property_get     (Projection,      _projection)
        property_get_ref (WorldCamTransformMatrix, _cachedWCMatrix)

        const matrix_4x4<numeric_t> getWorldTransformMatrix() const  { return _worldTransform.asMatrix(); }


    public:
        object2screen_transform()  { }

        object2screen_transform (transform<numeric_t> worldTransform,
                                 transform<numeric_t> cameraInverseTransform,
                                 projection<numeric_t> *projection) : _worldTransform  (worldTransform),
                                                                      _cameraInverseTransform (cameraInverseTransform),
                                                                      _projection      (projection)
        {
            _cachedWCMatrix = _cameraInverseTransform.asMatrix();
            _cachedWCMatrix.fastTransformMultiply (getWorldTransformMatrix());

            _cachedMatrix = _cachedWCMatrix;
            _projection->_implicitlyLeftMultiply (_cachedMatrix);
        }


        const matrix_4x4<numeric_t>& asMatrix() const
        {
            return _cachedMatrix;
        }


        object2screen_transform<numeric_t> withChangedCameraTransform (const transform<numeric_t> &newCameraTransform)
        {
            return object2screen_transform<numeric_t> (_worldTransform, newCameraTransform, _projection);
        }


        object2screen_transform<numeric_t> withChangedProjection (unique_ptr<projection<numeric_t>> &&projection)
        {
            object2screen_transform<numeric_t> result;
            result._cameraInverseTransform = _cameraInverseTransform;
            result._projection = projection;

            result._cachedMatrix = result._cachedWCMatrix = _cachedWCMatrix;
            result._projection->_implicitlyLeftMultiply (result._cachedMatrix);

            return result;
        }
    };


//----------------------------------------------------------------------------------------------------------------------

    typedef angle<float>       angle_f;
    typedef interval<float>    interval_f;
    typedef vector3<float>     vector3_f;
    typedef vector2<float>     vector2_f;
    typedef rotation<float>    rotation_f;
    typedef transform<float>   transform_f;
    typedef matrix_4x4<float>  matrix_4x4_f;
    typedef projection<float>  projection_f;
    typedef perspective_projection<float> perspective_projection_f;
    typedef orthographic_projection<float> orthographic_projection_f;
    typedef object2screen_transform<float> object2screen_transform_f;

    typedef angle<double>      angle_d;
    typedef interval<double>   interval_d;
    typedef vector3<double>    vector3_d;
    typedef vector2<double>    vector2_d;
    typedef rotation<double>   rotation_d;
    typedef transform<double>  transform_d;
    typedef matrix_4x4<double> matrix_4x4_d;
    typedef projection<double> projection_d;
    typedef perspective_projection<double>  perspective_projection_d;
    typedef orthographic_projection<double> orthographic_projection_d;
    typedef object2screen_transform<double> object2screen_transform_d;
}

//----------------------------------------------------------------------------------------------------------------------

#endif
