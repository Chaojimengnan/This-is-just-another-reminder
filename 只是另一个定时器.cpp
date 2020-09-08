// 只是另一个定时器.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "只是另一个定时器.h"

#define MAX_LOADSTRING 100
#define SAVEFILENAME "data.dat"
#define SCROLLWINDOWCALLNAME L"MYSCROLL"
#define WM_TRAY (WM_USER + 100)

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 类名
NOTIFYICONDATA nid;		//托盘属性
HMENU hMenu;			//托盘菜单


static LISTCUSTIMER structtimer[9];             // 0-6 一周计时器，7 长期计时器，8 定制计时器

// 此代码模块中包含的函数的前向声明:
INT_PTR CALLBACK    EditBox(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK ScollProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK EditTableBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool OverSaveFile(LISTCUSTIMER listtimer[], const char filename[], const char mode[]);
bool OverLoadFile(LISTCUSTIMER listtimer[], const char filename[], const char mode[]);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDS_MY, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MY));

    RegisterClassEx(&wcex);
    wcex.cbWndExtra = 8;
    wcex.lpfnWndProc = ScollProc;
    wcex.lpszClassName = SCROLLWINDOWCALLNAME;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.hIcon = NULL;
    RegisterClassEx(&wcex);
    hInst = hInstance; 

    // 加载文件
    OverLoadFile(structtimer, SAVEFILENAME, "rb");
    


    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU| WS_DLGFRAME| WS_MINIMIZEBOX,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        return FALSE;
    }
    
    // 设置托盘内容
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = IDI_MY;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
    nid.uCallbackMessage = WM_TRAY;
    nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY));

    StringCchPrintf(nid.szTip, 128, szTitle);

    // 设置菜单内容
    hMenu = CreatePopupMenu();
    AppendMenu(hMenu, MF_STRING, ID_SHOW, TEXT("显示"));
    AppendMenu(hMenu, MF_STRING, ID_EXIT, TEXT("退出"));
    
    Shell_NotifyIcon(NIM_ADD, &nid);
    

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);



    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    return (int) msg.wParam;
}

#define IDC_EDITTIMER 75    // 编辑计划   按钮ID
#define IDC_CREATETIMER 76  // 制定定时器 按钮ID
#define IDC_SHOWWEEK 77     // 一周计划   按钮ID
#define IDC_SHOWLONG 78     // 长期计划   按钮ID

#define IDC_SCROLLMAIN 79   // 主显示表
#define IDC_SCROLLSUB 80    // 副显示表
#define IDSHOWWEEK 51       // 显示一周表
#define IDSHOWLONG 52       // 显示长期表
#define IDSHOWCUS  53       // 显示自定义表
#define CUS_UPDATECUS 54    // 自定义信息，更新自定义窗口的数据
#define CUS_UPDATEMAIN 55   // 自定义信息，更新主窗口的数据
//#define MAXTIMERPERDAY 50   // 每天最多有几个定时器
//#define MAXTIMERLONG 100    // 最多有几个长期定时器


int changeWeekDay(int value)
{
    return (value == 0) ? 6 : value-1;
}


