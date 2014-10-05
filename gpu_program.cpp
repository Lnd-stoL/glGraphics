
#include "gpu_program.hpp"
#include "debug.hpp"

#include <GL/glew.h>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <algorithm>

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    shader::shader (GLint shaderType)
    {
        _initializeGLShader (shaderType);
    }


    shader::shader (const std::string &fileName, GLint shaderType) : shader (shaderType)
    {
        compileFromFile (fileName);
    }


    void shader::_initializeGLShader (GLenum shaderType)
    {
        _shaderId = glCreateShader (shaderType);

        if (!debug::gl::test()) return;
        debug::log::println_gl (mkstr ("shader created with id ", _shaderId));
    }


    void shader::compileFromFile (const std::string &fileName)
    {
        debug::log::println (mkstr ("loading shader source from '", fileName, "'"));
        std::ifstream sourceFile (fileName);
        if (!sourceFile.good())
        {
            debug::log::println_err ("unable to open specified file");
            return;
        }

        std::string sourceFileContent ((std::istreambuf_iterator<char>(sourceFile)), std::istreambuf_iterator<char>());
        compile (sourceFileContent);
    }


    void shader::compile (const std::string &codeString)
    {
        if (_compiled) return;

        debug::log::println ("compiling shader ...");
        auto strPtr = codeString.c_str();
        glShaderSource (_shaderId, 1, &strPtr, nullptr);
        glCompileShader (_shaderId);

        GLint result = GL_FALSE;
        int infoLogLength = int();
        glGetShaderiv (_shaderId, GL_COMPILE_STATUS,  &result);

        if (result != GL_TRUE)
        {
            glGetShaderiv (_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
            char *glInfoLog = new char[infoLogLength + 1];
            glGetShaderInfoLog (_shaderId, infoLogLength, nullptr, glInfoLog);

            debug::log::println_gl (glInfoLog);
            delete[] glInfoLog;

            debug::log::println_err ("compilation failed");
            return;
        }

        _compiled = true;
        debug::log::println_mrk (mkstr (asString(), " successfully compiled and is ready for use"));
    }


    shader::~shader()
    {
        if (_shaderId && _shaderId != GL_INVALID_INDEX)
        {
            glDeleteShader (_shaderId);
            debug::gl::test();
            debug::log::println_gl (mkstr (asString(), " released"));
        }

        _shaderId = GL_INVALID_INDEX;
    }


    /*virtual*/ std::string shader::asString() const
    {
        return mkstr ("shader [with id ", _shaderId, "; ", _compiled ? "compiled" : "empty", "]");
    }

//----------------------------------------------------------------------------------------------------------------------

    vertex_shader::vertex_shader() : shader (GL_VERTEX_SHADER)
    {  }


    vertex_shader::vertex_shader (const std::string &fileName) : shader (fileName, GL_VERTEX_SHADER)
    {  }

//----------------------------------------------------------------------------------------------------------------------

    fragment_shader::fragment_shader() : shader (GL_FRAGMENT_SHADER)
    {  }


    fragment_shader::fragment_shader (const std::string &fileName) : shader (fileName, GL_FRAGMENT_SHADER)
    {  }

//----------------------------------------------------------------------------------------------------------------------

    gpu_program::gpu_program (shared_ptr<i_vertex_layout> vertexLayout) : _vertexLayout (vertexLayout)
    {
        _initializeGLProgram();
    }


    gpu_program::gpu_program (shared_ptr<i_vertex_layout> vertexLayout,
                              const vertex_shader &vshader,
                              const fragment_shader &fshader) : gpu_program (vertexLayout)
    {
        _attachAndLink (vshader, fshader);
    }


    gpu_program::gpu_program (shared_ptr<i_vertex_layout> vertexLayout,
                              const std::string &vertShaderFileName,
                              const std::string &fragShaderFileName) : gpu_program (vertexLayout)
    {
        vertex_shader   vshader (vertShaderFileName);
        fragment_shader fshader (fragShaderFileName);

        _attachAndLink (vshader, fshader);
    }


    void gpu_program::_attachAndLink (const vertex_shader &vshader, const fragment_shader &fshader)
    {
        attach (vshader);
        attach (fshader);
        link();
    }


    void gpu_program::_bindVertexAttributes()
    {
        auto attributes = _vertexLayout->getAttributes();
        for (unsigned i = 0; i < attributes.size(); ++i)
        {
            glBindAttribLocation (_programId, i, attributes[i].name.c_str());
            if (!debug::gl::test()) return;

            glEnableVertexAttribArray (i);
        }
    }


    void gpu_program::attach (const shader& attachement)
    {
        if (!attachement.getCompiled())
        {
            debug::log::println_err (
                mkstr ("can't attach ", attachement.asString(), " to ", asString(), " until it is compiled."));

            return;
        }

        glAttachShader (_programId, attachement.getGlId());
        if (!debug::gl::test()) return;
        debug::log::println (mkstr (attachement.asString(), " successfully attached to ", asString()));
    }


    void gpu_program::link()
    {
        if (_linked) return;
        _bindVertexAttributes();

        glLinkProgram (_programId);
        GLint result = GL_FALSE;
        glGetProgramiv (_programId, GL_LINK_STATUS, &result);

        if (result != GL_TRUE)
        {
            int infoLogLength = int();
            glGetProgramiv (_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
            char *glInfoLog = new char[infoLogLength + 1];
            glGetProgramInfoLog (_programId, infoLogLength, nullptr, glInfoLog);

            debug::log::println_gl (glInfoLog);
            delete[] glInfoLog;

            debug::log::println_err ("compilation failed");
            return;
        }

        if (!debug::gl::test()) return;
        _linked = true;
        debug::log::println_mrk (mkstr (asString(), " successfully linked and is ready to use"));
    }


    void gpu_program::_initializeGLProgram()
    {
        _programId = glCreateProgram();

        if (!debug::gl::test()) return;
        debug::log::println_gl (mkstr ("gpu program created with id ", _programId));
    }


    bool gpu_program::_testValid() const
    {
        if (!_linked)
        {
            debug::log::println_err (mkstr ("unable to use ", asString(), " until it is linked"));
            return false;
        }

        return true;
    }


    void gpu_program::use() const
    {
        if (!_testValid()) return;
        _bind();

        auto attributes = _vertexLayout->getAttributes();
        for (unsigned i = 0; i < attributes.size(); ++i)
        {
            glVertexAttribPointer (i, attributes[i].dimension, attributes[i].type, attributes[i].normalized,
                                   _vertexLayout->getStride(), (GLvoid *) attributes[i].offset);
        }
    }


    void gpu_program::_bind() const
    {
        glUseProgram (_programId);
        if (!debug::gl::test()) return;
    }


    GLuint gpu_program::_locateUniform (const string &name) const
    {
        GLint location = glGetUniformLocation (_programId, name.c_str());
        if (location < 0)
        {
            debug::log::println_err (mkstr ("can't find uniform named '", name, "' in ", asString()));
            return GL_INVALID_INDEX;
        }

        return static_cast<GLuint> (location);
    }


    void gpu_program::setUniform (const std::string &name, const math3D::matrix_4x4<float> &value)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name);
        if (location == GL_INVALID_INDEX) return;

        glUniformMatrix4fv (location, 1, GL_TRUE, value.raw());
        debug::gl::test();
    }


    void gpu_program::setUniform (const std::string &name, const math3D::vector3_f &value)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name);
        if (location == GL_INVALID_INDEX) return;

        glUniform3f (location, value.getX(), value.getY(), value.getZ());
        debug::gl::test();
    }


    void gpu_program::setUniformSampler (const std::string &name, unsigned textureIndex)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name);
        if (location == GL_INVALID_INDEX) return;

        glUniform1i (location, textureIndex);
        debug::gl::test();
    }


    gpu_program::~gpu_program()
    {
        if (_programId && _programId != GL_INVALID_VALUE)
        {
            glDeleteProgram (_programId);
            debug::log::println_gl (mkstr (asString(), " released"));
        }

        _programId = GL_INVALID_VALUE;
    }


    /*virtual*/ std::string gpu_program::asString() const
    {
        return mkstr ("gpu_program [with id ", _programId, "; ", _linked ? "linked" : "raw", "]");
    }
}