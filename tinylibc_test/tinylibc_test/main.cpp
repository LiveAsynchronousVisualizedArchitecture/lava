
// -todo: use GetProcessHeap instead of heapHndl
// -todo: pack into separate header file
// -todo: put windows CRT header in header
// -todo: try initial window

// todo: put initterm into header
// todo: put console CRT header in header
// todo: make header have separate declaration and implementation sections
// todo: make header have console and windows CRT #define controls
// todo: make preprocessor mechanics for subsystem

#include "tinylib_win.h"
#include "no_rt_util.h"
#include <WinUser.h>

//#include <vector>
//#define EASTL_EABASE_DISABLED
//#include <EASTL/string.h>
//
//template<class T> using vec = std::vector<T>; 


template<class T> class tbl
{
private:
  void* p;

  void       set_size(ui64  s)
  { 
    *((ui64*)clearBits(p) + 1) = s;
  }
  void  set_sizeBytes(ui64 sb)
  {
    *((ui64*)clearBits(p)) = sb;      // first 8 bytes should be the total size of the buffer in bytes
  } 

public:
  static void*  setDestructorBit(void* p)
  {
    //return (void*)((ui64)p ^ (((ui64)1l)<<63));
    return (void*)((ui64)p | (1llu<<63));
  }
  static bool   getDestructorBit(void* p)
  {
    return (((ui64)p)>>63)!=0;
  }
  static void*         clearBits(void* p)
  {
    return (void*)( ((ui64)p) & 0x0000FFFFFFFFFFFF);
  }
  static ui64          sizeBytes(ui64 count)  // sizeBytes is meant to take the same arguments as a constructor and return the total number of bytes to hold the entire stucture given those arguments 
  {
    return sizeof(ui64)*2 + count*sizeof(T);
  }

  tbl(){}
  tbl(ui64  count)
  {
    ui64 sb = tbl::sizeBytes(count);
    p       = Allocator().allocate(sb);
    p       = setDestructorBit(p);
    set_size(count);
    set_sizeBytes(sb);
  }
  tbl(void*  addr, ui64 count, bool owner=true) :
    p(addr)
  {
    //ui64 sb = tbl::sizeBytes(count);
    //p       = addr;
    if(owner){
      set_sizeBytes( tbl::sizeBytes(count) );
      set_size(count);
    }
  }
  tbl(void*  addr) :
    p(addr)
  {
    //ui64 sb = tbl::sizeBytes(count);
    //set_size(count);
    //set_sizeBytes(sb);
    //
    //p = addr;
  }
  tbl(tbl const&) = delete;
  void operator=(tbl const&) = delete;
  tbl(tbl&& rval)
  {
    p      = rval.p;
    rval.p = nullptr;
  }
  ~tbl()
  {
    if(p && getDestructorBit(p)){
      Deleter().operator()((T*)clearBits(p));  //free(p);
      p = nullptr;
    }
  }

  T& operator[](ui64 i)
  {
    return data()[i];
  }

  T*         data()
  {
    //ui64 pnum = 
    ui64* maskptr = (ui64*)clearBits(p); // (ui64*)( ((ui64)p) & 0x0000FFFFFFFFFFFF);
    return (T*)(maskptr+2);
    //return (void*)((ui64*)p+2);
  }
  ui64   capacity() const
  {
    return (sizeBytes() - sizeof(ui64)*2) / sizeof(T);
  }
  ui64       size() const
  {
    return *((ui64*)clearBits(p) + 1);   // second 8 bytes should be the number of elements
  } 
  ui64  sizeBytes() const
  {
    return *((ui64*)clearBits(p));   // first 8 bytes should be the total size of the buffer in bytes
  } 
  auto       addr() const -> void*
  {
    return p;
  }
};


static char* strmsg = "hello tiny world";
void entry(int argc, char** argv)
{
  //char* mem = (char*)malloc(17);
  char* mem = new char[17];

  memcpy(mem, strmsg, 17);
  memset(mem+4, 0, 17-4);
  //vec<char> mem(17);  
  printf(mem);

  delete mem;
  //free(mem);
}



HWND       hwnd;
HWND   hWndList;
HWND hwndButton;

const char g_szClassName[] = "min_win";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch(msg)
  {
    case WM_CLOSE:
      DestroyWindow(hwnd);
    break;
    case WM_DESTROY:
      PostQuitMessage(0);
    break;
    case WM_COMMAND:
      SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"name");
    break;
    default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return 0;
}

