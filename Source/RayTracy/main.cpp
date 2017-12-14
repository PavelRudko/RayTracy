#include <stdint.h>
#include <iostream>
#include <memory.h>
#include "Renderer.h"

#ifdef PLATFORM_WINDOWS

#include <Windows.h>

uint32_t width, height;
uint8_t* buffer;
HWND window;
HDC src;
HBITMAP bitmap;
Renderer renderer;

void CreateBuffer()
{
    RECT windowRect;
    GetClientRect(window, &windowRect);
    width = windowRect.right - windowRect.left;
    height = windowRect.bottom - windowRect.top;

    auto hdc = GetDC(window);
    bitmap = CreateBitmap(width, height, 1, 4 * 8, buffer);
    src = CreateCompatibleDC(hdc);
    ReleaseDC(window, hdc);

    BITMAPINFO bmi;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 4 * 8;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = width * height * 4;
    bitmap = CreateDIBSection(src, &bmi, DIB_RGB_COLORS, (void**)&buffer, NULL, 0);

    SelectObject(src, bitmap);
}

void DestroyBuffer()
{
    DeleteDC(src);
    DeleteObject(bitmap);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
    }
    else if (message == WM_PAINT && src != NULL)
    {
        renderer.Render(buffer, width, height);
        
        PAINTSTRUCT ps;
        auto dc = BeginPaint(window, &ps);
        BitBlt(dc, 0, 0, width, height, src, 0, 0, SRCCOPY);
        EndPaint(window, &ps);
    }
    else if (message == WM_SIZE)
    {
        DestroyBuffer();
        CreateBuffer();
    }
    return DefWindowProc(hwnd, message, wparam, lparam);
}

int main(int argc, char** argv)
{
    src = NULL;
    if (!renderer.Initialize(argc, argv)) {
        return -1;
    }

    WNDCLASS windowClass;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.lpszClassName = "Window";
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpfnWndProc = &WindowProcedure;
    RegisterClass(&windowClass);

    window = CreateWindow(windowClass.lpszClassName, "RayTracy", WS_OVERLAPPEDWINDOW, 100, 100, 640, 480, NULL, NULL, windowClass.hInstance, NULL);
    
    CreateBuffer();
    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    MSG message;
    while (GetMessage(&message, NULL, 0, 0) > 0) {
        DispatchMessage(&message);
    }

    DestroyBuffer();
    renderer.CleanUp();

    return 0;
}

#else

#include <X11/Xlib.h>
#include <X11/Xutil.h>

uint32_t width, height;
uint8_t* buffer;
XImage* image;
Display* display;
Window window;

void GetWindowSize(Display* display, Window window, uint32_t* width, uint32_t* height)
{
    Window w;
    uint32_t  borderWidth, depth;
    int x, y;
    XGetGeometry(display, window, &w, &x, &y, width, height, &borderWidth, &depth);
}

void CreateBuffer()
{
    GetWindowSize(display, window, &width, &height);
    uint32_t bufferLength = width * height * 4;
    buffer = new uint8_t[bufferLength];
    auto visual = DefaultVisual(display, 0);
    image = XCreateImage(display, visual, 24, ZPixmap, 0, (char*)buffer, width, height, 8 * 4, 0);
}

void DestroyBuffer()
{
    XDestroyImage(image);
}

int main(int argc, char** argv)
{
    Renderer renderer;
    if (!renderer.Initialize(argc, argv)) {
        return -1;
    }
    display = XOpenDisplay(NULL);
    int whiteColor = WhitePixel(display, DefaultScreen(display));
    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 100, 100, 640, 480, 0, whiteColor, whiteColor);
    XSelectInput(display, window, StructureNotifyMask | ExposureMask);
    XMapWindow(display, window);
    XStoreName(display, window, "RayTracy");
    auto context = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, context, whiteColor);

    auto deleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &deleteMessage, 1);
    
    CreateBuffer();

    bool isRunning = true;
    while (isRunning) {
        while (XPending(display)) {
            XEvent event;
            XNextEvent(display, &event);
            if (XFilterEvent(&event, None)) {
                continue;
            }
            if (event.type == ClientMessage && event.xclient.data.l[0] == deleteMessage) {
                isRunning = false;
                break;
            }
            else if (event.type == Expose) {
                DestroyBuffer();
                CreateBuffer();
            }
        }
        if (isRunning) {
            renderer.Render(buffer, width, height);
            XPutImage(display, window, DefaultGC(display, 0), image, 0, 0, 0, 0, width, height);
        }
    }

    DestroyBuffer();
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    renderer.CleanUp();

    return 0;
}

#endif


