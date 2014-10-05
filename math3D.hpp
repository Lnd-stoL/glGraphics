
#ifndef __math3D__included__
#define __math3D__included__

//----------------------------------------------------------------------------------------------------------------------

#include <stdexcept>
#include <iomanip>

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
        angle(numeric_t angleNum) : _angleRad (angleNum) {  }

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
            return std::move (temp);
        }


        void intersect (const this_t &operand)
        {
            _begin = std::max (_begin, operand._begin);
            _end   = std::min (_end, operand._end);

            if (_end < _begin) _end = _begin;
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
        vector3<another_numeric_t> withAnotherNumericType()
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
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t, int sideN>
    class matrix : public oo_extensions::i_as_string
    {
    public:
        typedef matrix<numeric_t, sideN> this_t;

    protected:
        numeric_t _matrix[sideN][sideN] = {};

    public:
        property_get (Side, sideN)


    protected:
        inline void _testBounds (int row, int col)
        {
            if (row < 0 || col < 0 || row >= sideN || col >= sideN)
                throw std::range_error (mkstr (" invalid matrix indexing: ", col, " ", row, " are out of 0 ", sideN));
        }

    public:
        inline numeric_t get(int row, int col)
        {
            _testBounds (row, col);
            return _matrix[row][col];
        }

        inline void set(int row, int col, numeric_t value)
        {
            _testBounds (row, col);
            _matrix[row, col] = value;
        }

        inline numeric_t& at(int row, int col)
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

            return std::move (temp);
        }


        void conjugate()
        {
            _im.inverse();
        }


        this_t conjugated() const
        {
            this_t temp (*this);
            temp.conjugate();

            return std::move (temp);
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

            return std::move (temp);
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

            return std::move (temp);
        }


        void operator*= (numeric_t operand)
        {
            scale (operand);
        }


        this_t operator* (numeric_t operand) const
        {
            this_t temp (*this);
            temp.scale (operand);

            return std::move (temp);
        }


        template<typename another_numeric_t>
        quaternion<another_numeric_t> withAnotherNumericType()
        {
            return quaternion<another_numeric_t> (_im.withAnotherNumericType<another_numeric_t>(), (another_numeric_t) _re);
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


    public:
        rotation() : _quaternion (quaternion_t::ident())
        { }


        rotation (vector3<numeric_t> axis, numeric_t angle)
        {
            axis.normalize();
            numeric_t half_angle = angle / 2;
            numeric_t sin_a = (numeric_t) std::sin (half_angle);

            _quaternion = quaternion_t (axis.getX() * sin_a, axis.getY() * sin_a, axis.getZ() * sin_a,
                                        (numeric_t) std::cos (half_angle));
        }


        rotation (quaternion_t quaternion_) : _quaternion (quaternion_)
        { }


        axis_angle asAxisAngle()
        {
            if (_quaternion.getRe() > 1) _quaternion.normalize();
            numeric_t angle = 2 * std::acos (_quaternion.getRe());
            numeric_t s = math_ex::squareRoot (1 - math_ex::square (_quaternion.getRe()));

            vector3<numeric_t> axis (1, 0, 0);

            if (s > 0.001)
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
            _quaternion = rot._quaternion.product (_quaternion);
        }


        template<typename another_numeric_t>
        rotation<another_numeric_t> withAnotherNumericType()
        {
            return rotation<another_numeric_t> (_quaternion.withAnotherNumericType<another_numeric_t>());
        }


        void inverse()
        {
            _quaternion.inverse();
        }


        this_t inversed()
        {
            this_t tmp (*this);
            tmp.inverse();

            return std::move (tmp);
        }


        vector3<numeric_t> apply (const vector3<numeric_t> vec)
        {
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
    class perspective_projection
    {
        angle<numeric_t>    _fovy         = angle<numeric_t>();
        numeric_t           _aspect       = numeric_t();
        interval<numeric_t> _viewInterval = interval<numeric_t>();

    public:
        property_get (Fov,          _fovy)
        property_get (Aspect,       _aspect)
        property_get (ViewInterval, _viewInterval)


    public:
        perspective_projection() { }

        perspective_projection (angle<numeric_t> fovy, numeric_t aspect, interval<numeric_t> viewInterval) :
            _fovy         (fovy),
            _aspect       (aspect),
            _viewInterval (viewInterval)
        {  }


        matrix_4x4<numeric_t> asMatrix() const
        {

            matrix_4x4<numeric_t> projectionMatrix();
            return projectionMatrix;
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


    public:
        property_get (Translation, _translation)
        property_get (Rotation,    _rotation)
        property_get (Scale,       _scale)


    public:
        transform() {  }


        transform (vector3<numeric_t> translation, rotation<numeric_t> rot,
                   vector3<numeric_t> scale = vector3<numeric_t>::ident()) : _translation (translation),
                                                                             _rotation    (rot),
                                                                             _scale       (scale)
        {  }


        matrix_4x4<numeric_t> asMatrix() const
        {
            if (!_inversed)
            {
                matrix_4x4<numeric_t> transformMatrix;
                transformMatrix.setCol (3, _translation.getX(), _translation.getY(), _translation.getZ(), 1.0);
                _rotation.write2Matrix (transformMatrix);

                if (!_identScale)
                {
                    transformMatrix.scaleRow3 (0, _scale.getX());
                    transformMatrix.scaleRow3 (1, _scale.getY());
                    transformMatrix.scaleRow3 (2, _scale.getZ());
                }

                return std::move (transformMatrix);
            }

            else
            {
                matrix_4x4<numeric_t> transformMatrix;
                _rotation.write2Matrix (transformMatrix);

                transformMatrix.at (0, 3) = transformMatrix.rowVec3 (0) * _translation;
                transformMatrix.at (1, 3) = transformMatrix.rowVec3 (1) * _translation;
                transformMatrix.at (2, 3) = transformMatrix.rowVec3 (2) * _translation;
                transformMatrix.at (3, 3) = 1;

                return std::move (transformMatrix);
            }
        }


        void inverse()
        {
            _inversed = !_inversed;

            _translation.inverse();
            _rotation.inverse();
            _scale.inverse();
        }


        this_t inversed()
        {
            this_t tmp (*this);
            tmp.inverse();

            return std::move (tmp);
        }


        template<typename another_numeric_t>
        transform<another_numeric_t> withAnotherNumericType()
        {
            return transform<another_numeric_t> (_translation.withAnotherNumericType<another_numeric_t>(),
                                                 _rotation.withAnotherNumericType<another_numeric_t>(),
                                                 _scale.withAnotherNumericType<another_numeric_t>());
        }
    };

//----------------------------------------------------------------------------------------------------------------------

    template<typename numeric_t>
    class object2screen_transform
    {
        transform<numeric_t> _worldTransform  = transform<numeric_t>();
        transform<numeric_t> _cameraTransform = transform<numeric_t>();
        perspective_projection<numeric_t> _projection = perspective_projection<numeric_t>();

        matrix_4x4<numeric_t> _cachedWorldMatrix;
        matrix_4x4<numeric_t> _cachedWCMatrix;
        matrix_4x4<numeric_t> _cachedMatrix;


    public:
        property_get_ref (WorldTransform,  _worldTransform)
        property_get_ref (CameraTransform, _cameraTransform.inversed())
        property_get_ref (Projection,      _projection)

        property_get_ref (WorldTransformMatrix,    _cachedWorldMatrix)
        property_get_ref (WorldCamTransformMatrix, _cachedWCMatrix)


    protected:
        void _implicitlyMultiplyByProjectionMatrix (matrix_4x4<numeric_t> &wcMatrix)
        {
            numeric_t f = 1.0 / std::tan (_projection.getFov() / 2);
            wcMatrix.scaleRow (0, f / _projection.getAspect());
            wcMatrix.scaleRow (1, f);

            wcMatrix.copyRow (2, 3, -1);

            numeric_t viewLength = _projection.getViewInterval().length();
            wcMatrix.scaleRow (2, -_projection.getViewInterval().borderSumm() / viewLength);
            wcMatrix.at (2, 3) -=
                (2 * _projection.getViewInterval().getFrom() * _projection.getViewInterval().getTo()) / viewLength;
        }


    public:
        object2screen_transform (transform<numeric_t> worldTransform,
                                 transform<numeric_t> cameraTransform,
                                 perspective_projection<numeric_t> projection) : _worldTransform  (worldTransform),
                                                                                 _cameraTransform (cameraTransform),
                                                                                 _projection      (projection)
        {
            _cameraTransform.inverse();

            _cachedWorldMatrix = _worldTransform.asMatrix();
            _cachedWCMatrix = _cameraTransform.asMatrix();
            _cachedWCMatrix.fastTransformMultiply (_cachedWorldMatrix);

            _cachedMatrix = _cachedWCMatrix;
            _implicitlyMultiplyByProjectionMatrix (_cachedMatrix);
        }


        const matrix_4x4<numeric_t>& asMatrix() const
        {
            return _cachedMatrix;
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
    typedef perspective_projection<float> perspective_projection_f;
    typedef object2screen_transform<float> object2screen_transform_f;

    typedef angle<double>      angle_d;
    typedef interval<double>   interval_d;
    typedef vector3<double>    vector3_d;
    typedef vector2<double>    vector2_d;
    typedef rotation<double>   rotation_d;
    typedef transform<double>  transform_d;
    typedef matrix_4x4<double> matrix_4x4_d;
    typedef perspective_projection<float> perspective_projection_d;
    typedef object2screen_transform<double> object2screen_transform_d;
}

//----------------------------------------------------------------------------------------------------------------------

#endif