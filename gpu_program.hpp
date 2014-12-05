
#ifndef __gpu_program__included__
#define __gpu_program__included__

//----------------------------------------------------------------------------------------------------------------------

#include "gpu_buffer.hpp"
#include "resource.hpp"

//----------------------------------------------------------------------------------------------------------------------

namespace render
{
    class resources;

//----------------------------------------------------------------------------------------------------------------------

    class shader :
        public resource
    {
        GLuint _shaderId  = GL_INVALID_INDEX;
        bool   _compiled  = false;

    public:
        property_get (isCompiled, _compiled)
        property_get (glId,       _shaderId)


    protected:
        void _initializeGLShader (GLenum shaderType);
        explicit shader (GLenum shaderType);
        shader (const string &fileName, GLenum shaderType);

    public:
        declare_ptr (shader)
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

    class geometry_shader : public shader
    {
    public:
        declare_ptr_alloc (geometry_shader)

        geometry_shader();
        explicit geometry_shader (const string &fileName);
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
            vector<string> _shaderFileNames;

            id (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName, const string &fragShaderFileName);
            id (i_vertex_layout::ptr vertexLayout, const string &vertShaderFileName, const string &geomShaderFileName,
                const string &fragShaderFileName);
            id (i_vertex_layout::ptr vertexLayout, vector<string> &&shaderFileNames);

            virtual string hashString() const;
        };


    protected:
        GLuint _programId = 0;
        bool   _linked    = false;

        i_vertex_layout::ptr _vertexLayout;

    public:
        property_get (glId,         _programId)
        property_get (linked,       _linked)
        property_get (vertexLayout, _vertexLayout)


    protected:
        void _initializeGLProgram();
        void _bindVertexAttributes();
        bool _testValid() const;
        void _bind() const;
        GLuint _locateUniform (const string &name, bool ignoreIfNotExists) const;

    public:
        declare_ptr_alloc (gpu_program)

        explicit gpu_program (i_vertex_layout::ptr vertexLayout);

        gpu_program (const gpu_program::id &resourceId, resources &renderResources);
        gpu_program (i_vertex_layout::ptr vertexLayout, const vector<string> &shaderFileNames, resources &renderResources);
        ~gpu_program();

        void attach (shader::ptr attachement);
        void link();
        void use() const;

        void setUniform (const string &name, float value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3d::matrix_4x4<float> &value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3d::vector3_f &value, bool ignoreIfNotExists = false);
        void setUniform (const string &name, const math3d::vector2_f &value, bool ignoreIfNotExists = false);
        void setUniformSampler (const string &name, unsigned textureIndex, bool ignoreIfNotExists = false);

        virtual string asString() const;
    };
}

//----------------------------------------------------------------------------------------------------------------------

#endif