//INT_PTR CALLBACK ListBoxExampleProc(HWND hDlg, UINT message, 
//        WPARAM wParam, LPARAM lParam)
//{
//  switch (message)
//  {
//  case WM_INITDIALOG:
//      {
//          // Add items to list. 
//          HWND hwndList = GetDlgItem(hDlg, IDC_LISTBOX_EXAMPLE);  
//          for (int i = 0; i < ARRAYSIZE(Roster); i++) 
//          { 
//              int pos = (int)SendMessage(hwndList, LB_ADDSTRING, 0, 
//                  (LPARAM) Roster[i].achName); 
//              // Set the array index of the player as item data.
//              // This enables us to retrieve the item from the array
//              // even after the items are sorted by the list box.
//              SendMessage(hwndList, LB_SETITEMDATA, pos, (LPARAM) i); 
//          } 
//          // Set input focus to the list box.
//          SetFocus(hwndList); 
//          return TRUE;               
//      } 
//
//  case WM_COMMAND:
//      switch (LOWORD(wParam))
//      {
//      case IDOK:
//      case IDCANCEL:
//          EndDialog(hDlg, LOWORD(wParam));
//          return TRUE;
//
//      case IDC_LISTBOX_EXAMPLE:
//          {
//              switch (HIWORD(wParam)) 
//              { 
//              case LBN_SELCHANGE:
//                  {
//                      HWND hwndList = GetDlgItem(hDlg, IDC_LISTBOX_EXAMPLE); 
//
//                      // Get selected index.
//                      int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0); 
//
//                      // Get item data.
//                      int i = (int)SendMessage(hwndList, LB_GETITEMDATA, lbItem, 0);
//
//                      // Do something with the data from Roster[i]
//                      TCHAR buff[MAX_PATH];
//                      StringCbPrintf (buff, ARRAYSIZE(buff),  
//                          TEXT("Position: %s\nGames played: %d\nGoals: %d"), 
//                          Roster[i].achPosition, Roster[i].nGamesPlayed, 
//                          Roster[i].nGoalsScored);
//
//                      SetDlgItemText(hDlg, IDC_STATISTICS, buff); 
//                      return TRUE; 
//                  } 
//              }
//          }
//          return TRUE;
//      }
//  }
//  return FALSE;
//}

//void entry(int argc, char** argv)
int WINAPI WinMain(HINSTANCE     hInstance, 
                   HINSTANCE hPrevInstance,
                   LPSTR         lpCmdLine, 
                   int            nCmdShow)
{

  SECTION(register window class)
  { //Step 1: Registering the Window Class
    WNDCLASSEX wc; 

    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc)){
      MessageBox(NULL, "Window Registration Failed!", "Error!",
          MB_ICONEXCLAMATION | MB_OK);
      return 0;
    }
  }

  SECTION(create main window)
  {
    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Tiny Windows GUI",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 512, 256,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL){
      MessageBox(NULL, "Window Creation Failed!", "Error!",
          MB_ICONEXCLAMATION | MB_OK);
      return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
  }
  SECTION(create a button)
  {
    //HWND lb = CreateWindowEx()
    hwndButton = CreateWindow(
      "BUTTON",  // Predefined class; Unicode assumed 
      "PRESS HERE FOR NOTHING",      // Button text 
      WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
      10,         // x position 
      10,         // y position 
      100,        // Button width
      100,        // Button height
      hwnd,     // Parent window
      NULL,       // No menu.
      hInstance, //(HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), 
      NULL);      // Pointer not needed.
  }
  SECTION(list box)
  {
    hWndList = CreateWindowEx(
      WS_EX_CLIENTEDGE, 
      TEXT("listbox"), 
      "", 
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_AUTOVSCROLL, 
      240, 40, 150, 200, 
      hwnd, 
      (HMENU)105, 
      NULL, NULL);
    SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"name");
    SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"extension");
    SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"date");
    SendMessage(hWndList, LB_ADDSTRING, 0, (LPARAM)"size");
  }

  MSG Msg;
  SECTION(message loop)
  {
    while(GetMessage(&Msg, NULL, 0, 0) > 0){    // Message Loop
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
    }
  }

  return (int)Msg.wParam;
}




//extern int WINAPI WinMain (
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR lpCmdLine,
//    _In_ int nShowCmd
//    );
