#include <Windows.h>
#include <tchar.h>
#include "resource.h"

#define TIMER 1
const int clientWidth = 500;  //客户区宽
const int clientHeight = 500; //客户区高
const int wallWidth = 25;     //墙的宽
const int wallHeight = 25;    //墙的高

BOOL isCX = TRUE;        //是否能穿墙(完整功能暂未写)
BOOL isGameOver = FALSE; //游戏是否结束(完整功能暂未写)

class Snake
{
public:
	int x; //保存当前坐标X
	int y; //保存当前坐标Y
	Snake* next; //保存下一个子节点
	static POINT pt;   //保存食物的坐标	
	static int Length; //保存所有节点的长度
	Snake() {}
	Snake(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	//绘图
	static void Draw(HWND hwnd, HDC hdc, Snake* snake)
	{
		if (snake == NULL)
			return;
		Snake* p = snake;
		p = p->next;//指向第一个节点

		HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

		//双缓冲贴图
		/*-----创建画布DC-----*/
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP bmp = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		SelectObject(hdcMem, bmp);
		/*-----创建画布DC-----*/

		/*------绘制身体图片-----*/
		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		SelectObject(hdcBuffer, hBitmap);
		/*------绘制身体图片-----*/

		/*-----绘制食物------*/
		HDC hdcFood = CreateCompatibleDC(hdc);
		HBITMAP hFoodBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		SelectObject(hdcFood, hFoodBmp);
		BitBlt(hdcMem, Snake::pt.x * 20, Snake::pt.y * 20, 20, 20, hdcFood, 0, 0, SRCCOPY);
		/*-----绘制食物完毕------*/

		/*-----绘制蛇身体------*/
		for (Snake* s = p;s != NULL;s = s->next)
			BitBlt(hdcMem, s->x * 20, s->y * 20, 20, 20, hdcBuffer, 0, 0, SRCCOPY);
		/*-----绘制蛇身体结束------*/

		BitBlt(hdc, 0, 0, clientWidth, clientHeight, hdcMem, 0, 0, SRCCOPY);//贴图

		/*------释放资源------*/
		DeleteObject(hBitmap);
		DeleteObject(bmp);
		DeleteObject(hFoodBmp);
		DeleteDC(hdcBuffer);
		DeleteDC(hdcMem);
		DeleteDC(hdcFood);
		/*------释放资源完毕------*/
	}
	//创建身体,身体长度
	static Snake* Create(POINT pt[], int length)
	{
		Snake* snake = new Snake();
		Snake* p = snake;
		Snake* s = NULL;
		for (int i = 0;i < length;i++)
		{
			s = new Snake(pt[i].x, pt[i].y);
			p->next = s;
			p = s;
		}
		s->next = NULL;
		Snake::Length = length + 1;//记录所有的结点包括头节点
		return snake;
	}
	//绘制游戏结束
	static void DrawGameOver(HWND hwnd)
	{
		isGameOver = TRUE;
		HDC hdc = GetDC(hwnd);
		RECT rect;
		rect.left = 200;
		rect.right = rect.left + 100;
		rect.top = 200;
		rect.bottom = rect.top + 100;
		::SetTextColor(hdc, RGB(0, 0, 0));
		DrawText(hdc, _T("游戏结束"), 4, &rect, DT_CENTER);
		ReleaseDC(hwnd, hdc);
	}
	//设置蛇头   vk:方向
	static void SetHeader(Snake* snake, int vk, HWND hwnd)
	{
		Snake* p = snake;
		p = p->next;   //记录蛇头的位置
		POINT pt;    //记录蛇头的坐标
		pt.x = p->x;
		pt.y = p->y;
		switch (vk)
		{
		case VK_LEFT:
			pt.x--;
			break;
		case VK_UP:
			pt.y--;
			break;
		case VK_RIGHT:
			pt.x++;
			break;
		case VK_DOWN:
			pt.y++;
			break;
		}

		BOOL isWall = (pt.x == 25 || pt.y == 25 || pt.x < 0 || pt.y < 0); //判断是否撞上墙
		if (!isCX)
		{
			if (isWall)
			{
				KillTimer(hwnd, TIMER); //游戏结束功能未写==!
				SetWindowText(hwnd, L"游戏结束");
				DrawGameOver(hwnd);
				return;
			}
		}
		else
		{
			if (isWall)
			{
				//穿墙
				switch (vk)
				{
				case VK_LEFT://左
					if (pt.x == -1)
						pt.x = 24;
					break;
				case VK_RIGHT:
					if (pt.x == 25)
						pt.x = 0;
					break;
				case VK_UP:
					if (pt.y == -1)
						pt.y = 24;
					break;
				case VK_DOWN:
					if (pt.y == 25)
						pt.y = 0;
					break;
				}
			}
		}
		Snake* p2 = snake;
		for (int j = 0;j < Snake::Length - 2;j++)//找到最后第二个节点
		{
			p2 = p2->next;
		}
		Snake* last = p2;
		p2 = p2->next; //获取最后第一个节点
		last->next = NULL;//将最后第二个的子节点设置为NULL
		p2->x = pt.x; //设置最后一个节点的坐标
		p2->y = pt.y;
		if (p->x == Snake::pt.x && p->y == Snake::pt.y)//判断是否撞上了食物		
		{
			Snake* food = new Snake(last->x, last->y);//最后两个节点重叠
			last->next = food;
			food->next = NULL;
			Snake::Length++;
			SetFood(snake);
		}
		//将最后一个节点放到头节点上
		Snake* t = snake;
		t = t->next; //指向第一个节点
		p2->next = t;
		snake->next = p2;
	}
	//设置食物(不能出现在蛇身上)
	static void SetFood(Snake* snake)
	{
	Label:
		Snake* p = snake;
		srand(GetTickCount64());
		int x = rand() % wallWidth;
		int y = rand() % wallHeight;
		while (p)
		{
			if (p->x == x && p->y == y) //食物出现不能在身体的某一处
			{
				goto Label;
			}
			p = p->next;
		}
		Snake::pt.x = x;
		Snake::pt.y = y;
	}
};
int Snake::Length = 0;
POINT Snake::pt;

LRESULT CALLBACK WndProc(
	HWND hwnd,  // handle to dialog box
	UINT msg,     // message
	WPARAM wParam, // first message parameter
	LPARAM lParam  // second message parameter
)
{
	static Snake* snake;
	static int VK_FLAG = VK_RIGHT;  //记录最后一次按下的值

	switch (msg)
	{
	case WM_CREATE:
	{
		POINT pt[] = { {4,0},{3,0},{2,0},{1,0},{0,0} };
		Snake::SetFood(snake);
		snake = Snake::Create(pt, 5);
		isGameOver = FALSE;
		SetTimer(hwnd, TIMER, 100, NULL);

		RECT rect;
		GetClientRect(hwnd, &rect);
		RECT rect2;
		GetWindowRect(hwnd, &rect2);
		int width = rect2.right - rect2.left - (rect.right - rect.left);
		int height = rect2.bottom - rect2.top - (rect.bottom - rect.top);
		MoveWindow(hwnd, 300, 100, width + clientWidth, height + clientHeight, FALSE);
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		Snake::Draw(hwnd, hdc, snake);
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_KEYDOWN:
	{
		int n = (int)wParam;
		if (n >= 37 && n <= 40)
		{
			VK_FLAG = n;
		}
	}
	break;
	case WM_TIMER:
	{
		if (wParam == TIMER)
		{
			HDC hdc = GetDC(hwnd);
			Snake::SetHeader(snake, VK_FLAG, hwnd); //设置蛇头位置
			Snake::Draw(hwnd, hdc, snake);     //重新绘制
			ReleaseDC(hwnd, hdc);
		}
	}
	break;
	case WM_CLOSE:
		ExitProcess(0);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR str, int n)
{
	TCHAR name[] = _T("贪吃蛇");
	WNDCLASSEX wndClass;
	wndClass.cbClsExtra = 0;
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.cbWndExtra = 0;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.hCursor = LoadCursor(hInst, IDC_ARROW);
	wndClass.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wndClass.hIconSm = NULL;
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = WndProc;
	wndClass.lpszClassName = name;
	wndClass.lpszMenuName = NULL;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClassEx(&wndClass);
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, name, _T("贪吃蛇"),
		WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX), 300, 100,
		clientWidth, clientHeight, NULL, NULL, hInst, NULL);
	ShowWindow(hwnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return 0;
}