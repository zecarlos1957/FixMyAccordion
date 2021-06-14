#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");

#define PANELSIZE 12
#define MAX_BUTTONS 8


//                 1    2    3    4    5     6     7    8    9    10     11    12
char * Note[12]={"FA#","SI","MI","LA","RE","SOL","DO","FA","SIb","MIb","SOL#","DO#"};

int ClefFA[12]={2,7,12,5,10,3,8,1,6,11,4,9};


static int ChordList[12][5] =
{
	{1,9,12,2,3},
	{2,10,1,5,4},
	{3,11,2,6,5},
	{4,12,3,7,6},
	{5,1,4,8,7},
	{6,2,5,9,8},
	{7,3,6,10,9},
	{8,4,7,11,10},
	{9,5,8,12,11},
	{10,6,9,1,12},
	{11,7,10,2,1},
	{12,8,11,3,2}
};

static int ShaftList[12][4] =
{
	{12,17,19,0},
	{3,5,10,0},
	{8,13,15,0},
	{1,8,20,0},
	{4,9,11,16},
	{14,19,0,0},
	{5,7,12,0},
	{10,15,17,0},
	{6,18,23,0},
	{6,11,13,18},
	{4,16,21,0}, 
	{7,9,14,0}
};
 
static int MajorKeys[MAX_BUTTONS][3] =
{
	{-5,9,-12},
	{-11,3,-6},
	{-6,10,-1},
	{-1,5,-8},
	{-8,12,-3},
	{-3,7,-10},
	{-10,2,-5},
	{-12,4,-7}
};

int X1, Y1;
int wSpace, hSpace;

class TShaft
{
    DWORD conf;
    int x,y,status;
    int position;
    int pinout;
    char name[5];
public:
    TShaft(int n1, int n2, int n3 = 0, int n4 = 0);
    void Draw(HDC dc, int st);
    void ReversePos();
    int SetPos(int);
    int Where();
    int Select(int *,int *);
    void GetPin(int*,int*);

};

class TKey
{
    DWORD Lconf,Rconf;
    int x,y;
    int status;
    int position;
    void MakeConfig(int n1, int n2, int n3, int n4);

public:
    TKey(int n1, int n2, int n3, int n4 = 0);
    int SetPos(int);
    int MouseOver(POINTS p);
    void Draw(HDC dc,int stat);
    void GetKeyConfig(DWORD *lc, DWORD *rc){*lc =Lconf; *rc = Rconf;}
};


