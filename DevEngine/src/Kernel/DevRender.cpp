#include <Kernel/DevRender.h>

#include <Kernel/DevLog.h>
#include <Shaders/DevShaderManager.h>

using namespace dev;

Render::Render() :
  _width(0),
  _height(0),
  _scene(NULL)
{
  _wnd = new Window();
  _hWnd = _wnd->Create(GetModuleHandle(0), CW_USEDEFAULT, CW_USEDEFAULT, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

  if (!_hWnd)
  {
    Log::GetLog()->WriteToLog("Not _hWnd (DevRender.cpp)");
  }
}

Render::Render(HWND hWnd) :
  _hWnd(hWnd),
  _width(0),
  _height(0),
  _wnd(NULL),
  _scene(NULL)
{
}

Render::Render(HINSTANCE hInstance, int PosX, int PosY, int Width, int Height) :
  _width(Width),
  _height(Height),
  _scene(NULL)
{
  _wnd = new Window();
  _hWnd = _wnd->Create(hInstance, PosX, PosY, Width, Height);

  if (!_hWnd)
  {
    Log::GetLog()->WriteToLog("Not _hWnd (DevRender.cpp)");
  }
}

Render::~Render()
{
  Destroy();
}

void Render::Destroy()
{
  if (_hWnd)
  {
    _hWnd = NULL;
    SendMessage(_hWnd, WM_CLOSE, 0, 0);
  }
  if (_deviceDX)
  {
    _deviceDX->Release();
    _deviceDX = NULL;
  }
  if (_directX)
  {
    _directX->Release();
    _directX = NULL;
  }
  if (_scene)
  {
    delete _scene;
    _scene = NULL;
  }
  if (_wnd)
  {
    delete _wnd;
    _wnd = NULL;
  }

  SystemInput::Release();
  SystemCS::Release();
  SystemTimer::Release();
}

bool Render::InitRender(int width, int height, int RefreshHz, bool FullScreenMode)
{
  if (!_directX)
    if (!(_directX = Direct3DCreate9(D3D_SDK_VERSION)))
    {
      Log::GetLog()->WriteToLog("Failed init Direct3D (DevRender.cpp)");
      return false;
    }

  ZeroMemory(&_display, sizeof(_display));
  if (FAILED(_directX->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &_display)))
  {
    Log::GetLog()->WriteToLog("Failed init DisplayMode (DevRender.cpp)");
    return false;
  }

  if (FullScreenMode)
  {
    _width = _display.Width;
    _height = _display.Height;
  }
  else
    if ((!width) || (!height))
    {
      RECT rect;
      GetClientRect(_hWnd, &rect);
      _width  = rect.right - rect.left;
      _height = rect.bottom - rect.top;
    }
    else
    {
      _width = width;
      _height = height;
    }

  ZeroMemory(&_parametersD3D, sizeof(_parametersD3D));
  if (FullScreenMode)
  {
    if (RefreshHz > 0) 
      _parametersD3D.FullScreen_RefreshRateInHz = RefreshHz;
    else 
      _parametersD3D.FullScreen_RefreshRateInHz = _display.RefreshRate;
  }

  _parametersD3D.BackBufferCount           = 0;
  _parametersD3D.BackBufferWidth           = _width;
  _parametersD3D.BackBufferHeight          = _height;
  _parametersD3D.Windowed                  = !FullScreenMode;
  _parametersD3D.SwapEffect                = D3DSWAPEFFECT_DISCARD;
  _parametersD3D.BackBufferFormat          = _display.Format;
  _parametersD3D.EnableAutoDepthStencil    = true;
  _parametersD3D.AutoDepthStencilFormat    = D3DFMT_D24S8;
  _parametersD3D.PresentationInterval      = D3DPRESENT_INTERVAL_IMMEDIATE;
  _parametersD3D.MultiSampleType           = D3DMULTISAMPLE_NONE;

  if (FAILED(_directX->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, _hWnd,
    D3DCREATE_HARDWARE_VERTEXPROCESSING, &_parametersD3D, &_deviceDX)))
  {
    Log::GetLog()->WriteToLog("Failed device Direct3D (DirectMain.cpp)");
    return false;
  }

  _deviceDX->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
  _deviceDX->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

  if (!FullScreenMode)
    ShowWindow(_hWnd, SW_SHOW);

  return true;
}

void Render::SetScene(Scene* scene)
{
  _scene = scene;
  SystemTimer::Get()->Reset(0);
}

void Render::Run()
{
  if (_wnd)
  {
    MSG msg = {0};

    while(true)
    {
      while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
      {
        if(!GetMessage(&msg, NULL, 0, 0))
          return;
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      Frame();
    }
  }
}

void Render::Frame()
{
  if (_scene)
    _scene->Update();
  
  _deviceDX->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, clearColor, 1.0f, 0);
  _deviceDX->BeginScene();
  _deviceDX->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
  _deviceDX->SetRenderState(D3DRS_LIGHTING, false);

  DevShaderManager::Get()->Update();

  _deviceDX->EndScene();
  _deviceDX->Present(0, 0, 0, 0);
}

void Render::Release()
{
  delete this;
}
