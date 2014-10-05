
#ifndef __gpu_program__included__
#define __gpu_program__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "vertex_buffer.hpp"

#include <GL/glew.h>
#include <SFML/Graphics.hpp>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class shader :
        public oo_extensions::non_copyable,
        public oo_extensions::i_as_string
    {
        GLint _shaderId  = GL_INVALID_INDEX;
        bool  _compiled  = false;

    public:
        property_get (Compiled, _compiled)
        property_get (GlId,     _shaderId)

    protected:
        void _initializeGLShader (GLenum shaderType);
        shader (GLint shaderType);
        shader (const string &fileName, GLint shaderType);

    public:
        ~shader();

        void compileFromFile (const string &fileName);
        void compile (const string &codeString);

        virtual string asString() const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class vertex_shader
        : public shader
    {
    public:
        vertex_shader();
        vertex_shader (const string &fileName);
    };

//----------------------------------------------------------------------------------------------------------------------

    class fragment_shader
        : public shader
    {
    public:
        fragment_shader();
        fragment_shader (const string &fileName);
    };

//----------------------------------------------------------------------------------------------------------------------

    class gpu_program :
        public oo_extensions::non_copyable,
        public oo_extensions::i_as_string
    {
        GLuint _programId = GL_INVALID_INDEX;
        bool   _linked    = false;

        shared_ptr<i_vertex_layout> _vertexLayout;

    public:
        property_get (GlId,         _programId)
        property_get (Linked,       _linked)
        property_get (VertexLayout, _vertexLayout)


    protected:
        void _initializeGLProgram();
        void _attachAndLink (const vertex_shader &vshader, const fragment_shader &fshader);
        void _bindVertexAttributes();
        bool _testValid() const;
        void _bind() const;
        GLuint _locateUniform (const string &name) const;

    public:
        gpu_program (shared_ptr<i_vertex_layout> vertexLayout);
        gpu_program (shared_ptr<i_vertex_layout> vertexLayout, const vertex_shader &vshader, const fragment_shader &fshader);
        gpu_program (shared_ptr<i_vertex_layout> vertexLayout, const string &vertShaderFileName, const string &fragShaderFileName);
        ~gpu_program();

        void attach (const shader& attachement);
        void link();
        void use() const;

        void setUniform (const string &name, const math3D::matrix_4x4<float> &value);
        void setUniform (const string &name, const math3D::vector3_f &value);
        void setUniformSampler (const string &name, unsigned textureIndex);

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
