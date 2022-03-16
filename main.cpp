#include <Windows.h>
#include <tchar.h>
#include "resource.h"

#define TIMER 1
const int clientWidth = 500;  //�ͻ�����
const int clientHeight = 500; //�ͻ�����
const int wallWidth = 25;     //ǽ�Ŀ�
const int wallHeight = 25;    //ǽ�ĸ�

BOOL isCX = TRUE;        //�Ƿ��ܴ�ǽ(����������δд)
BOOL isGameOver = FALSE; //��Ϸ�Ƿ����(����������δд)

class Snake
{
public:
	int x; //���浱ǰ����X
	int y; //���浱ǰ����Y
	Snake* next; //������һ���ӽڵ�
	static POINT pt;   //����ʳ�������	
	static int Length; //�������нڵ�ĳ���
	Snake() {}
	Snake(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	//��ͼ
	static void Draw(HWND hwnd, HDC hdc, Snake* snake)
	{
		if (snake == NULL)
			return;
		Snake* p = snake;
		p = p->next;//ָ���һ���ڵ�

		HINSTANCE hInst = (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE);

		//˫������ͼ
		/*-----��������DC-----*/
		HDC hdcMem = CreateCompatibleDC(hdc);
		HBITMAP bmp = CreateCompatibleBitmap(hdc, clientWidth, clientHeight);
		SelectObject(hdcMem, bmp);
		/*-----��������DC-----*/

		/*------��������ͼƬ-----*/
		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		SelectObject(hdcBuffer, hBitmap);
		/*------��������ͼƬ-----*/

		/*-----����ʳ��------*/
		HDC hdcFood = CreateCompatibleDC(hdc);
		HBITMAP hFoodBmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP2));
		SelectObject(hdcFood, hFoodBmp);
		BitBlt(hdcMem, Snake::pt.x * 20, Snake::pt.y * 20, 20, 20, hdcFood, 0, 0, SRCCOPY);
		/*-----����ʳ�����------*/

		/*-----����������------*/
		for (Snake* s = p;s != NULL;s = s->next)
			BitBlt(hdcMem, s->x * 20, s->y * 20, 20, 20, hdcBuffer, 0, 0, SRCCOPY);
		/*-----�������������------*/

		BitBlt(hdc, 0, 0, clientWidth, clientHeight, hdcMem, 0, 0, SRCCOPY);//��ͼ

		/*------�ͷ���Դ------*/
		DeleteObject(hBitmap);
		DeleteObject(bmp);
		DeleteObject(hFoodBmp);
		DeleteDC(hdcBuffer);
		DeleteDC(hdcMem);
		DeleteDC(hdcFood);
		/*------�ͷ���Դ���------*/
	}
	//��������,���峤��
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
		Snake::Length = length + 1;//��¼���еĽ�����ͷ�ڵ�
		return snake;
	}
	//������Ϸ����
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
		DrawText(hdc, _T("��Ϸ����"), 4, &rect, DT_CENTER);
		ReleaseDC(hwnd, hdc);
	}
	//������ͷ   vk:����
	static void SetHeader(Snake* snake, int vk, HWND hwnd)
	{
		Snake* p = snake;
		p = p->next;   //��¼��ͷ��λ��
		POINT pt;    //��¼��ͷ������
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

		BOOL isWall = (pt.x == 25 || pt.y == 25 || pt.x < 0 || pt.y < 0); //�ж��Ƿ�ײ��ǽ
		if (!isCX)
		{
			if (isWall)
			{
				KillTimer(hwnd, TIMER); //��Ϸ��������δд==!
				SetWindowText(hwnd, L"��Ϸ����");
				DrawGameOver(hwnd);
				return;
			}
		}
		else
		{
			if (isWall)
			{
				//��ǽ
				switch (vk)
				{
				case VK_LEFT://��
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
		for (int j = 0;j < Snake::Length - 2;j++)//�ҵ����ڶ����ڵ�
		{
			p2 = p2->next;
		}
		Snake* last = p2;
		p2 = p2->next; //��ȡ����һ���ڵ�
		last->next = NULL;//�����ڶ������ӽڵ�����ΪNULL
		p2->x = pt.x; //�������һ���ڵ������
		p2->y = pt.y;
		if (p->x == Snake::pt.x && p->y == Snake::pt.y)//�ж��Ƿ�ײ����ʳ��		
		{
			Snake* food = new Snake(last->x, last->y);//��������ڵ��ص�
			last->next = food;
			food->next = NULL;
			Snake::Length++;
			SetFood(snake);
		}
		//�����һ���ڵ�ŵ�ͷ�ڵ���
		Snake* t = snake;
		t = t->next; //ָ���һ���ڵ�
		p2->next = t;
		snake->next = p2;
	}
	//����ʳ��(���ܳ�����������)
	static void SetFood(Snake* snake)
	{
	Label:
		Snake* p = snake;
		srand(GetTickCount64());
		int x = rand() % wallWidth;
		int y = rand() % wallHeight;
		while (p)
		{
			if (p->x == x && p->y == y) //ʳ����ֲ����������ĳһ��
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
	static int VK_FLAG = VK_RIGHT;  //��¼���һ�ΰ��µ�ֵ

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
			Snake::SetHeader(snake, VK_FLAG, hwnd); //������ͷλ��
			Snake::Draw(hwnd, hdc, snake);     //���»���
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
	TCHAR name[] = _T("̰����");
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
	HWND hwnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, name, _T("̰����"),
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