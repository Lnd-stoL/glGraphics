
#ifndef __debug__included__
#define __debug__included__

//----------------------------------------------------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <exception>
#include <GL/glew.h>

#include "oo_extensions.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace debug
{
    class log
    {
        static std::chrono::time_point<std::chrono::system_clock> _programStartClock;


    public:
        static void print (const std::string &str);
        static void nextln();

        static void println    (const std::string &str);
        static void println_gl (const std::string &str);

        static void println_err (const std::string &str);
        static void println_mrk (const std::string &str);
    };


    class gl_exception : public std::exception
    {
        GLenum _glErrorCode = GL_NO_ERROR;

    public:
        property_get (GLErrorCode, _glErrorCode)

    public:
        gl_exception (GLenum glErrorCode, const std::string &description = "") : _glErrorCode (glErrorCode)
        {  }

        virtual const char* what() const noexcept { return "OpenGL reported a error."; }
    };


    class gl
    {
    public:
        static inline bool ok()
        {
            return glGetError() == GL_NO_ERROR;
        }

        static inline bool test()
        {
            if (ok()) return true;
            printGLError (glGetError());
            return false;
        }

        static inline void testCritical()
        {
            if (ok()) return;
            throw gl_exception (glGetError());
        }


    public:
        static void printGLError (GLenum errorCode);
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
