#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include <string>
#include <fstream>
#include "windows/Loader.h"
#include "windows/InputDay.h"
#include "windows/Window.h"
#include "windows/Main.h"
#include "gui/zahnrad.h"
#include "GUI.h"
#include "member/Member.h"
#include "member/Regular.h"
#include "member/Executive.h"
#include "util/Trip.h"
#include "util/Item.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define MAX_MEMORY  (32 * 1024)
#define MAX_BUFFER  64
#define UNUSED(a)   ((void)(a))
#define MAX_ITEMS 256

using namespace std;

//variable declaration
XWindow xw;
Attributes gui;
Window **windows;
int num_windows = 3;
int window_index = 0;

int num_members = 12; //change if needed
int num_days = 5;
int *purchases_a_day;
Item **items;
int *num_items;
Member **members; //sexy right?
Trip **trips; //it's ra1ning 2-dimensional arrays!

//INSTRUCTIONS TO ADD gdi32!!
//"BulkClub"->Properties->C/C++ Build->Settings->
//MinGW C++ Linker->Libraries->Libraries (-l)->
//*click icon with green plus sign*->Type "gdi32" without quotes->
//Ok->Apply->Ok

LRESULT CALLBACK wnd_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        gui.running = false;
        break;
    case WM_SIZE:
        if (xw.backbuffer) {
        xw.width = LOWORD(lParam);
        xw.height = HIWORD(lParam);
        surface_resize(xw.backbuffer, xw.hdc, xw.width, xw.height);
        } break;
    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR lpCmdLine, int shown) {
	FreeConsole();
    xw.wc.style = CS_HREDRAW|CS_VREDRAW;
    xw.wc.lpfnWndProc = wnd_proc;
    xw.wc.hInstance = hInstance;
    xw.wc.lpszClassName = "GUI";
    RegisterClass(&xw.wc);
    xw.hWnd = CreateWindowEx(
        0, xw.wc.lpszClassName, "BulkClub!",
        WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        0, 0, hInstance, 0);

    xw.hdc = GetDC(xw.hWnd);
    GetClientRect(xw.hWnd, &xw.rect);
    xw.backbuffer = surface_new(xw.hdc, xw.rect.right, xw.rect.bottom);
    xw.font = font_new(xw.hdc, "Times New Roman", 16);
    xw.width = xw.rect.right;
    xw.height = xw.rect.bottom;

    gui.font.userdata = zr_handle_ptr(&xw);
    gui.font.height = (zr_float)xw.font->height;
    gui.font.width =  font_get_text_width;
    zr_style_default(&gui.style, ZR_DEFAULT_ALL, &gui.font);
    zr_command_queue_init_fixed(&gui.queue, malloc(MAX_MEMORY), MAX_MEMORY);

    zr_window_init(&gui.window, zr_rect(5, 5, WINDOW_WIDTH - 20, WINDOW_HEIGHT - 40),
	ZR_WINDOW_BORDER,
	&gui.queue, &gui.style, &gui.input);

    set_style(&gui.style);

    purchases_a_day = new int[5];
    purchases_a_day[0] = 13; //day1
    purchases_a_day[1] = 12; //day2
    purchases_a_day[2] = 10; //day3
    purchases_a_day[3] = 12; //day4
    purchases_a_day[4] = 13; //day5
    members = new Member*[num_members];
	trips = new Trip*[num_days];
	items = new Item*[MAX_ITEMS];
	for (int i = 0; i < num_days; i++) trips[i] = new Trip[purchases_a_day[i]];
	num_items = new int;
	*num_items = 0;

    windows = new Window*[num_windows];
    windows[0] = new Loader(purchases_a_day, items, num_items, members, num_members, trips, num_days);
    windows[1] = new Main(purchases_a_day, items, num_items, members, num_members, trips, num_days);
    windows[2] = new InputDay(purchases_a_day, items, num_items, members, num_members, trips, num_days);
    //load your windows here!

    gui.running = true;

    while (gui.running) {
        /* Input */
        MSG msg;
        zr_input_begin(&gui.input);
        while (PeekMessage(&msg, xw.hWnd, 0, 0, PM_REMOVE)) {
        	if (msg.message == WM_KEYDOWN)
        		input_key(&gui.input, &msg, zr_true);
			else if (msg.message == WM_KEYUP)
				input_key(&gui.input, &msg, zr_false);
			else if (msg.message == WM_LBUTTONDOWN)
				input_btn(&gui.input, &msg, zr_true);
			else if (msg.message == WM_LBUTTONUP)
				input_btn(&gui.input, &msg, zr_false);
			else if (msg.message == WM_MOUSEMOVE)
				input_motion(&gui.input, &msg);
			else if (msg.message == WM_CHAR)
				input_text(&gui.input, &msg);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        zr_input_end(&gui.input);

        windows[window_index]->render_main(&gui.window);
        if (windows[window_index]->do_update()) {
        	windows[window_index]->set_data(purchases_a_day, items, num_items, members, num_members, trips, num_days);
        	for (int i = 0; i < num_windows; i++){
        		if (i != window_index) windows[i]->update_data(purchases_a_day, items, num_items, members, num_members, trips, num_days);
        	}
        }
        window_index = windows[window_index]->setWindow();

        surface_begin(xw.backbuffer);
        surface_clear(xw.backbuffer, 100, 100, 100);
        draw(xw.backbuffer, &gui.queue);
        surface_end(xw.backbuffer, xw.hdc);
    }

    free(zr_buffer_memory(&gui.queue.buffer));
    font_del(xw.font);
    surface_del(xw.backbuffer);
    ReleaseDC(xw.hWnd, xw.hdc);
    return 0;
}