void ShowTrayMsg(TCHAR* name,TCHAR* string)
{
    StringCchPrintf(nid.szInfoTitle, 64, name);
    StringCchPrintf(nid.szInfo, 256, string);
    nid.uTimeout = 5000;
    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

#define colorONEWEEK RGB(0, 255, 0)
#define colorDAY RGB(255, 0, 0)
#define colorDAYONEWEEK RGB(255, 255, 0)
#define colorLONG RGB(0,128,192)
#define colorEMPTY RGB(255, 255, 255)

void setNowColor(HDC hdc, bool isday,bool islong,bool isoneweek)
{
    if (isday and isoneweek)
    {
        SetBkColor(hdc, colorDAYONEWEEK);
    }
    else if (isday and !isoneweek)
    {
        SetBkColor(hdc, colorDAY);
    }
    else if (!isday and isoneweek)
    {
        SetBkColor(hdc, colorONEWEEK);
    }
    else if (islong)
    {
        SetBkColor(hdc, colorLONG);
    }
    else {
        SetBkColor(hdc, colorEMPTY);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    TCHAR tempbuffer[30];
    // 字体宽和高
    static int cxChar, cyChar;
    // 客户区宽和高
    static int cxClient, cyClient;
    // 日期
    static SYSTEMTIME startTime;
    // 自定义时间
    static CUSTIME startTimeCus;
    // 自定义时间对应的秒
    static int startTimerToSec;
    // 今天星期几
    static int weekIndex;
    switch (message)
    {
    case WM_CREATE:
        // 初始化总计时器
        
        SendMessage(hWnd, WM_TIMER, -1, 0);
        cxChar = LOWORD(GetDialogBaseUnits());
        cyChar = HIWORD(GetDialogBaseUnits());

        GetWindowRect(hWnd, &rect);

        MoveWindow(hWnd, rect.left, rect.top, cxChar * 137, cyChar * 35, TRUE);

        GetClientRect(hWnd, &rect);
        cxClient = rect.right;
        cyClient = rect.bottom;

        // 一周计划按钮
        CreateWindow(TEXT("BUTTON"), TEXT("一周计划"), WS_CHILD | WS_VISIBLE | BS_BOTTOM|BS_VCENTER| WS_BORDER,
            cxChar * 8, cyChar * 2, cxChar * 20, cyChar * 3, 
            hWnd, (HMENU)IDC_SHOWWEEK, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        // 长期计划按钮
        CreateWindow(TEXT("BUTTON"), TEXT("长期计划"), WS_CHILD | WS_VISIBLE | BS_BOTTOM | BS_VCENTER | WS_BORDER,
            cxChar * 44, cyChar * 2, cxChar * 20, cyChar * 3,
            hWnd, (HMENU)IDC_SHOWLONG, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        // 制定定时器按钮
        CreateWindow(TEXT("BUTTON"), TEXT("制定定时器"), WS_CHILD | WS_VISIBLE | BS_BOTTOM | BS_VCENTER | WS_BORDER,
            cxChar * 112, cyChar * 5, cxChar * 20, cyChar * 3,
            hWnd, (HMENU)IDC_CREATETIMER, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );
        // 编辑计划按钮
        CreateWindow(TEXT("BUTTON"), TEXT("编辑计划"), WS_CHILD | WS_VISIBLE | BS_BOTTOM | BS_VCENTER | WS_BORDER,
            cxChar * 80, cyChar * 2, cxChar * 20, cyChar * 3,
            hWnd, (HMENU)IDC_EDITTIMER, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );

        // 主表子窗口 14，14，14，！！！！
        CreateWindow(SCROLLWINDOWCALLNAME, TEXT("主表"), WS_CHILD | WS_VISIBLE| WS_BORDER,
            cxChar * 4, cyChar * 6.5, cxChar * 103, cyChar * 24,
            hWnd, (HMENU)IDC_SCROLLMAIN, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );
        // 发送显示一周表的信息
        SendMessageW(GetDlgItem(hWnd, IDC_SCROLLMAIN), WM_COMMAND, IDSHOWWEEK, NULL);

        // 副表子窗口
        CreateWindow(SCROLLWINDOWCALLNAME, TEXT("副表"), WS_CHILD | WS_VISIBLE| WS_BORDER,
            cxChar * 112, cyChar * 10, cxChar * 19, cyChar * 20,
            hWnd, (HMENU)IDC_SCROLLSUB, ((LPCREATESTRUCT)lParam)->hInstance,
            NULL
        );
        // 发送显示自定义表的信息
        SendMessageW(GetDlgItem(hWnd, IDC_SCROLLSUB), WM_COMMAND, IDSHOWCUS, NULL);

        break;
    case WM_TRAY:
        switch (lParam)
        {

            case WM_RBUTTONDOWN:
            {
                //获取鼠标坐标
                POINT pt; GetCursorPos(&pt);

                //解决在菜单外单击左键菜单不消失的问题
                SetForegroundWindow(hWnd);

                //使菜单某项变灰
                //EnableMenuItem(hMenu, ID_SHOW, MF_GRAYED);	

                //显示并获取选中的菜单（当点击到这个托盘就显示这个菜单，然后根据这个菜单的返回来做出变化）
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd,
                    NULL);
                if (cmd == ID_EXIT)
                    PostMessage(hWnd, WM_DESTROY, NULL, NULL);
                if (cmd == ID_SHOW)
                    ShowWindow(hWnd, SW_SHOW);
                break;
            }
            case WM_LBUTTONDBLCLK:
                ShowWindow(hWnd, SW_SHOW);
                break;

        }
        break;
    case WM_TIMER:
    {
        if (wParam != -1)
        {
            GetLocalTime(&startTime);
            int OutterIndex = wParam/100;
            int InnerIndex = wParam- OutterIndex*100;
            /*StringCchPrintf(tempbuffer, 30, L"%d,%d", OutterIndex, InnerIndex);
            MessageBox(hWnd, tempbuffer, L"看一眼", NULL);*/
            // 更新当前时间
            startTimeCus.hou = startTime.wHour;
            startTimeCus.min = startTime.wMinute;
            startTimeCus.sec = startTime.wSecond;
            startTimerToSec = CusTimeToSec(startTimeCus);
            weekIndex = changeWeekDay(startTime.wDayOfWeek);
            MessageBox(hWnd, structtimer[OutterIndex][InnerIndex].remindstring, structtimer[OutterIndex][InnerIndex].name, MB_SYSTEMMODAL| MB_SETFOREGROUND);
            ShowTrayMsg(structtimer[OutterIndex][InnerIndex].name, structtimer[OutterIndex][InnerIndex].remindstring);
            KillTimer(hWnd,wParam);

            // 当是日循环并且当前时间在之中
            if(structtimer[OutterIndex][InnerIndex].isday and 
                startTimerToSec>= CusTimeToSec(structtimer[OutterIndex][InnerIndex].uppertime)and 
                (startTimerToSec < CusTimeToSec(structtimer[OutterIndex][InnerIndex].lowertime)))
            { 
                SetTimer(hWnd, wParam, CusTimeToSec(structtimer[OutterIndex][InnerIndex].aimtime)*1000, NULL);
            }

            // 当不满足前面的要求并且不是周循环则删除
            else if (!structtimer[OutterIndex][InnerIndex].isoneweek) {
                structtimer[OutterIndex].erase(structtimer[OutterIndex].begin() + InnerIndex, structtimer[OutterIndex].begin() + InnerIndex + 1);
            }

        }
        else {
            // 先杀掉第二天的定时器
            KillTimer(hWnd, -1);
            GetLocalTime(&startTime);
            CUSTIME finalTime = { 23,59,59 };
            startTimeCus.hou = -startTime.wHour;
            startTimeCus.min = -startTime.wMinute;
            startTimeCus.sec = -startTime.wSecond;
            CUSTIME result = CusTimerAddCus(finalTime, startTimeCus);

            startTimeCus.hou = startTime.wHour;
            startTimeCus.min = startTime.wMinute;
            startTimeCus.sec = startTime.wSecond;
            startTimerToSec = CusTimeToSec(startTimeCus);
            weekIndex = changeWeekDay(startTime.wDayOfWeek);
            /*StringCchPrintf(tempbuffer, 30, L"%d:%d:%d", result.hou, result.min, result.sec);
            MessageBox(hWnd, tempbuffer,L"HHE1",NULL);*/
            SetTimer(hWnd, -1, (CusTimeToSec(result) + 2)*1000, NULL);

            // 普通星期X的定时器设置
            for (int i = 0; i < structtimer[weekIndex].size(); i++)
            {
                // 日循环特别处理，当当前时间早于开始时间时 设置上限时间为间隔，当再次获得信息时候再转为瞄准时间
                // 当当前时间处于开始时间和结束时间之间时 直接设置瞄准时间为间隔，判断当前时间如果比结束时间晚则killtimer
                // 当当前时间晚于结束时间时 判断是不是周循环，不是则直接删除
                if (structtimer[weekIndex][i].isday)
                {
                    if (startTimerToSec < CusTimeToSec(structtimer[weekIndex][i].uppertime))
                    {
                        SetTimer(hWnd, weekIndex * 100 + i, (CusTimeToSec(structtimer[weekIndex][i].uppertime)- startTimerToSec+2) * 1000, NULL);
                    }
                    else if (startTimerToSec >= CusTimeToSec(structtimer[weekIndex][i].uppertime) and
                        startTimerToSec < CusTimeToSec(structtimer[weekIndex][i].lowertime))
                    {
                        SetTimer(hWnd, weekIndex * 100 + i, CusTimeToSec(structtimer[weekIndex][i].aimtime) * 1000, NULL);
                    }
                    else {
                        if (!structtimer[weekIndex][i].isoneweek)
                        {
                            structtimer[weekIndex].erase(structtimer[weekIndex].begin() + i, structtimer[weekIndex].begin() + i + 1);
                        }
                    }
                } // 普通的不循环定时器，判断当前时间是否早于瞄准时间，如果是直接设置瞄准时间为间隔，再次获得信息之后直接killtimer、
                // 如果晚于当前时间，判断是不是周循环，不是则直接删除
                else {
                    if (startTimerToSec < CusTimeToSec(structtimer[weekIndex][i].aimtime))
                    {
                        SetTimer(hWnd, weekIndex * 100 + i, (CusTimeToSec(structtimer[weekIndex][i].aimtime)- startTimerToSec) * 1000, NULL);
                    }
                    else {
                        if (!structtimer[weekIndex][i].isoneweek)
                        {
                            structtimer[weekIndex].erase(structtimer[weekIndex].begin() + i, structtimer[weekIndex].begin() + i + 1);
                        }
                    }
                }
            }

            // 对于长时间定时器，先判断年月日是否和当前时间相同，如果相同，就判断当前时间是否早于瞄准时间
            // 如果晚于了瞄准时间，则删除
            for (int  i = 0; i < structtimer[7].size(); i++)
            {
                INT64 nowResult = (INT64)startTime.wYear * 10000000000 + (INT64)startTime.wMonth * 100000000
                    + (INT64)startTime.wDay * 1000000 + startTime.wHour * 10000 + startTime.wMinute * 100 + startTime.wSecond;
                INT64 nowResult2 = (INT64)structtimer[7][i].year * 10000000000 + (INT64)structtimer[7][i].month * 100000000
                    + (INT64)structtimer[7][i].day * 1000000 + structtimer[7][i].aimtime.hou * 10000 + structtimer[7][i].aimtime.min * 100 + structtimer[7][i].aimtime.sec;

                // 当当前时间大于长时间定时器的瞄准时间则除掉该定时器
                if (nowResult >= nowResult2)
                {
                    structtimer[7].erase(structtimer[7].begin() + i, structtimer[7].begin() + i + 1);
                    continue;
                }
                if (nowResult < nowResult2 and (startTime.wYear == structtimer[7][i].year and startTime.wMonth == structtimer[7][i].month and
                    startTime.wDay == structtimer[7][i].day))
                {
                    SetTimer(hWnd, 700 + i, (CusTimeToSec(structtimer[7][i].aimtime)- startTimerToSec + 2) * 1000, NULL);
                }
            }
        }
        SendMessageW(GetDlgItem(hWnd, IDC_SCROLLMAIN), WM_COMMAND, IDSHOWWEEK, NULL);
        

    }
        //InvalidateRect(hWnd, NULL, FALSE);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            switch (wmId)
            {
            case IDC_CREATETIMER:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITBOX), hWnd, EditBox);
                break;
            case IDC_EDITTIMER:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_EDITTIMERBOX), hWnd, EditTableBox);
                break;
            case IDC_SHOWWEEK:
                SendMessageW(GetDlgItem(hWnd, IDC_SCROLLMAIN), WM_COMMAND, IDSHOWWEEK, NULL);
                break;
            case IDC_SHOWLONG:
                SendMessageW(GetDlgItem(hWnd, IDC_SCROLLMAIN), WM_COMMAND, IDSHOWLONG, NULL);
                break;
            case CUS_UPDATECUS:
                SendMessageW(GetDlgItem(hWnd, IDC_SCROLLSUB), WM_COMMAND, IDSHOWCUS, NULL);
            case CUS_UPDATEMAIN:
                SendMessageW(GetDlgItem(hWnd, IDC_SCROLLMAIN), WM_COMMAND, IDSHOWWEEK, NULL);
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            SetBkColor(hdc, colorDAYONEWEEK);
            TextOut(hdc, 950, 50, L"周循环&日循环", 7);
            SetBkColor(hdc, colorLONG);
            TextOut(hdc, 1000, 20, L"长时间", 3);
            SetBkColor(hdc, colorDAY);
            TextOut(hdc, 950, 20, L"日循环", 3);
            SetBkColor(hdc, colorONEWEEK);
            TextOut(hdc, 900, 20, L"周循环", 3);
            SetBkColor(hdc, colorEMPTY);
            TextOut(hdc, 900, 50, L"都不是", 3);
            /*TCHAR temp[50];
            size_t len;*/
            /*StringCchPrintf(temp, 50, TEXT("%d:%d"), sizeof(long),8);
            StringCchLength(temp, 50, &len);

            TextOut(hdc, 10, 10, temp, len);*/
                //mytime = CusTimerAddSec(mytime, 15);
            
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_CLOSE:
        /*StringCchPrintf(tempbuffer, 30, L"%d", MessageBox(hWnd, L"不转入后台直接关闭吗？", L"提醒", MB_YESNOCANCEL | MB_ICONINFORMATION));
        MessageBoxW(hWnd, tempbuffer, L"呵呵", NULL);*/
        ShowWindow(hWnd, SW_HIDE);
        
            
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        OverSaveFile(structtimer, SAVEFILENAME, "wb");
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int numiterator(int start, int times, int distance)
{
    return start + times * distance;
}

WCHAR weekString[7][4] = { L"星期一",L"星期二",L"星期三",L"星期四",L"星期五",L"星期六",L"星期天" };
#define MAXTIMERSIZE 100

LRESULT CALLBACK ScollProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;
    // 是一周计划的模式还是长期计划还是自定义表的模式，0，1，2分别对应其中
    //static int playMode = 1;

    

    // 字体宽和高
    static int cxChar, cyChar;
    // 客户区宽和高
    static int cxClient, cyClient;
    SCROLLINFO si;
    int temp;


    int FirstLine;          // 失效区域的第一行（需重绘的第一行）
    int LastLine;           // 失效区域的最后一行（（需重绘的最后一行））

    static int yPos;                    // 当前滚条位置

    static int ulScrollLines;           // 每滑动一次鼠标滚轮移动的行数
    static int iDeltaPerLine;           // 每行的滑轮Delta
    static int iAccumDelta;             // 合计的滑轮滚动delta
    //static int size = structtimer->size();


    switch (message)
    {
    case WM_CREATE:
        cxChar = LOWORD(GetDialogBaseUnits());
        cyChar = 2*HIWORD(GetDialogBaseUnits());
        SetWindowLongPtr(hWnd, 0, 0);
        
        //hdc = GetDC(hWnd);
        //SetBkMode(hdc, TRANSPARENT);
        //SelectObject(hdc, GetStockObject(NULL_BRUSH));
        //ReleaseDC(hWnd, hdc);

        GetClientRect(hWnd, &rect);
        cxClient = rect.right;
        cyClient = rect.bottom;

        si.cbSize = sizeof(si);
        si.fMask = SIF_RANGE | SIF_PAGE;
        si.nMin = 0;
        si.nMax = MAXTIMERSIZE - 1;
        si.nPage = (cyClient) / cyChar;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

        


    case WM_SETTINGCHANGE:
        SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &ulScrollLines, 0);

        if (ulScrollLines)
        {
            iDeltaPerLine = WHEEL_DELTA / ulScrollLines;
        }
        else {
            iDeltaPerLine = 0;
        }
        break;

    case WM_MOUSEWHEEL:
        if (iDeltaPerLine == 0) // 如果不能使用滚轮
        {
            break;
        }
        iAccumDelta += GET_WHEEL_DELTA_WPARAM(wParam);  // 获取鼠标滚动的delta

        // 如果是往上
        while (iAccumDelta >= iDeltaPerLine)
        {
            SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, 0);
            iAccumDelta -= iDeltaPerLine;
        }

        // 如果是往下
        while (iAccumDelta <= -iDeltaPerLine)
        {
            SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
            iAccumDelta += iDeltaPerLine;
        }
        break;
    case WM_KEYDOWN:
        // 我们没有必要再把之前写滚轮移动的代码再写一次，直接使用sendmessage函数发滚动信息即可
        switch (wParam)
        {
            // 下方向键
        case VK_DOWN:
            // 发送信息WM_VSCROLL，wParam是下一行
            SendMessageW(hWnd, WM_VSCROLL, SB_LINEDOWN, 0);
            break;
            // 上方向键
        case VK_UP:
            SendMessageW(hWnd, WM_VSCROLL, SB_LINEUP, 0);
            break;
            // HOME键回到顶部
        case VK_HOME:
            SendMessageW(hWnd, WM_VSCROLL, SB_TOP, 0);
            break;
            // END键降到底部
        case VK_END:
            SendMessageW(hWnd, WM_VSCROLL, SB_BOTTOM, 0);
            break;
        default:
            break;
        }
        break;


    case WM_VSCROLL:

        // 获得垂直滚动条的所有信息
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &si);

        yPos = si.nPos;

        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            si.nPos -= 1;
            break;
        case SB_LINEDOWN:
            si.nPos += 1;
            break;
        case SB_PAGEUP:
            si.nPos -= si.nPage;
            break;
        case SB_PAGEDOWN:
            si.nPos += si.nPage;
            break;
        case SB_THUMBPOSITION:
            si.nPos = HIWORD(wParam);
            break;
        case SB_THUMBTRACK:
            si.nPos = si.nTrackPos;
            break;
        case SB_TOP:
            si.nPos = si.nMin;
            break;
        case SB_BOTTOM:
            si.nPos = si.nMax;
            break;
        }

        // 设置滚动条滑块的新位置
        si.fMask = SIF_POS;
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

        // 获得滚动条滑块，由于窗口调整，它可能不是同一个值
        GetScrollInfo(hWnd, SB_VERT, &si);

        // 与此前的保存的值进行比较，如果不同则滚动窗口
        if (si.nPos != yPos)
        {
            ScrollWindow(hWnd, 0, cyChar * (yPos - si.nPos),NULL , NULL);
            //UpdateWindow(hWnd);
        }

        break;
    case WM_COMMAND:
    {
        switch (wParam)
        {
        case IDSHOWWEEK:
            //MessageBox(hWnd, TEXT("显示一周表"), TEXT("显示一周表"), MB_OK);
            SetWindowLongPtr(hWnd, 0, 0);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case IDSHOWLONG:
            //MessageBox(hWnd, TEXT("显示长期表"), TEXT("显示长期表"), MB_OK);
            SetWindowLongPtr(hWnd, 0, 1);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        case IDSHOWCUS:
            //MessageBox(hWnd, TEXT("显示自定义表"), TEXT("显示自定义表"), MB_OK);
            SetWindowLongPtr(hWnd, 0, 2);
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        default:
            break;
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        size_t len;

        // 获得垂直滚动条的位置
        si.cbSize = sizeof(si);
        si.fMask = SIF_POS;
        GetScrollInfo(hWnd, SB_VERT, &si);
        yPos = si.nPos;

        TCHAR szBuffer[128];
        GetClientRect(hWnd, &rect);

        // 计算需要重绘的区域
        FirstLine = max(0, yPos + ps.rcPaint.top / cyChar);
        LastLine = yPos + ps.rcPaint.bottom / cyChar;
        /*LISTCUSTIMER::iterator m;
        int i;*/
        SetBkColor(hdc, RGB(255, 255, 255));
        switch (GetWindowLongPtr(hWnd, 0))
        {
        case 0:
            SetTextAlign(hdc, TA_CENTER);
            if (FirstLine == 0)
            {
                FirstLine += 1;
                for (size_t i = 0; i < 7; i++)
                {
                    temp = numiterator(8, i, 14) * cxChar;
                    TextOut(hdc, temp, cyChar * (-yPos) + cyChar / 4, weekString[i], 3);
                }
            }
            for (size_t t = 0; t < 7; t++)
            {
                LastLine = min(yPos + ps.rcPaint.bottom / cyChar, structtimer[t].size());
                for (int i = FirstLine; i <= LastLine; i++)
                {
                    setNowColor(hdc, structtimer[t][i - 1].isday, structtimer[t][i - 1].islong, structtimer[t][i - 1].isoneweek);
                    temp = numiterator(8, t, 14) * cxChar;
                    StringCchPrintf(szBuffer, 128, structtimer[t][i-1].name);
                    StringCchLength(szBuffer, 128, &len);
                    TextOut(hdc, temp, cyChar * (i - yPos), szBuffer, len);
                    StringCchPrintf(szBuffer, 128, TEXT("%d:%d:%d"), structtimer[t][i-1].aimtime.hou, structtimer[t][i-1].aimtime.min, structtimer[t][i-1].aimtime.sec);
                    StringCchLength(szBuffer, 128, &len);
                    TextOut(hdc, temp, cyChar * (i - yPos) + cyChar / 2, szBuffer, len);
                    /*temp = numiterator(1, t, 14) * cxChar;
                    MoveToEx(hdc, temp, cyChar * (i - yPos), NULL);
                    LineTo(hdc, temp, cyChar * (i - yPos + 1));*/
                }
                /*temp = numiterator(1, 7, 14) * cxChar;
                MoveToEx(hdc, temp, cyChar * (LastLine - yPos), NULL);
                LineTo(hdc, temp, cyChar * (LastLine - yPos + 1));
                MoveToEx(hdc, cxChar, cyChar * (LastLine - yPos), NULL);
                LineTo(hdc, 99 * cxChar, cyChar * (LastLine - yPos));*/

            }
        
            MoveToEx(hdc, cxChar, cyChar* (LastLine+1 - yPos), NULL);
            LineTo(hdc, 99 * cxChar, cyChar* (LastLine+1 - yPos));
            break;
        case 1:
            SetTextAlign(hdc, TA_CENTER);
            LastLine = min(LastLine, structtimer[7].size());
            for (int i = FirstLine; i < LastLine; i++)
            {
                setNowColor(hdc, structtimer[7][i].isday, structtimer[7][i].islong, structtimer[7][i].isoneweek);
                StringCchPrintf(szBuffer, 128, structtimer[7][i].name);
                StringCchLength(szBuffer, 128, &len);
                TextOut(hdc, 8 * cxChar, cyChar * (i - yPos), szBuffer, len);
                StringCchPrintf(szBuffer, 128, TEXT("%d:%d:%d"), structtimer[7][i].aimtime.hou, structtimer[7][i].aimtime.min, structtimer[7][i].aimtime.sec);
                StringCchLength(szBuffer, 128, &len);
                TextOut(hdc, 8 * cxChar, cyChar * (i - yPos) + cyChar / 2, szBuffer, len);
                /*MoveToEx(hdc, cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, cxChar, cyChar * (i - yPos + 1));
                MoveToEx(hdc, 15 * cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, 15 * cxChar, cyChar * (i - yPos + 1));
                MoveToEx(hdc, cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, 15 * cxChar, cyChar * (i - yPos));*/
            }
            /*MoveToEx(hdc, cxChar, cyChar * (LastLine - yPos), NULL);
            LineTo(hdc, 15 * cxChar, cyChar * (LastLine - yPos));*/
            break;
        case 2:
            SetTextAlign(hdc, TA_CENTER);
            LastLine = min(LastLine, structtimer[8].size());
            for (int i = FirstLine; i < LastLine; i++)
            {
                setNowColor(hdc, structtimer[8][i].isday, structtimer[8][i].islong, structtimer[8][i].isoneweek);
                StringCchPrintf(szBuffer, 128, structtimer[8][i].name);
                StringCchLength(szBuffer, 128, &len);
                TextOut(hdc, 8 * cxChar, cyChar * (i - yPos), szBuffer, len);
                StringCchPrintf(szBuffer, 128, TEXT("%d:%d:%d"), structtimer[8][i].aimtime.hou, structtimer[8][i].aimtime.min, structtimer[8][i].aimtime.sec);
                StringCchLength(szBuffer, 128, &len);
                TextOut(hdc, 8 * cxChar, cyChar * (i - yPos) + cyChar / 2, szBuffer, len);
                /*MoveToEx(hdc, cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, cxChar, cyChar * (i - yPos + 1));
                MoveToEx(hdc, 15 * cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, 15 * cxChar, cyChar * (i - yPos + 1));
                MoveToEx(hdc, cxChar, cyChar * (i - yPos), NULL);
                LineTo(hdc, 15 * cxChar, cyChar * (i - yPos));*/
            }
            /*MoveToEx(hdc, cxChar, cyChar * (LastLine - yPos), NULL);
            LineTo(hdc, 15 * cxChar, cyChar * (LastLine - yPos));*/
            break;
        default:
            break;
        }
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool CheckInput()
{
    return false;
}

// 启动或禁用日循环项
void dayGroupSwitch(HWND hWnd,int tbool)
{
    EnableWindow(GetDlgItem(hWnd, IDC_DAYMACRO), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_UPPERSTRING), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_LOWERSTRING), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_UPPERHOU), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_UPPERMIN), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_UPPERSEC), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_LOWERHOU), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_LOWERMIN), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_LOWERSEC), tbool);
}

