/*
This code is made by CiberBoy and protected with the GPL-3 license, meaning that the use of parts of this code  requieres that you credit my original project
and that your project is open source as well! So, if you skid/take parts from it, credit me and make your program open source.
*/
#pragma comment(lib, "Msimg32.lib")
#include <windows.h>
#include <math.h>

#define WIDTH  600
#define HEIGHT 600
#define TIMER_ID 1
#define PI 3.14159265359
struct Vertex {
    double x, y, z;
    COLORREF color;
};
Vertex cube[8] = {
    {-1, -1, -1, RGB(255, 0, 0)},  {1, -1, -1, RGB(0, 255, 0)},
    {1, 1, -1, RGB(0, 0, 255)},    {-1, 1, -1, RGB(255, 255, 0)},
    {-1, -1, 1, RGB(0, 255, 255)}, {1, -1, 1, RGB(255, 0, 255)},
    {1, 1, 1, RGB(255, 165, 0)},   {-1, 1, 1, RGB(128, 0, 128)}
};
//cube faces
int faces[6][4] = {
    {0, 1, 2, 3}, {4, 5, 6, 7}, //back and front
    {0, 1, 5, 4}, {2, 3, 7, 6}, //L and r
    {0, 3, 7, 4}, {1, 2, 6, 5}  //top - bottom
};
double angleX = 0, angleY = 0;
Vertex rotate(Vertex p) {
    double cosX = cos(angleX), sinX = sin(angleX);
    double cosY = cos(angleY), sinY = sin(angleY);
    double y = p.y * cosX - p.z * sinX;
    double z = p.y * sinX + p.z * cosX;
    p.y = y, p.z = z;
    double x = p.x * cosY + p.z * sinY;
    p.z = -p.x * sinY + p.z * cosY;
    p.x = x;
    return p;
}
POINT project(Vertex p) {
    int scale = 100;
    int px = (int)(p.x * scale) + WIDTH / 2;
    int py = (int)(-p.y * scale) + HEIGHT / 2;
    return { px, py };
}
COLORREF interpolateclr(COLORREF c1, COLORREF c2, double t) {
    int r = (1 - t) * GetRValue(c1) + t * GetRValue(c2);
    int g = (1 - t) * GetGValue(c1) + t * GetGValue(c2);
    int b = (1 - t) * GetBValue(c1) + t * GetBValue(c2);
    return RGB(r, g, b);
}
void drawgradient(HDC hdc, POINT poly[4], COLORREF colors[4]) {
    TRIVERTEX vertices[4] = {
        { poly[0].x, poly[0].y, (COLOR16)(GetRValue(colors[0]) << 8), (COLOR16)(GetGValue(colors[0]) << 8), (COLOR16)(GetBValue(colors[0]) << 8), 0 },
        { poly[1].x, poly[1].y, (COLOR16)(GetRValue(colors[1]) << 8), (COLOR16)(GetGValue(colors[1]) << 8), (COLOR16)(GetBValue(colors[1]) << 8), 0 },
        { poly[2].x, poly[2].y, (COLOR16)(GetRValue(colors[2]) << 8), (COLOR16)(GetGValue(colors[2]) << 8), (COLOR16)(GetBValue(colors[2]) << 8), 0 },
        { poly[3].x, poly[3].y, (COLOR16)(GetRValue(colors[3]) << 8), (COLOR16)(GetGValue(colors[3]) << 8), (COLOR16)(GetBValue(colors[3]) << 8), 0 }
    };
    GRADIENT_TRIANGLE triangles[2] = { {0, 1, 2}, {2, 3, 0} };
    GradientFill(hdc, vertices, 4, triangles, 2, GRADIENT_FILL_TRIANGLE);
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    static PAINTSTRUCT ps;
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, TIMER_ID, 30, NULL);
        break;
    case WM_TIMER:
        if (wParam == TIMER_ID) {
            angleX += 0.05;
            angleY += 0.05;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;
    case WM_PAINT:
    {
        hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        for (int i = 0; i < 6; i++) {
            POINT poly[4];
            COLORREF colors[4];
            for (int j = 0; j < 4; j++) {
                Vertex v = rotate(cube[faces[i][j]]);
                poly[j] = project(v);
                colors[j] = v.color;
            }
            drawgradient(hdc, poly, colors);
        }
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_DESTROY:
        KillTimer(hwnd, TIMER_ID);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("Cube3D");
    RegisterClass(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, TEXT("3d cube example by ciberboy"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
