#ifndef DEV_SHADERS_H
#define DEV_SHADERS_H

#include <Kernel/DevInclude.h>
#include <Element/DevElement.h>

namespace dev
{
  class Shader : public Object
  {
  public:
    Shader(const char* path, DWORD type, const char* fuction);
    virtual ~Shader();

    virtual void CompileShader();

    inline const char* GetPath() const
    {
      return _path.c_str();
    }

    inline const DWORD* GetType() const
    {
      return &_type;
    }

    inline const char* GetFunction() const
    {
      return _function.c_str();
    }

    virtual void SetShader();

  protected:

    virtual const char* getCharType();
    virtual bool supportTypeShader();

    ID3DXConstantTable*     _constantTable;
    ID3DXBuffer*            _shaderBuffer;
    std::string             _path;
    DWORD                   _type;
    std::string             _function;
  };

  class VertexShader : public Shader
  {
  public:

    enum TypeVertexShader
    {
      VS_1_1,
      VS_2_0,
      VS_2_a,
      VS_2_SW,
      VS_3_0,
      VS_3_SW
    };

    VertexShader(const char* path, TypeVertexShader type, const char* fuction = functionShader);
    virtual ~VertexShader();

    virtual void CompileShader();

    virtual void SetShader();

  protected:
    virtual const char* getCharType();
    virtual bool supportTypeShader();

    IDirect3DVertexShader9* _shader;
  };

  class PixelShader : public Shader
  {
  public:

    enum TypePixelShader
    {
      PS_1_0,
      PS_1_1,
      PS_1_2,
      PS_1_3,
      PS_1_4,
      PS_2_0,
      PS_2_a,
      PS_2_b,
      PS_2_SW,
      PS_3_0,
      PS_3_SW
    };

    PixelShader(const char* path, TypePixelShader type, const char* fuction = functionShader);
    virtual ~PixelShader();

    virtual void CompileShader();

    virtual void SetShader();

  protected:
    virtual const char* getCharType();
    virtual bool supportTypeShader();

    IDirect3DPixelShader9* _shader;
  };
}

#endif