// 启动或禁用长时间项
void longGroupSwitch(HWND hWnd, int tbool)
{

    EnableWindow(GetDlgItem(hWnd, IDC_LONGMACRO), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_YEAR), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_MONTH), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_DAY), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_DAYSTRING2), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_MONTHSTRING), tbool);
    EnableWindow(GetDlgItem(hWnd, IDC_DAYSTRING), tbool);
}

TCHAR mouthString[12][4] = { L"一月",L"二月",L"三月",L"四月",L"五月",L"六月",L"七月",L"八月",L"九月",L"十月",L"十一月",L"十二月" };
TCHAR dayString[31][3] = { L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"11",L"12",L"13",L"14",L"15",L"16",L"17",L"18",
L"19",L"20",L"21",L"22",L"23",L"24",L"25",L"26",L"27",L"28",L"29",L"30",L"31" };

TCHAR minsecString[60][3] = { L"0",L"1",L"2",L"3",L"4",L"5",L"6",L"7",L"8",L"9",L"10",L"11",L"12",L"13",L"14",L"15",L"16",L"17",L"18",
L"19",L"20",L"21",L"22",L"23",L"24",L"25",L"26",L"27",L"28",L"29",L"30",L"31",L"32",L"33",L"34",L"35",L"36",L"37",L"38",L"39",L"40",
L"41",L"42",L"43",L"44",L"45",L"46",L"47",L"48",L"49",L"50",L"51",L"52",L"53",L"54",L"55",L"56",L"57",L"58",L"59" };

