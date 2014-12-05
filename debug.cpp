
#include "debug.hpp"

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>

using oo_extensions::mkstr;
using oo_extensions::mkfstr;
using namespace std::chrono;

//----------------------------------------------------------------------------------------------------------------------

namespace debug
{
    time_point<system_clock>  log::_programStartClock = system_clock::now();

//----------------------------------------------------------------------------------------------------------------------

    /*static*/ void log::print (const std::string &str)
    {
        std::cout << str;
    }


    /*static*/ void log::nextln()
    {
        print ("\n");
    }


    /*static*/ void log::println (const std::string &str)
    {
        time_point<system_clock> nowClock = system_clock::now();
        duration<double> elapsedSeconds = nowClock - _programStartClock;

        //std::ostringstream timeLabel;
        auto timeLabel = mkfstr ("[", std::setw (7), std::setfill ('x'), std::fixed, std::setprecision (0),
               elapsedSeconds.count() * 1000.0, " ms]: ");
        print (mkstr (timeLabel, str));

        nextln();
    }


    /*static*/ void log::println_gl (const std::string &str)
    {
        println (mkstr ("GL: ", str));
    }


    /*static*/ void log::println_err (const std::string &str)
    {
        println (mkstr ("\033[1;31m", str, "\033[0m"));
    }


    /*static*/ void log::println_mrk (const std::string &str)
    {
        println (mkstr ("\033[1;32m", str, "\033[0m"));
    }

//----------------------------------------------------------------------------------------------------------------------

    /*static*/ void gl::printGLError (GLenum errorCode)
    {
        if (errorCode == GL_NO_ERROR) return;

        std::string errorString = "";
        if (errorCode == GL_INVALID_ENUM)
            errorString = "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";

        else if (errorCode == GL_INVALID_VALUE)
            errorString = "GL_INVALID_VALUE: A numeric argument is out of range.";

        else if (errorCode == GL_INVALID_OPERATION)
            errorString = "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";

        else if (errorCode == GL_INVALID_FRAMEBUFFER_OPERATION)
            errorString = "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";

        else if (errorCode == GL_OUT_OF_MEMORY)
            errorString = "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";

        else errorString = "Unknown internal error.";


        log::println_err (mkstr ("OpenGL reported an error: (", (unsigned) errorCode, ") ", errorString));
    }
}
