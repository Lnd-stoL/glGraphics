
#include "gpu_program.hpp"

#include "render_resources.hpp"
#include "resource_manager_impl.hpp"

using oo_extensions::mkstr;

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    shader::shader (GLenum shaderType)
    {
        _initializeGLShader (shaderType);
    }


    shader::shader (const std::string &fileName, GLenum shaderType) : shader (shaderType)
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
        debug::gl::test();

        GLint result = 0;
        int infoLogLength = int();
        glGetShaderiv (_shaderId, GL_COMPILE_STATUS,  &result);

        glGetShaderiv (_shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *glInfoLog = new char[infoLogLength + 1];
        glGetShaderInfoLog (_shaderId, infoLogLength, nullptr, glInfoLog);

        if (infoLogLength > 1)  debug::log::println_gl (glInfoLog);
        delete[] glInfoLog;

        if (result != 1)
        {
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

    vertex_shader::vertex_shader()
        : shader (GL_VERTEX_SHADER)
    {
    }


    vertex_shader::vertex_shader (const std::string &fileName)
        : shader (fileName, GL_VERTEX_SHADER)
    {
        resource::_loaded();
    }

//----------------------------------------------------------------------------------------------------------------------

    fragment_shader::fragment_shader()
        : shader (GL_FRAGMENT_SHADER)
    {
    }


    fragment_shader::fragment_shader (const std::string &fileName)
        : shader (fileName, GL_FRAGMENT_SHADER)
    {
        resource::_loaded();
    }

//----------------------------------------------------------------------------------------------------------------------

    geometry_shader::geometry_shader()
        : shader (GL_GEOMETRY_SHADER)
    {
    }


    geometry_shader::geometry_shader (const std::string &fileName)
        : shader (fileName, GL_GEOMETRY_SHADER)
    {
        resource::_loaded();
    }

//----------------------------------------------------------------------------------------------------------------------

    gl_bindable_impl (gpu_program)

//----------------------------------------------------------------------------------------------------------------------

    /*virtual*/ string gpu_program::id::hashString() const
    {
        unsigned totalLength = 20;  // average length of vertex layout id
        for (auto& nextStr : _shaderFileNames)  totalLength += nextStr.size() + 1;

        string hashStr;
        hashStr.reserve (totalLength);
        for (auto& nextShaderFileName : _shaderFileNames)
        {
            hashStr.append (nextShaderFileName);
            hashStr.append ("*");
        }

        hashStr.append (_vertexLayout->hashString());
        return hashStr;
    }


    gpu_program::id::id (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName, const string &fragShaderFileName)
        : gpu_program::id::id (vertexLayout, { vertShaderFileName, fragShaderFileName })
    {

    }


    gpu_program::id::id (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName, const string &geomShaderFileName,
                         const string &fragShaderFileName)
        : gpu_program::id::id (vertexLayout, { vertShaderFileName, geomShaderFileName, fragShaderFileName })
    {

    }


    gpu_program::id::id (i_vertex_layout::ptr vertexLayout, vector<string> &&shaderFileNames)
        : _vertexLayout (vertexLayout)
    {
        _shaderFileNames.swap (shaderFileNames);
    }

//----------------------------------------------------------------------------------------------------------------------

    gpu_program::gpu_program (i_vertex_layout::ptr vertexLayout) : _vertexLayout (vertexLayout)
    {
        _initializeGLProgram();
    }


    void gpu_program::_bindVertexAttributes()
    {
        auto attributes = _vertexLayout->attributes();
        for (unsigned i = 0; i < attributes.size(); ++i)
        {
            glEnableVertexAttribArray (i);

            glBindAttribLocation (_programId, i, attributes[i].name.c_str());
            if (!debug::gl::test()) return;
        }
    }


    void gpu_program::attach (shader::ptr attachement)
    {
        resource::_changedAfterLoading();

        if (!attachement->isCompiled())
        {
            debug::log::println_err (
                mkstr ("can't attach ", attachement->asString(), " to ", asString(), " until it is compiled."));

            return;
        }

        glAttachShader (_programId, attachement->glId());
        if (!debug::gl::test()) return;
        debug::log::println (mkstr (attachement->asString(), " successfully attached to ", asString()));
    }


    void gpu_program::link()
    {
        if (_linked) return;
        _bindVertexAttributes();

        glLinkProgram (_programId);
        debug::gl::test();

        GLint result = 0;
        glGetProgramiv (_programId, GL_LINK_STATUS, &result);

        int infoLogLength = int();
        glGetProgramiv (_programId, GL_INFO_LOG_LENGTH, &infoLogLength);
        char *glInfoLog = new char[infoLogLength + 1];
        glGetProgramInfoLog (_programId, infoLogLength, nullptr, glInfoLog);

        if (infoLogLength > 1)  debug::log::println_gl (glInfoLog);
        delete[] glInfoLog;

        if (result != 1)
        {
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

        if (!gl_bindable<gpu_program>::isBoundNow())
        {
            _bind();
            gl_bindable<gpu_program>::_bindThis();
        }

        for (unsigned i = 0; i < 10; ++i)
        {
            glDisableVertexAttribArray (i);
        }

        auto attributes = _vertexLayout->attributes();
        for (unsigned i = 0; i < attributes.size(); ++i)
        {
            glEnableVertexAttribArray (i);
            glVertexAttribPointer (i, attributes[i].dimension,
                                           (GLenum) attributes[i].type,
                                           (GLboolean) attributes[i].normalized,
                                           (GLsizei) _vertexLayout->strideInBytes(),
                                           (GLvoid *) attributes[i].offset);
        }
    }


    void gpu_program::_bind() const
    {
        glUseProgram (_programId);
    }


    GLuint gpu_program::_locateUniform (const string &name, bool ignoreIfNotExists) const
    {
        GLint location = glGetUniformLocation (_programId, name.c_str());                        // TODO: Inifficient
        if (location < 0 && !ignoreIfNotExists)
        {
            debug::log::println_err (mkstr ("can't find uniform named '", name, "' in ", asString()));
            return GL_INVALID_INDEX;
        }

        return static_cast<GLuint> (location);
    }


    void gpu_program::setUniform (const std::string &name, const math3d::matrix_4x4<float> &value, bool ignoreIfNotExists)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name, ignoreIfNotExists);
        if (location == GL_INVALID_INDEX) return;

        glUniformMatrix4fv (location, 1, GL_TRUE, value.raw());
    }



    void gpu_program::setUniform (const std::string &name, float value, bool ignoreIfNotExists)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name, ignoreIfNotExists);
        if (location == GL_INVALID_INDEX) return;

        glUniform1f (location, value);
    }


    void gpu_program::setUniform (const std::string &name, const math3d::vector3_f &value, bool ignoreIfNotExists)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name, ignoreIfNotExists);
        if (location == GL_INVALID_INDEX) return;

        glUniform3f (location, value.x(), value.y(), value.z());
    }


    void gpu_program::setUniform (const std::string &name, const math3d::vector2_f &value, bool ignoreIfNotExists)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name, ignoreIfNotExists);
        if (location == GL_INVALID_INDEX) return;

        glUniform2f (location, value.x(), value.y());
    }


    void gpu_program::setUniformSampler (const std::string &name, unsigned textureIndex, bool ignoreIfNotExists)
    {
        if (!_testValid()) return;
        _bind();
        GLuint location = _locateUniform (name, ignoreIfNotExists);
        if (location == GL_INVALID_INDEX) return;

        glUniform1i (location, textureIndex);
        debug::gl::test();
    }


    void gpu_program::setUniform (uniform_id uniformId, const math3d::matrix_4x4<float> &value)
    {
        if (!_testValid()) return;
        _bind();

        glUniformMatrix4fv (uniformId, 1, GL_TRUE, value.raw());
    }


    void gpu_program::setUniform (uniform_id uniformId, float value)
    {
        if (!_testValid()) return;
        _bind();

        glUniform1f (uniformId, value);
    }


    void gpu_program::setUniform (uniform_id uniformId, const math3d::vector3_f &value)
    {
        if (!_testValid()) return;
        _bind();

        glUniform3f (uniformId, value.x(), value.y(), value.z());
    }


    void gpu_program::setUniform (uniform_id uniformId, const math3d::vector2_f &value)
    {
        if (!_testValid()) return;
        _bind();

        glUniform2f (uniformId, value.x(), value.y());
    }


    void gpu_program::setUniformSampler (uniform_id uniformId, unsigned textureIndex)
    {
        if (!_testValid()) return;
        _bind();

        glUniform1i (uniformId, textureIndex);
    }


    gpu_program::~gpu_program()
    {
        if (_programId)
        {
            glDeleteProgram (_programId);
            debug::log::println_gl (mkstr (asString(), " released"));
        }

        _programId = 0;
    }


    /*virtual*/ std::string gpu_program::asString() const
    {
        return mkstr ("gpu_program [with id ", _programId, "; ", _linked ? "linked" : "raw", "]");
    }


    gpu_program::gpu_program (const gpu_program::id &resourceId, resources &renderResources) :
        gpu_program (resourceId._vertexLayout, resourceId._shaderFileNames, renderResources)
    {
        resource::_loaded();
    }


    gpu_program::gpu_program (i_vertex_layout::ptr vertexLayout, const vector<string> &shaderFileNames,
                              resources &renderResources)
        : gpu_program (vertexLayout)
    {
        for (auto& nextShaderFileName : shaderFileNames)
        {
            bool supposedFragment = nextShaderFileName.rfind (".frag") != string::npos;              // TODO: A bit inifficient
            bool supposedVertex   = nextShaderFileName.rfind (".vert") != string::npos;
            bool supposedGeometry = nextShaderFileName.rfind (".geom") != string::npos;

            if ((int) supposedFragment + (int) supposedVertex + (int) supposedGeometry != 1)
            {
                debug::log::println_err (mkstr ("failed to classify shader from filename '", nextShaderFileName, "'"));
                continue;
            }

            shader::ptr loadedShader;

            if (supposedFragment)  loadedShader = renderResources.requestFromFile<fragment_shader> (nextShaderFileName);
            if (supposedVertex)    loadedShader = renderResources.requestFromFile<vertex_shader> (nextShaderFileName);
            if (supposedGeometry)  loadedShader = renderResources.requestFromFile<geometry_shader> (nextShaderFileName);

            attach (loadedShader);
        }

        link();
    }


    gpu_program::uniform_id  gpu_program::uniformLocation (const string &name, bool ignoreIfNotExists) const
    {
        return (gpu_program::uniform_id) _locateUniform (name, ignoreIfNotExists);
    }


    bool gpu_program::isValidUniformLocation (uniform_id uniformId)
    {
        return uniformId != gpu_program::invalidUniformId;
    }
}