TCHAR yearString[40][5] = { L"2020",L"2021",L"2022",L"2023",L"2024",L"2025",L"2026",L"2027",L"2028",L"2029",
L"2030",L"2031",L"2032",L"2033",L"2034",L"2035",L"2036",L"2037",L"2038",L"2039",
L"2040",L"2041",L"2042",L"2043",L"2044",L"2045",L"2046",L"2047",L"2048",L"2049",
L"2050",L"2051",L"2052",L"2053",L"2054",L"2055",L"2056",L"2057",L"2058",L"2059"
};

bool IsLeapYear(int Year)
{
    if ((Year % 4 == 0 && Year % 100 != 0) || Year % 400 == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// 判断是否是合法的日期
bool IsValidDate(int Year, int Month, int Day)
{
    int nDay;

    if (Year < 1 || Month >12 || Month < 1 || Day < 1)
    {
        return false;
    }

    switch (Month) {
    case 4:
    case 6:
    case 9:
    case 11:
        nDay = 30;
        break;
    case 2:
        if (IsLeapYear(Year))
        {
            nDay = 29;
        }
        else
        {
            nDay = 28;
        }
        break;
    default:
        nDay = 31;
    }

    if (Day > nDay)
    {
        return false;
    }

    return true;
}


// 编辑定时器窗口
INT_PTR CALLBACK EditBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static CUSTIMER tempTimer = { NULL };
    TCHAR tempbuffer[30];
    HWND hwndList,hwndList2,hwndList3;
    WORD templen;
    int lbItem1,lbItem2,lbItem3;
    switch (message)
    {
    case WM_INITDIALOG:
        tempTimer.isoneweek = false;
        tempTimer.isday = false;
        tempTimer.islong = false;
        tempTimer.aimtime = { -1,-1,-1 };
        tempTimer.lowertime = { -1,-1,-1 };
        tempTimer.uppertime = { -1,-1,-1 };
        tempTimer.day = -1;
        tempTimer.month = -1;
        tempTimer.year = -1;

        hwndList = GetDlgItem(hWnd, IDC_YEAR);
        for (int i = 0; i < 40; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)yearString[i]);
        }

        hwndList = GetDlgItem(hWnd, IDC_MONTH);
        for (int i = 0; i < 12; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)mouthString[i]);
        }
        hwndList = GetDlgItem(hWnd, IDC_DAY);
        for (int i = 0; i < 31; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)dayString[i]);

        }
        hwndList = GetDlgItem(hWnd, IDC_AIMHOU);
        hwndList2 = GetDlgItem(hWnd, IDC_UPPERHOU);
        hwndList3 = GetDlgItem(hWnd, IDC_LOWERHOU);
        for (int i = 0; i < 24; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList2, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList3, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
        }
        hwndList = GetDlgItem(hWnd, IDC_AIMMIN);
        hwndList2 = GetDlgItem(hWnd, IDC_UPPERMIN);
        hwndList3 = GetDlgItem(hWnd, IDC_LOWERMIN);
        for (int i = 0; i < 60; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList2, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList3, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
        }
        hwndList = GetDlgItem(hWnd, IDC_AIMSEC);
        hwndList2 = GetDlgItem(hWnd, IDC_UPPERSEC);
        hwndList3 = GetDlgItem(hWnd, IDC_LOWERSEC);
        for (int i = 0; i < 60; i++)
        {
            SendMessage(hwndList, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList2, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
            SendMessage(hwndList3, LB_ADDSTRING, 0,(LPARAM)minsecString[i]);
        }
        longGroupSwitch(hWnd, FALSE);
        dayGroupSwitch(hWnd, FALSE);
        //StringCchCopy(tempTimer.name,MAXSTRINGNAME,)

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int iwParam = LOWORD(wParam);
        int hwParam = HIWORD(wParam);
        switch (iwParam)
        {
        case IDCANCEL:
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDSAVE:        // 如果点击了保存
            
            //name字段的检查--------------------------------------
            templen = (WORD)SendDlgItemMessage(hWnd,
                IDC_NAME,
                EM_LINELENGTH,
                (WPARAM)0,
                (LPARAM)0);

            if (templen == 0)
            {
                MessageBox(hWnd, L"请输入名字", L"错误", MB_ICONERROR);
                break;
            }   // 这里不包括null-terminal
            else if (templen > MAXSTRINGNAME - 1) {
                MessageBox(hWnd, L"名字长度超过范围", L"错误", MB_ICONERROR);
                break;
            }
            *((LPWORD)tempTimer.name) = templen;

            SendDlgItemMessage(hWnd,
                IDC_NAME,
                EM_GETLINE,
                (WPARAM)0,       // line 0 
                (LPARAM)tempTimer.name);

            tempTimer.name[templen] = 0;
            //MessageBox(hWnd, tempTimer.name, L"NAME", MB_OK);
            //name字段的检查--------------------------------------

            //remindstring字段的检查------------------------------
            templen = SendDlgItemMessage(hWnd,
                IDC_REMINDSTRING,
                WM_GETTEXTLENGTH,
                (WPARAM)0,      
                (LPARAM)0);

            if (templen == 0)
            {
                MessageBox(hWnd, L"请输入提醒内容", L"错误", MB_ICONERROR);
                break;
            }   // 包括null-terminal
            else if (templen > MAXSTRING) {
                MessageBox(hWnd, L"提醒内容长度超过范围", L"错误", MB_ICONERROR);
                break;
            }
            SendDlgItemMessage(hWnd,
                IDC_REMINDSTRING,
                WM_GETTEXT,
                (WPARAM)templen+1,
                (LPARAM)tempTimer.remindstring);
            //MessageBox(hWnd, tempTimer.remindstring, L"NAME", MB_OK);
            //remindstring字段的检查------------------------------

            //aim字段的检查---------------------------------------
            lbItem1 = (int)SendDlgItemMessage(hWnd, IDC_AIMHOU, LB_GETCURSEL, 0, 0);
            lbItem2 = (int)SendDlgItemMessage(hWnd, IDC_AIMMIN, LB_GETCURSEL, 0, 0);
            lbItem3 = (int)SendDlgItemMessage(hWnd, IDC_AIMSEC, LB_GETCURSEL, 0, 0);
            if (lbItem1 == LB_ERR or lbItem2 == LB_ERR or lbItem3 == LB_ERR)
            {
                MessageBox(hWnd, L"请正确设置瞄准时间", L"错误", MB_ICONERROR);
                break;
            }
            tempTimer.aimtime.hou = lbItem1;
            tempTimer.aimtime.min = lbItem2;
            tempTimer.aimtime.sec = lbItem3;
            StringCchPrintf(tempbuffer, 30, L"%d:%d:%d", tempTimer.aimtime.hou, tempTimer.aimtime.min, tempTimer.aimtime.sec);
            //MessageBox(hWnd, tempbuffer, L"AIM", MB_OK);
            //aim字段的检查---------------------------------------

            //如果开启长时间，对长时间组的检查------------------------
            //注意要检测日期是否合法
            if (tempTimer.islong)
            {
                lbItem1 = (int)SendDlgItemMessage(hWnd, IDC_MONTH, LB_GETCURSEL, 0, 0);
                lbItem2 = (int)SendDlgItemMessage(hWnd, IDC_DAY, LB_GETCURSEL, 0, 0);
                lbItem3 = (int)SendDlgItemMessage(hWnd, IDC_YEAR, LB_GETCURSEL, 0, 0);
                if (lbItem1 == LB_ERR or lbItem2 == LB_ERR or lbItem3 == LB_ERR or 
                    !IsValidDate(lbItem3 + 2020, lbItem1 + 1, lbItem2 + 1))
                {
                    MessageBox(hWnd, L"请正确设置年份月份和日份", L"错误", MB_ICONERROR);
                    break;
                }
                tempTimer.year = lbItem3 + 2020;
                tempTimer.month = lbItem1+1;
                tempTimer.day = lbItem2+1;
                StringCchPrintf(tempbuffer, 30, L"%d年%d月%d日", tempTimer.year, tempTimer.month, tempTimer.day);
                //MessageBox(hWnd, tempbuffer, L"year&mouth&day", MB_OK);
            }
            //如果开启长时间，对长时间组的检查------------------------

            //如果开启日循环，对日循环组的检查------------------------
            if (tempTimer.isday)
            {
                for (int i = 0; i < 3; i++)
                {
                    lbItem1 = (int)SendDlgItemMessage(hWnd, IDC_UPPERHOU+i, LB_GETCURSEL, 0, 0);
                    lbItem2 = (int)SendDlgItemMessage(hWnd, IDC_LOWERHOU+i, LB_GETCURSEL, 0, 0);
                    if (lbItem1 == LB_ERR or lbItem2 == LB_ERR)
                    {
                        MessageBox(hWnd, L"请正确设置上限和下限时间", L"错误", MB_ICONERROR);
                        tempTimer.isday = false;
                        break;
                    }
                    
                }
                lbItem1 = (int)SendDlgItemMessage(hWnd, IDC_UPPERHOU, LB_GETCURSEL, 0, 0);
                lbItem2 = (int)SendDlgItemMessage(hWnd, IDC_LOWERHOU , LB_GETCURSEL, 0, 0);
                if (lbItem1 > lbItem2)
                {
                    MessageBox(hWnd, L"上限时间不能大于下限时间", L"错误", MB_ICONERROR);
                    tempTimer.isday = false;
                    break;
                }                
                if (lbItem1 == lbItem2)
                {
                    lbItem1 = (int)SendDlgItemMessage(hWnd, IDC_UPPERMIN, LB_GETCURSEL, 0, 0);
                    lbItem2 = (int)SendDlgItemMessage(hWnd, IDC_LOWERMIN, LB_GETCURSEL, 0, 0);
                    if (lbItem1 >= lbItem2)
                    {
                        MessageBox(hWnd, L"上限时间不能大于下限时间", L"错误", MB_ICONERROR);
                        tempTimer.isday = false;
                        break;
                    }
                }
                if (!tempTimer.isday)
                {
                    tempTimer.isday = true;
                    break;
                }
                tempTimer.uppertime.hou = (int)SendDlgItemMessage(hWnd, IDC_UPPERHOU, LB_GETCURSEL, 0, 0);
                tempTimer.lowertime.hou = (int)SendDlgItemMessage(hWnd, IDC_LOWERHOU, LB_GETCURSEL, 0, 0);
                tempTimer.uppertime.min = (int)SendDlgItemMessage(hWnd, IDC_UPPERMIN, LB_GETCURSEL, 0, 0);
                tempTimer.lowertime.min = (int)SendDlgItemMessage(hWnd, IDC_LOWERMIN, LB_GETCURSEL, 0, 0);
                tempTimer.uppertime.sec = (int)SendDlgItemMessage(hWnd, IDC_UPPERSEC, LB_GETCURSEL, 0, 0);
                tempTimer.lowertime.sec = (int)SendDlgItemMessage(hWnd, IDC_LOWERSEC, LB_GETCURSEL, 0, 0);
                
            }
            //如果开启日循环，对日循环组的检查------------------------
            MessageBox(hWnd, L"保存成功", L"成功", MB_OK);
            structtimer[8].push_back(tempTimer);
            SendMessageW(GetParent(hWnd), WM_COMMAND, CUS_UPDATECUS, NULL);
            EndDialog(hWnd, LOWORD(wParam));

            break;
        case IDC_ISONEWEEK: // 如果点击了周循环
            if (hwParam == BN_CLICKED)
            {
                if (!tempTimer.isoneweek)
                {
                    tempTimer.isoneweek = true;
                    SendMessage(GetDlgItem(hWnd, IDC_ISONEWEEK), BM_SETCHECK, BST_CHECKED, NULL);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISLONG), FALSE);
                }
                else {
                    tempTimer.isoneweek = false;
                    SendMessage(GetDlgItem(hWnd, IDC_ISONEWEEK), BM_SETCHECK, BST_UNCHECKED, NULL);
                    if (!tempTimer.isday)
                    EnableWindow(GetDlgItem(hWnd, IDC_ISLONG), TRUE);
                }
            }
            break;
        case IDC_ISDAY:     // 如果点击了日循环
            if (hwParam == BN_CLICKED)
            {
                if (!tempTimer.isday)
                {
                    tempTimer.isday = true;
                    dayGroupSwitch(hWnd, TRUE);
                    SendMessage(GetDlgItem(hWnd, IDC_ISDAY), BM_SETCHECK, BST_CHECKED, NULL);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISLONG), FALSE);
                }
                else {
                    dayGroupSwitch(hWnd, FALSE);
                    tempTimer.isday = false;
                    SendMessage(GetDlgItem(hWnd, IDC_ISDAY), BM_SETCHECK, BST_UNCHECKED, NULL);
                    if (!tempTimer.isoneweek)
                    EnableWindow(GetDlgItem(hWnd, IDC_ISLONG), TRUE);
                }
            }
            break;
        case IDC_ISLONG:    // 如果点击了长时间
            if (hwParam==BN_CLICKED)
            {
                if (!tempTimer.islong)
                {
                    tempTimer.islong = true;
                    longGroupSwitch(hWnd, TRUE);
                    SendMessage(GetDlgItem(hWnd, IDC_ISLONG), BM_SETCHECK, BST_CHECKED, NULL);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISDAY), FALSE);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISONEWEEK), FALSE);
                }
                else {
                    longGroupSwitch(hWnd, FALSE);
                    tempTimer.islong = false;
                    SendMessage(GetDlgItem(hWnd, IDC_ISLONG), BM_SETCHECK, BST_UNCHECKED, NULL);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISDAY), TRUE);
                    EnableWindow(GetDlgItem(hWnd, IDC_ISONEWEEK), TRUE);
                }
            }
            break;

        default:
            break;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


