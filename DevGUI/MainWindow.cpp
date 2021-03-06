#include <MainWindow.h>

#include <Kernel/DevLog.h>

#include <Model/DevVertex.h>
#include <Model/DevFileModel.h>
#include <Model/DevSkyBox.h>

#include <Manipulator/DevManipulatorWASD.h>

#include <Shaders/DevShaders.h>

using namespace GUI;

MainWindow::MainWindow() :
  _hWnd(NULL),
  _hInst(NULL),
  _render(NULL),
  _input(NULL),
  _camera(NULL),
  _scene(NULL),
  _width(0),
  _height(0),
  _mouseX(0),
  _mouseY(0)
{
}

MainWindow::~MainWindow()
{
  destroy();
}

void MainWindow::destroy()
{
  if (_render)
  {
    _render->Release();
    _render = NULL;
  }
  CloseWindow(_hWnd);
  _hWnd = NULL;
}

bool MainWindow::Create(HINSTANCE hInst, int PosX, int PosY, int Width, int Height)
{
  WNDCLASS WinClass;

  _hInst = hInst;
  _width = Width;
  _height = Height;

  WinClass.style          = 0;
  WinClass.lpfnWndProc    = WindowProc;
  WinClass.cbClsExtra     = 0;
  WinClass.cbWndExtra     = 0;
  WinClass.hInstance      = _hInst;
  WinClass.hIcon          = LoadIcon(_hInst, MAKEINTRESOURCE(IDI_APPLICATION));
  WinClass.hCursor        = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
  WinClass.hbrBackground  = (HBRUSH)0;
  WinClass.lpszMenuName   = 0;
  WinClass.lpszClassName  = NAME_WINDOWCLASS;

  if (!RegisterClass(&WinClass))
  {
    MessageBox(0, "Error registration window (MainWindow.cpp)", "Error:", MB_ICONERROR);
    return FALSE;
  }

  _hWnd = 
     CreateWindowEx( 
     WS_EX_CLIENTEDGE, NAME_WINDOWCLASS, NAME_WINDOWCLASS, 
     WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE,
     PosX, PosY, Width, Height, 
     NULL, NULL, _hInst, (LPVOID)this
     );

  if (!_hWnd)
  {
    MessageBox(0, "Not _hWnd (MainWindow.cpp)", "Error:", MB_ICONERROR);
    return FALSE;
  }

  _render = new dev::Render(_hWnd);
  if (!_render->InitRender(0, 0, 0, false))
  {
    MessageBox(0, "Error init DirectX!", "Error:", MB_ICONERROR);
    return FALSE;
  }

  LoadScene();

  _render->Run();

  return TRUE;
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message)
  {
  case WM_NCCREATE :
    {
      MainWindow* MainWnd = (MainWindow *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
      MainWnd->_hWnd = hWnd;
      SetWindowLong(hWnd, GWL_USERDATA, (LONG)MainWnd);
      return TRUE;
    }
  default:
    {
      return ((MainWindow*)GetWindowLong(hWnd, GWL_USERDATA))->MsgProc(hWnd, Message, wParam, lParam);
    }
  }
}

LRESULT CALLBACK MainWindow::MsgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
  switch(Message)
  {
  case WM_KEYDOWN:
    {
      if (wParam == VK_ESCAPE)
      {
        SendMessage(hwnd, WM_DESTROY, 0, 0);
        break;
      }
      break;
    }
  case WM_DESTROY:
    {
      PostQuitMessage(0);
      break;
    }
  }

  return DefWindowProc(hwnd, Message, wParam, lParam);
}

void MainWindow::LoadScene()
{
  _camera = 
    new dev::Camera(dev::Vec3(0, 5, 0), dev::Vec3(0, 5, 5), dev::Vec3(0, 1, 0), 
    PI_4, (float)_width/_height, 1.0f, 250.0f);

  _camera->SetManipulator(new dev::ManipulatorWASD());
  _camera->GetManipulator()->SetSpeed(0.04f);
  _scene = new dev::Scene(_camera);
  _render->SetScene(_scene);
  dev::FileModel* fm = new dev::FileModel("zone.dfm");
  _scene->AddElement(fm);

  fm->GetMesh("Plane")->SetTexture("textures/plite.dds");
  fm->GetMesh("Jump1")->SetTexture("textures/plite.dds");
  fm->GetMesh("Jump2")->SetTexture("textures/plite.dds");
  fm->GetMesh("Border1")->SetTexture("textures/arrow.dds");
  fm->GetMesh("Border2")->SetTexture("textures/arrow.dds");

  dev::SkyBox* sb = new dev::SkyBox();
  sb->SetTexture("textures/city.dds", dev::Texture::TEX_CUBE);
  sb->SetCamera(_camera);
  _scene->AddElement(sb);
  
  ShowCursor(FALSE);
}

void MainWindow::Frame()
{
  _render->Frame();
}
