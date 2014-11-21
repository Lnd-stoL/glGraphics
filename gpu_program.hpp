
#ifndef __gpu_program__included__
#define __gpu_program__included__

//----------------------------------------------------------------------------------------------------------------------

#include "oo_extensions.hpp"
#include "math3D.hpp"
#include "gpu_buffer.hpp"
#include "resource.hpp"

#include <GL/glew.h>

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class resources;

//----------------------------------------------------------------------------------------------------------------------

    class shader :
        public oo_extensions::non_copyable,
        public oo_extensions::i_as_string
    {
        GLuint _shaderId  = GL_INVALID_INDEX;
        bool   _compiled  = false;

    public:
        property_get (Compiled, _compiled)
        property_get (GlId,     _shaderId)

    protected:
        void _initializeGLShader (GLenum shaderType);
        explicit shader (GLenum shaderType);
        shader (const string &fileName, GLint shaderType);

    public:
        declare_ptr_alloc (shader)
        ~shader();

        void compileFromFile (const string &fileName);
        void compile (const string &codeString);

        virtual string asString() const;
    };

//----------------------------------------------------------------------------------------------------------------------

    class vertex_shader : public shader
    {
    public:
        declare_ptr_alloc (vertex_shader)

        vertex_shader();
        explicit vertex_shader (const string &fileName);
    };

//----------------------------------------------------------------------------------------------------------------------

    class fragment_shader : public shader
    {
    public:
        declare_ptr_alloc (fragment_shader)

        fragment_shader();
        explicit fragment_shader (const string &fileName);
    };

//----------------------------------------------------------------------------------------------------------------------

    class gpu_program :
        public resource,
        public gl_bindable<gpu_program>
    {
    public:
        struct id : resource::id
        {
            i_vertex_layout::ptr _vertexLayout;
            string _vertShaderFileName;
            string _fragShaderFileName;

            id (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName, const string &fragShaderFileName) :
                _vertexLayout (vertexLayout),
                _vertShaderFileName (vertShaderFileName),
                _fragShaderFileName (fragShaderFileName)
            { }

            virtual string hashString() const;
        };


    protected:
        GLuint _programId = GL_INVALID_INDEX;
        bool   _linked    = false;

        i_vertex_layout::ptr _vertexLayout;

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
        GLuint _locateUniform (const string &name, bool ignoreIfNotExists) const;

    public:
        declare_ptr_alloc (gpu_program)

        explicit gpu_program (i_vertex_layout::ptr vertexLayout);

        gpu_program (const gpu_program::id &resourceId, resources &renderResources);
        gpu_program (i_vertex_layout::ptr vertexLayout, const vertex_shader &vshader, const fragment_shader &fshader);
        gpu_program (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName,
                     const string &fragShaderFileName, resources &renderResources);
        ~gpu_program();

        void attach (const shader& attachement);
        void link();
        void use() const;

        void setUniform (const string &name, float value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3D::matrix_4x4<float> &value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3D::vector3_f &value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3D::vector2_f &value, bool ignoreIfNotExists = false);
        void setUniformSampler (const string &name, unsigned textureIndex, bool ignoreIfNotExists = false);

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