// 编辑表窗口
INT_PTR CALLBACK EditTableBox(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    TCHAR tempbuffer[100];
    static HWND hwndList[9];
    // 0-6一周表 , 7长期表，8制定表
    static int selectOuterIndex = -1;
    // 表内部的定时器索引
    static int selectInnerIndex = -1;
    int OuterIndex, InnerIndex;
    size_t len;
    switch (message)
    {
    case WM_INITDIALOG:
        hwndList[0] = GetDlgItem(hWnd, IDC_ZEST1);
        hwndList[1] = GetDlgItem(hWnd, IDC_ZEST2);
        hwndList[2] = GetDlgItem(hWnd, IDC_ZEST3);
        hwndList[3] = GetDlgItem(hWnd, IDC_ZEST4);
        hwndList[4] = GetDlgItem(hWnd, IDC_ZEST5);
        hwndList[5] = GetDlgItem(hWnd, IDC_ZEST6);
        hwndList[6] = GetDlgItem(hWnd, IDC_ZEST7);
        hwndList[7] = GetDlgItem(hWnd, IDC_ZEST8);
        hwndList[8] = GetDlgItem(hWnd, IDC_ZEST9);
        for (int t = 0; t < 9; t++)
        {
            
            for (int i = 0; i < structtimer[t].size(); i++)
            {
                SendMessage(hwndList[t], LB_ADDSTRING, 0,(LPARAM)structtimer[t][i].name);
            }
        }
        hwndList[0] = GetDlgItem(hWnd, IDC_OPERATELIST);
        for (int i = 0; i < 9; i++)
        {
            SendMessage(hwndList[0], LB_ADDSTRING, 0, (LPARAM)minsecString[i]);
        }
        hwndList[0] = GetDlgItem(hWnd, IDC_ZEST1);

        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int iwParam = LOWORD(wParam);
        int hwParam = HIWORD(wParam);
        switch (iwParam)
        {
        case IDOK:
            SendMessageW(GetParent(hWnd), WM_COMMAND, CUS_UPDATECUS, NULL);
            SendMessageW(GetParent(hWnd), WM_COMMAND, CUS_UPDATEMAIN, NULL);
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDCANCEL:
            SendMessageW(GetParent(hWnd), WM_COMMAND, CUS_UPDATECUS, NULL);
            SendMessageW(GetParent(hWnd), WM_COMMAND, CUS_UPDATEMAIN, NULL);
            EndDialog(hWnd, LOWORD(wParam));
            return (INT_PTR)TRUE;
        case IDADD:
            OuterIndex = (int)SendDlgItemMessage(hWnd, IDC_OPERATELIST, LB_GETCURSEL, 0, 0);
            if (OuterIndex == LB_ERR)
            {
                MessageBox(hWnd, L"请设置操作表索引(0-6为一周表,7为长期表，8为制定表)", L"错误", MB_ICONERROR);
                break;
            }
            if (OuterIndex == 8)
            {
                MessageBox(hWnd, L"制定表不能自己给自己添加内容", L"错误", MB_ICONERROR);
                break;
            }
            InnerIndex = (int)SendMessage(hwndList[8], LB_GETCURSEL, 0, 0);

            if (InnerIndex == LB_ERR)
            {
                MessageBox(hWnd, L"请在制定表中选择一个要添加的定时器", L"错误", MB_ICONERROR);
                break;
            }
            if (structtimer[8][InnerIndex].islong)
            {
                SYSTEMTIME tempTimer;
                GetLocalTime(&tempTimer);
                INT64 nowResult = (INT64)tempTimer.wYear * 10000000000 + (INT64)tempTimer.wMonth * 100000000 
                    + (INT64)tempTimer.wDay * 1000000 + tempTimer.wHour * 10000 + tempTimer.wMinute * 100 + tempTimer.wSecond;
                INT64 nowResult2 = (INT64)structtimer[8][InnerIndex].year * 10000000000 + (INT64)structtimer[8][InnerIndex].month * 100000000
                    + (INT64)structtimer[8][InnerIndex].day * 1000000 + structtimer[8][InnerIndex].aimtime.hou * 10000 + structtimer[8][InnerIndex].aimtime.min * 100 + structtimer[8][InnerIndex].aimtime.sec;
                if (nowResult>=nowResult2)
                {
                    MessageBox(hWnd, L"该长时间定时器已经过期啦", L"错误", MB_ICONERROR);
                    break;
                }
            }
            if (structtimer[8][InnerIndex].islong and OuterIndex != 7)
            {
                MessageBox(hWnd, L"长时间类型的定时器只能添加进入长期表", L"错误", MB_ICONERROR);
                break;
            }
            if (!structtimer[8][InnerIndex].islong and OuterIndex == 7)
            {
                MessageBox(hWnd, L"非长时间类型的定时器不能添加进入长期表", L"错误", MB_ICONERROR);
                break;
            }
            CUSTIMER thecusTemp = structtimer[8][InnerIndex];
            structtimer[OuterIndex].push_back(thecusTemp);
            SendMessage(hwndList[OuterIndex], LB_ADDSTRING, 0, (LPARAM)structtimer[8][InnerIndex].name);
            SendMessage(hwndList[OuterIndex], WM_PAINT, 0, 0);
            return (INT_PTR)TRUE;
        case IDDEL:
            OuterIndex = (int)SendDlgItemMessage(hWnd, IDC_OPERATELIST, LB_GETCURSEL, 0, 0);
            if (OuterIndex == LB_ERR)
            {
                MessageBox(hWnd, L"请设置操作表索引(0-6为一周表,7为长期表，8为制定表)", L"错误", MB_ICONERROR);
                break;
            }
            InnerIndex = (int)SendMessage(hwndList[OuterIndex], LB_GETCURSEL, 0, 0);
            if (InnerIndex == LB_ERR)
            {
                MessageBox(hWnd, L"请在操作表索引指向的表中选择一个要删除的定时器", L"错误", MB_ICONERROR);
                break;
            }
            structtimer[OuterIndex].erase(structtimer[OuterIndex].begin() + InnerIndex, structtimer[OuterIndex].begin() + InnerIndex+1);
            SendMessage(hwndList[OuterIndex], LB_DELETESTRING, (WPARAM)InnerIndex, 0);
            SendMessage(hwndList[OuterIndex], WM_PAINT, 0, 0);
            return (INT_PTR)TRUE;
        default:
            break;
        }
        if (hwParam == LBN_DBLCLK)
        {
            hdc = GetDC(hWnd);
            for ( OuterIndex = 0; OuterIndex < 9; OuterIndex++)
            {
                if (hwndList[OuterIndex] == (HWND)lParam)
                    break;
                
            }
            InnerIndex = (int)SendMessage(hwndList[OuterIndex], LB_GETCURSEL, 0, 0);
            StringCchPrintf(tempbuffer, 100, L"名字: %s", structtimer[OuterIndex][InnerIndex].name);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPNAME, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

            StringCchPrintf(tempbuffer, 100, L"瞄准时间: %d:%d:%d", structtimer[OuterIndex][InnerIndex].aimtime.hou,
                structtimer[OuterIndex][InnerIndex].aimtime.min, structtimer[OuterIndex][InnerIndex].aimtime.sec);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPAIM, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

            StringCchPrintf(tempbuffer, 100, L"%s", structtimer[OuterIndex][InnerIndex].remindstring);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPSTRING, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

            StringCchPrintf(tempbuffer, 100, L"日循环: %d", structtimer[OuterIndex][InnerIndex].isday);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPISDAY, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

            StringCchPrintf(tempbuffer, 100, L"长时间: %d", structtimer[OuterIndex][InnerIndex].islong);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPISLONG, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

            StringCchPrintf(tempbuffer, 100, L"周循环: %d", structtimer[OuterIndex][InnerIndex].isoneweek);
            StringCchLength(tempbuffer, 100, &len);
            SendDlgItemMessage(hWnd, IDC_TEMPISONEWEEK, WM_SETTEXT, NULL, (LPARAM)tempbuffer);
            if (structtimer[OuterIndex][InnerIndex].islong)
            {
                StringCchPrintf(tempbuffer, 100, L"到期时间: %d年%d月%d日", structtimer[OuterIndex][InnerIndex].year
                   , structtimer[OuterIndex][InnerIndex].month
                , structtimer[OuterIndex][InnerIndex].day);
                StringCchLength(tempbuffer, 100, &len);
                SendDlgItemMessage(hWnd, IDC_TEMPSTRING1, WM_SETTEXT, NULL, (LPARAM)tempbuffer);


                SendDlgItemMessage(hWnd, IDC_TEMPSTRING2, WM_SETTEXT, NULL, NULL);
            }

            else if (structtimer[OuterIndex][InnerIndex].isday)
            {
                StringCchPrintf(tempbuffer, 100, L"开始时间: %d:%d:%d", structtimer[OuterIndex][InnerIndex].uppertime.hou,
                    structtimer[OuterIndex][InnerIndex].uppertime.min, structtimer[OuterIndex][InnerIndex].uppertime.sec
                );
                StringCchLength(tempbuffer, 100, &len);
                SendDlgItemMessage(hWnd, IDC_TEMPSTRING1, WM_SETTEXT, NULL, (LPARAM)tempbuffer);

                StringCchPrintf(tempbuffer, 100, L"结束时间: %d:%d:%d", structtimer[OuterIndex][InnerIndex].lowertime.hou,
                    structtimer[OuterIndex][InnerIndex].lowertime.min, structtimer[OuterIndex][InnerIndex].lowertime.sec
                );
                StringCchLength(tempbuffer, 100, &len);
                SendDlgItemMessage(hWnd, IDC_TEMPSTRING2, WM_SETTEXT, NULL, (LPARAM)tempbuffer);
            }
            else {
                SendDlgItemMessage(hWnd, IDC_TEMPSTRING1, WM_SETTEXT, NULL, NULL);
                SendDlgItemMessage(hWnd, IDC_TEMPSTRING2, WM_SETTEXT, NULL, NULL);
            }

            ReleaseDC(hWnd, hdc);
        }
        break;

    }
    return (INT_PTR)FALSE;
}

