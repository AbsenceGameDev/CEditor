#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>


#include "../inc/dataformat.h"
#include "../inc/editor_core.h"
#include "../inc/ui/handler.h"

// Data
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static bool g_SwapChainOccluded = false;
static UINT g_ResizeWidth = 0;
static UINT g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void MainLoop(ImGuiIO& io)
{
   // Start with loading our congifs ot otehr threads
   EditorCore::StartReadConfigs();
   
   // states
   ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

   // Actual Main loop
   bool IsDone = false;
   while (IsDone == false)
   {
      // Poll and handle messages (inputs, window resize, etc.)
      // See the WndProc() function below for our to dispatch events to the Win32 backend.
      MSG Msg;
      while (::PeekMessage(&Msg, nullptr, 0U, 0U, PM_REMOVE))
      {
         ::TranslateMessage(&Msg);
         ::DispatchMessage(&Msg);

         if (Msg.message == WM_QUIT) { IsDone = true; }
      }
      if (IsDone) { break; }

      // Handle window being minimized or screen locked
      if (g_SwapChainOccluded && g_pSwapChain->Present(0, DXGI_PRESENT_TEST) == DXGI_STATUS_OCCLUDED)
      {
         ::Sleep(10);
         continue;
      }
      g_SwapChainOccluded = false;

      // Handle window resize (we don't resize directly in the WM_SIZE handler)
      if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
      {
         CleanupRenderTarget();
         g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
         g_ResizeWidth = g_ResizeHeight = 0;
         CreateRenderTarget();
      }

      // Start the Dear ImGui frame
      ImGui_ImplDX11_NewFrame();
      ImGui_ImplWin32_NewFrame();
      ImGui::NewFrame();

      //
      // Setting up windows before rendering pass
      UIHandler::OnPaint(io, ClearColor);

      // Rendering
      ImGui::Render();
      const float ClearColorWAlpha[4] = {
         ClearColor.x * ClearColor.w, ClearColor.y * ClearColor.w, ClearColor.z * ClearColor.w, ClearColor.w
      };
      g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
      g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, ClearColorWAlpha);
      ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

      // Present
      HRESULT hr = g_pSwapChain->Present(1, 0); // Present with vsync
      //HRESULT hr = g_pSwapChain->Present(0, 0); // Present without vsync
      g_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
   }
}

// Main code
int main(int, char**)
{
   // Create application window
   //ImGui_ImplWin32_EnableDpiAwareness();
   WNDCLASSEXW Wc = {
      sizeof(Wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr
   };
   ::RegisterClassExW(&Wc);
   HWND Hwnd = ::CreateWindowW(Wc.lpszClassName, L"CEditor", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, Wc.hInstance,
                               nullptr);

   // Initialize Direct3D
   if (!CreateDeviceD3D(Hwnd))
   {
      CleanupDeviceD3D();
      ::UnregisterClassW(Wc.lpszClassName, Wc.hInstance);
      return 1;
   }

   // Show the window
   ::ShowWindow(Hwnd, SW_SHOWDEFAULT);
   ::UpdateWindow(Hwnd);

   // Setup Dear ImGui context
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& IODevice = ImGui::GetIO();
   (void)IODevice;
   IODevice.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
   IODevice.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

   // Setup Dear ImGui style
   ImGui::StyleColorsDark();
   //ImGui::StyleColorsLight();

   // Setup Platform/Renderer backends
   ImGui_ImplWin32_Init(Hwnd);
   ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

   // Load Fonts
   UIHandler::GlobalSmallFont = UIHandler::LoadFonts(IODevice);

   // Run main loop
   MainLoop(IODevice);

   // Deinit ui before ctors of certain maps are called  
   UIHandler::DeinitUI();

   // Cleanup
   ImGui_ImplDX11_Shutdown();
   ImGui_ImplWin32_Shutdown();
   ImGui::DestroyContext();

   CleanupDeviceD3D();
   ::DestroyWindow(Hwnd);
   ::UnregisterClassW(Wc.lpszClassName, Wc.hInstance);

   return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
   // Setup swap chain
   DXGI_SWAP_CHAIN_DESC SwapDescr;
   ZeroMemory(&SwapDescr, sizeof(SwapDescr));
   SwapDescr.BufferCount = 2;
   SwapDescr.BufferDesc.Width = 0;
   SwapDescr.BufferDesc.Height = 0;
   SwapDescr.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
   SwapDescr.BufferDesc.RefreshRate.Numerator = 60;
   SwapDescr.BufferDesc.RefreshRate.Denominator = 1;
   SwapDescr.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
   SwapDescr.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
   SwapDescr.OutputWindow = hWnd;
   SwapDescr.SampleDesc.Count = 1;
   SwapDescr.SampleDesc.Quality = 0;
   SwapDescr.Windowed = TRUE;
   SwapDescr.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

   UINT CreateDeviceFlags = 0;
   //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
   D3D_FEATURE_LEVEL FeatureLevel;
   const D3D_FEATURE_LEVEL FeatureLevelArray[2] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0,};
   HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, CreateDeviceFlags, FeatureLevelArray, 2,
                                               D3D11_SDK_VERSION, &SwapDescr, &g_pSwapChain, &g_pd3dDevice, &FeatureLevel,
                                               &g_pd3dDeviceContext);
   if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
   {
      res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, CreateDeviceFlags, FeatureLevelArray, 2,
                                          D3D11_SDK_VERSION, &SwapDescr, &g_pSwapChain, &g_pd3dDevice, &FeatureLevel, &g_pd3dDeviceContext);
   }

   if (res != S_OK)
   {
      return false;
   }

   CreateRenderTarget();
   return true;
}

void CleanupDeviceD3D()
{
   CleanupRenderTarget();
   if (g_pSwapChain)
   {
      g_pSwapChain->Release();
      g_pSwapChain = nullptr;
   }
   if (g_pd3dDeviceContext)
   {
      g_pd3dDeviceContext->Release();
      g_pd3dDeviceContext = nullptr;
   }
   if (g_pd3dDevice)
   {
      g_pd3dDevice->Release();
      g_pd3dDevice = nullptr;
   }
}

void CreateRenderTarget()
{
   ID3D11Texture2D* pBackBuffer;
   g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
   g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
   pBackBuffer->Release();
}

void CleanupRenderTarget()
{
   if (g_mainRenderTargetView == nullptr) { return; }

   g_mainRenderTargetView->Release();
   g_mainRenderTargetView = nullptr;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
   if (ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam))
      return true;

   switch (Msg)
   {
   case WM_SIZE:
      if (wParam == SIZE_MINIMIZED) { return 0; }

      g_ResizeWidth = static_cast<UINT>(LOWORD(lParam)); // Queue resize
      g_ResizeHeight = static_cast<UINT>(HIWORD(lParam));
      return 0;
   case WM_SYSCOMMAND:
      // Disable ALT application menu
      if ((wParam & 0xfff0) == SC_KEYMENU) { return 0; }
      break;
   case WM_DESTROY:
      ::PostQuitMessage(0);
      return 0;

   default: break;
   }
   return ::DefWindowProcW(hWnd, Msg, wParam, lParam);
}
