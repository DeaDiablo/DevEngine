#include <Model/DevMesh.h>

#include <Shaders/DevShaderManager.h>

using namespace dev;

Mesh::Mesh(const char* name, const Vec3& position, const Vec3& rotation, const Vec3& scale) :
  Element(position, rotation, scale),
  _name(name),
  _vertices(NULL),
  _indexes(NULL),
  _vShader(NULL),
  _pShader(NULL),
  _orderNum(0),
  _material(0)
{
  dirtyTextures();
}

Mesh::Mesh(const char* name) :
  Element(),
  _name(name),
  _vertices(NULL),
  _indexes(NULL),
  _vShader(NULL),
  _pShader(NULL),
  _material(NULL),
  _orderNum(0)
{
  dirtyTextures();
}

Mesh::~Mesh()
{
  dirtyTextures();
  if (_material)
  {
    delete _material;
    _material = NULL;
  }
}

void Mesh::SetVertices(LPVOID vertices, int numberVertex, Vertex::VertexType VT_Type)
{
  _vertices = new Vertex::ArrayVertices(vertices, numberVertex, VT_Type);
}

void Mesh::SetIndexes(LPVOID indexes, int numberIndexes, D3DFORMAT D3DFMT_INDEX)
{
  _indexes = new Vertex::ArrayIndexes(indexes, numberIndexes, D3DFMT_INDEX);
}

void Mesh::SetVerticesFromFile(HANDLE file, int numberVertex, Vertex::VertexType VT_Type)
{
  if (_vertices == NULL)
    _vertices = new Vertex::ArrayVertices();

  _vertices->SetVerticesFromFile(file, numberVertex, VT_Type);
}

void Mesh::SetIndexesFromFile(HANDLE file, int numberIndexes, D3DFORMAT D3DFMT_INDEX)
{
  if (_indexes == NULL)
    _indexes = new Vertex::ArrayIndexes();

  _indexes->SetIndexesFromFile(file, numberIndexes, D3DFMT_INDEX);
}

void Mesh::Draw()
{
  if (_vertices == NULL)
    return;

  Element::Draw();

  _vertices->SetAsSource();
  if (_indexes == NULL)
  {
    _deviceDX->DrawPrimitive(D3DPT_TRIANGLELIST, 0, _vertices->GetNumberVertices());
  }
  else
  {
    _indexes->SetAsSource();
    _deviceDX->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, _indexes->GetNumberIndexes(), 0, _indexes->GetNumberIndexes()/3);
  }
}

void Mesh::SetVertexShader(const char* path, VertexShader::TypeVertexShader type, const char* funcName)
{
  if (_vShader)
    DevShaderManager::Get()->RemoveVertexShader(_vShader);
  
  _vShader = DevShaderManager::Get()->GetVertexShader(path, type, funcName);
}

void Mesh::SetPixelShader(const char* path, PixelShader::TypePixelShader type, const char* funcName)
{

  if (_pShader)
    DevShaderManager::Get()->RemovePixelShader(_pShader);

  _pShader = DevShaderManager::Get()->GetPixelShader(path, type, funcName);

  if (_pShader && !_material)
    _material = new Material();
}

void Mesh::SetOrderNum(int num)
{
  _orderNum = num;
}

void Mesh::SetMaterial(Material* material)
{
  _material = material;
}

void Mesh::UpdateParameters()
{
  Element::UpdateParameters();
  if (_material)
  {
    if (_pShader == NULL)
      _deviceDX->SetMaterial(&_material->GetAsD3DMaterial());
  }

  for(unsigned int i = 0; i < _textures.size(); i++)
    _textures[i]->SetTexture(i);
}

void dev::Mesh::ReturnParameters()
{
  for(unsigned int i = 0; i < _textures.size(); i++)
    _deviceDX->SetTexture(i, NULL);
}

void Mesh::RemoveTexture(const char* path)
{
  for (TextureList::iterator i = _textures.begin(); i != _textures.end(); i++)
  {
    if ((*i)->GetPath() == path)
    {
      DevTextureManager::Get()->RemoveTexture((*i)->GetPath());
      _textures.erase(i);
    }
  }
}

void Mesh::SetTexture(const char* path, Texture::TypeTexture type)
{
  for (unsigned int i = 0; i < _textures.size(); i++)
    if (_textures[i]->GetPath() == path)
      return;

  _textures.push_back(DevTextureManager::Get()->GetTexture(path, type));
}

void Mesh::dirtyTextures()
{
  for (unsigned int i = 0; i < _textures.size(); i++)
    DevTextureManager::Get()->RemoveTexture(_textures[i]);
  _textures.clear();
}