// 保存
bool OverSaveFile(LISTCUSTIMER listtimer[], const char filename[], const char mode[])
{
    FILE* fp;
    fopen_s(&fp, filename, mode);
    if (fp == NULL)
        return 0;
    for (size_t t = 0; t < 9; t++)
    {
        int len = listtimer[t].size();
        fwrite((char*)&len, sizeof(int), 1, fp);
        LISTCUSTIMER::iterator i;
        for (i = listtimer[t].begin(); i != listtimer[t].end(); ++i)
        {
            fwrite((char*)&*i, sizeof(CUSTIMER), 1, fp);
        }
    }
    
    fclose(fp);
    return 1;
}

// 加载
bool OverLoadFile(LISTCUSTIMER listtimer[], const char filename[], const char mode[])
{
    FILE* fp;
    fopen_s(&fp, filename, mode);
    if (fp == NULL)
        return 0;
    for (size_t t = 0; t < 9; t++)
    {
        int temp;
        fread((char*)&temp, sizeof(int), 1, fp);
        for (int i = 0; i < temp; i++)
        {
            CUSTIMER temp;
            fread((char*)&temp, sizeof(CUSTIMER), 1, fp);
            listtimer[t].push_back(temp);
        }
    }
    
    fclose(fp);
    return 1;
}