class ButtonPanel
{
    HWND hWnd;
    int initpos;
    TShaft *shaft[PANELSIZE];
    TKey *key[MAX_BUTTONS];

public:
    ButtonPanel(HWND hwnd);
    ~ButtonPanel();
    int Init();
    int Find(int *index);
    int GetNext(int i);
    void OnPaint();
    int CheckClick(POINTS p);
    void OnKeyClick(int n, UINT msg);
};

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           700,                 /* The programs width */
           450,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static ButtonPanel *btnPanel = NULL;
    
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            btnPanel = new ButtonPanel(hwnd);
            if(btnPanel->Init() < 0)
            {
                MessageBox(0, "Fail to build buttons panel","Error", MB_OK);
                return -1;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        {
            POINTS p = MAKEPOINTS(lParam);
            int n = btnPanel->CheckClick(p);
            if(n != -1)   
                btnPanel->OnKeyClick(n, message);
            break;
        }     
        case WM_PAINT:
            btnPanel->OnPaint();
            break;
            
        case WM_DESTROY:
            delete btnPanel;
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

ButtonPanel::ButtonPanel(HWND hwnd): hWnd(hwnd)
{
    ZeroMemory(shaft, (PANELSIZE * sizeof(TShaft*)));
    ZeroMemory(key, (MAX_BUTTONS * sizeof(TKey*)));
    
    // Locate the image in the screen
    X1 = 40;
    Y1 = 70;

    wSpace = 21;    // Space between keys
    hSpace = 21;    // Space between shafts
    initpos = 2;    // position of first key in buttons panel
}

ButtonPanel::~ButtonPanel()
{
    for(int i = 0; i < MAX_BUTTONS; i++)
        delete key[i];
    for(int i = 0; i < PANELSIZE; i++)
          delete shaft[i];
}

int ButtonPanel::Init()
{
    int index[MAX_BUTTONS];
    
    if(!Find(index))
    {
       MessageBox(0, "Impossible to sort keys","Error", MB_OK);
       return -1;
    }
    //
    for(int n = 0; n < MAX_BUTTONS; n++)
    {
        TKey *k = new TKey(MajorKeys[index[n]][0], MajorKeys[index[n]][1], MajorKeys[index[n]][2]);
        k->SetPos(n + initpos + 1);
        key[n] = k;
    }
    
    for(int n = 0; n < PANELSIZE; n++)
    {
        TShaft * tr = new TShaft(ShaftList[n][0], ShaftList[n][1], ShaftList[n][2], ShaftList[n][3]);
        
        for(int i = 0; i < 2; i++)
        {
            int p = tr->Where();
            if(p > 0 && p < 13 && shaft[p - 1] == 0)
            {
                tr->SetPos(p);
                shaft[p - 1] = tr;
                break;
            }
            else  // reverse position and try again
            {
                if(i == 0)tr->ReversePos();
                else 
                {
                    delete tr;
                    return -2;
                }
            }
        }
    }
    return 0;
}

/*
*    Sort the key list to correct order
*    return true if sucessfull, false otherwise. 
*/

int ButtonPanel::Find(int *index)
{
    int value, done;
    
    ZeroMemory(index, MAX_BUTTONS * sizeof(int));
               
    for(int i = 0; i < MAX_BUTTONS; i++)
    {
        value = i;
        done = 0;
        *(index + 7) = value;
        
        for (int n = 0; n < MAX_BUTTONS; n++)
        {
            value = GetNext(value);
            if (value < 0) break;
            else
            {
                done++;
                *(index + (6 - n)) = value;
            }
        }
        if (done == 7) return 1;
    }
    return 0;
}

int ButtonPanel::GetNext(int i)
{
    // get first note of chord
    int value = MajorKeys[i][0];

    // find the chord index that match the third note
    for(int t = 0; t < MAX_BUTTONS; t++)
    {
        if (i != t && value == MajorKeys[t][2])
            return t;                   
    }
    return -1;
}

void ButtonPanel::OnPaint()
{
    PAINTSTRUCT ps;
    
    HDC dc = BeginPaint(hWnd, &ps);
    int x1 = X1 - (wSpace / 2);
    int w = 25 * wSpace + wSpace;
    LOGBRUSH lb;
    lb.lbStyle = BS_PATTERN;

    HANDLE old = SelectObject(dc,CreatePen(PS_SOLID, 3, RGB(200, 100, 150)));

    MoveToEx(dc, x1, Y1 - 10, NULL);
    LineTo(dc, x1 + w, Y1 - 10);
    LineTo(dc, x1 + w, Y1 + 275);
    LineTo(dc, x1, Y1 + 275);
    LineTo(dc, x1, Y1 - 10);
    
    DeleteObject(SelectObject(dc,old));
    
    for(int i = 0; i < MAX_BUTTONS; i++)
        key[i]->Draw(dc, 0);
 
    for(int n = 0; n < PANELSIZE; n++)
    {
        if(shaft[n])  
            shaft[n]->Draw(dc, 0);
    }
    
    old = SelectObject(dc, CreatePen(PS_SOLID, 3, RGB(200, 200, 200)));
    
    for(int n = 0; n < 25; n++)
    {
        int xx = x1 + n * wSpace + wSpace;
        MoveToEx(dc, xx - 2, Y1 - 10, NULL);
        LineTo(dc, xx + 2 , Y1 - 10);
    }
    for(int n = 0; n < 25; n++)
    {
        int xx = x1 + n * wSpace + wSpace;
        MoveToEx(dc, xx - 2, Y1 + 275, NULL);
        LineTo(dc, xx + 2 , Y1 + 275);
    }
    
    DeleteObject(SelectObject(dc,old));
    
    EndPaint(hWnd, &ps);
}
 
int ButtonPanel::CheckClick(POINTS p)
{
    for(int n = 0; n < MAX_BUTTONS; n++)
    {
        if(key[n]->MouseOver(p))
            return n;
    }
    return -1;
}

void ButtonPanel::OnKeyClick(int n, UINT msg)
{
    DWORD Lconf, Rconf;
    HDC dc = GetDC(hWnd);
    
    key[n]->GetKeyConfig(&Lconf, &Rconf);
    key[n]->Draw(dc, msg == WM_LBUTTONDOWN);

    for(int n = 0; n < PANELSIZE; n++)
    {
        if((Lconf >> n) & 1) 
            shaft[n]->Draw(dc, msg == WM_LBUTTONDOWN);
        if((Rconf >> n) & 1) 
            shaft[n]->Draw(dc, msg == WM_LBUTTONDOWN);
    }
     
    ReleaseDC(hWnd,dc);
}

TKey::TKey(int n1, int n2, int n3, int n4): position(0),
                                          status(0)
{
    MakeConfig(n1, n2, n3, n4);
}

void TKey::MakeConfig(int n1, int n2, int n3, int n4)
{
    Lconf = Rconf = 0;
    if (n1 > 0)      Rconf |= (DWORD)1 << (n1 - 1);
    else if (n1 < 0) Lconf |= (DWORD)1 << (abs(n1) - 1);
    if (n2 > 0)      Rconf |= (DWORD)1 << (n2 - 1);
    else if (n2 < 0) Lconf |= (DWORD)1 << (abs(n2) - 1);
    if (n3 > 0)      Rconf |= (DWORD)1 << (n3 - 1);
    else if (n3 < 0) Lconf |= (DWORD)1 << (abs(n3) - 1);
    if (n4 > 0)      Rconf |= (DWORD)1 << (n4 - 1);
    else if (n4 < 0) Lconf |= (DWORD)1 << (abs(n4) - 1);
}

int TKey::SetPos(int p)
{
    if (p < 0 || p > 12)
        return 0;
    position = p;
    x = X1 + position * (wSpace * 2) + (wSpace / 2);
    y = Y1 + 5;
    return p;
}

void TKey::Draw(HDC dc, int stat)
{
    int l = wSpace - 5;
   
    if(status != stat)
    {
        status = stat;
        HANDLE old = SelectObject(dc,CreatePen(PS_SOLID, 1, RGB(200, 200, 200)));
        MoveToEx(dc, x, y - 40, NULL);
        LineTo(dc, x, y + 12 * hSpace + 20);
        
         for(int i = 0; i < PANELSIZE; i++)
        {
            if((Lconf >> i) & 1)
            {
                MoveToEx(dc, x - l, y + i * hSpace, NULL);
                LineTo(dc, x, y + i * hSpace);
            }
            if((Rconf >> i) & 1)
            {
                MoveToEx(dc, x, y + i * hSpace, NULL);
                LineTo(dc, x + l, y + i * hSpace);
            }
        }
        y = stat ? (y + (hSpace - 10)) : (y - (hSpace - 10));
        DeleteObject(SelectObject(dc,old));
    }

    MoveToEx(dc, x, y - 40, NULL);
    LineTo(dc, x, y + 12 * hSpace + 20);
 
    for(int i = 0; i < PANELSIZE; i++)
    {
        if((Lconf >> i) & 1)
        {
            MoveToEx(dc, x - l, y + i * hSpace, NULL);
            LineTo(dc, x, y + i * hSpace);
        }
        if((Rconf >> i) & 1)
        {
            MoveToEx(dc, x, y + i * hSpace, NULL);
            LineTo(dc, x + l, y + i * hSpace);
        }
    }
}

int TKey::MouseOver(POINTS p)
{
    if(p.x > x - 15 && p.x < x + 15)
        return 1;
    return 0;
}

TShaft::TShaft(int n1, int n2, int n3, int n4)
{
    conf = 0; name[0] = '\0'; status = 0;
    if(n1)conf = (DWORD)1 << (n1 - 1);
    if(n2)conf |= (DWORD)1 << (n2 - 1);
    if(n3)conf |= (DWORD)1 << (n3 - 1);
    if(n4)conf |= (DWORD)1 << (n4 - 1);
}

void TShaft::Draw(HDC dc, int st)
{
    int yy;
    if(st)
    {
        yy = y + 10;
        SetTextColor(dc, RGB(240, 240, 240));
    }
    else
    {
        yy = y - 3;
        SetTextColor(dc, RGB(0, 0, 0));
    }
    SetBkMode(dc, TRANSPARENT);
 
    if ( status && st == 0)
    {
        HANDLE old = SelectObject(dc,CreatePen(PS_SOLID, 1, RGB(200, 200, 200)));
        for(int i = 0; i < 25; i++)
        {
            if((conf >> i) & 1)
            {
                int xx = x + wSpace * (24 - i);
            
                MoveToEx(dc, xx, y, NULL);
                LineTo(dc, xx, y + 11);
            }
        }
        DeleteObject(SelectObject(dc,old));
    }
    
    MoveToEx(dc, x, y, NULL);
    LineTo(dc, x + 25 * wSpace, y);

    for(int i = 0; i < 25; i++)
    {
        if((conf >> i) & 1)
        {
            int xx = x + wSpace * (24 - i);
            
            MoveToEx(dc, xx, yy, NULL);
            LineTo(dc, xx, y);
            if(((i + 1) & 1) && pinout == (i >> 1) + 1)
            {
                int xx = x + (24 - i) * wSpace - (wSpace / 2) + 5;
                int yy = Y1 + hSpace * 12 + 50;
                TextOut(dc, xx, yy, Note[(i / 2)], lstrlen(Note[(i / 2)]));
            }
        }
    } 
    status = st;
}

void TShaft::ReversePos()
{
    DWORD aux=0;
    for(int i = 0; i < 25; i++)
        if((conf >> i) & 1)
            aux += (DWORD)1 << (24 - i);
    conf = aux;
}

int TShaft::SetPos(int p)
{
    x = X1;
    y = Y1 + p * hSpace;
    position = p;
    return p;
}

int TShaft::Where()
{
    int in[4], out[4], p;

    ZeroMemory(in, (4 * sizeof(int)));
    ZeroMemory(out, (4 * sizeof(int)));
    
    GetPin(in, out);
    if ((p = Select(in, out)))
        return p;
    return 0;
}

/**
    
    return index clef FA note name
*/
int TShaft::Select(int *in, int *out)
{
	int done;
	int nPins = 0;
	// Calculate number of pins in shaft
	for (int n = 0; n < 25; n++)
	{  
		if((conf >> n) & 1) 
		    nPins++;
	}
	for(int n = 0; n < 4; n++)
	{
		if (out[n] == 0) break;
		done = 0;
		for (int i = 0; i < 4; i++)
		{
			 if (in[i] == 0) break;
			 for (int z = 0; z < 5; z++)
			 {
				if (out[n] == ChordList[in[i] - 1][z])
                {
					done++;
					if (done == nPins)
					{
						pinout = out[n];
						lstrcpy(name, Note[out[n] - 1]);
						return ClefFA[out[n] - 1];
					}
					break;
				  }
			 }
		 }
	}
	return 0;
}

/**
    Identify input and output pins
*/
void TShaft::GetPin(int *_in, int *_out)
{
	int *in, *out, note;
	in = _in;
	out = _out;
	for (int i = 0; i < 25; i++)
	{
		if ((conf >> i) & 1)
		{
			note = (i >> 1) + (i & 1) + 1;
			*in++ = note;             // All pins are input (receive command from keys)
			if (!(i & 1)) 
			    *out++ = note;        // One odd pin is output (control for the valve)
		 }
	}
}
