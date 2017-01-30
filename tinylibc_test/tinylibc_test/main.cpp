
#include <cstdint>
#include <float.h>

#if defined(_WIN32)
// begin printf
extern "C" int __cdecl printf(const char * format, ...);

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#pragma comment(linker, "/defaultlib:user32.lib")
extern "C" int __cdecl printf(const char * format, ...)
{
    char szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;
          
    va_start( argptr, format );
    retValue = wvsprintf( szBuff, format, argptr );
    va_end( argptr );

    WriteFile(  GetStdHandle(STD_OUTPUT_HANDLE), szBuff, retValue,
                &cbWritten, 0 );

    return retValue;
}
// end printf

// begin malloc, calloc and free
extern HANDLE _heapHndl;
extern "C" void* __cdecl malloc(size_t bytes)
{
  return (void*)HeapAlloc(_heapHndl, 0, bytes);
}
extern "C" void  __cdecl   free(_Pre_maybenull_ _Post_invalid_ void*    mem)
{
  HeapFree(_heapHndl, 0, mem);
}
extern "C" void* __cdecl calloc(size_t nitems, size_t size)
{
    return HeapAlloc( _heapHndl, HEAP_ZERO_MEMORY, nitems * size );
}
//extern "C" void * __cdecl calloc(size_t nitems, size_t size)
//{
//    return HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, nitems * size );
//}

// end malloc and free

// begin hypot function from musl (needed by std::vector for some reason)
#if FLT_EVAL_METHOD > 1U && LDBL_MANT_DIG == 64
#define SPLIT (0x1p32 + 1)
#else
#define SPLIT (0x1p27 + 1)
#endif

//static void sq(double_t *hi, double_t *lo, double x)
//{
//	double_t xh, xl, xc;
//
//	xc = (double_t)x*SPLIT;
//	xh = x - xc + xc;
//	xl = x - xh;
//	*hi = (double_t)x*x;
//	*lo = xh*xh - *hi + 2*xh*xl + xl*xl;
//}
//extern "C" double _hypot(double x, double y)
//{
//	union {double f; uint64_t i;} ux = {x}, uy = {y}, ut;
//	int ex, ey;
//	double_t hx, lx, hy, ly, z;
//
//	/* arrange |x| >= |y| */
//	ux.i &= -1ULL>>1;
//	uy.i &= -1ULL>>1;
//	if (ux.i < uy.i) {
//		ut = ux;
//		ux = uy;
//		uy = ut;
//	}
//
//	/* special cases */
//	ex = ux.i>>52;
//	ey = uy.i>>52;
//	x = ux.f;
//	y = uy.f;
//	/* note: hypot(inf,nan) == inf */
//	if (ey == 0x7ff)
//		return y;
//	if (ex == 0x7ff || uy.i == 0)
//		return x;
//	/* note: hypot(x,y) ~= x + y*y/x/2 with inexact for small y/x */
//	/* 64 difference is enough for ld80 double_t */
//	if (ex - ey > 64)
//		return x + y;
//
//	/* precise sqrt argument in nearest rounding mode without overflow */
//	/* xh*xh must not overflow and xl*xl must not underflow in sq */
//	z = 1;
//	if (ex > 0x3ff+510) {
//		z = 0x1p700;
//		x *= 0x1p-700;
//		y *= 0x1p-700;
//	} else if (ey < 0x3ff-450) {
//		z = 0x1p-700;
//		x *= 0x1p700;
//		y *= 0x1p700;
//	}
//	sq(&hx, &lx, x);
//	sq(&hy, &ly, y);
//	return z*sqrt(ly+lx+hy+hx);
//}
extern "C" double _hypot(double x, double y){ return 0; }

// end hypot function from musl (needed by std::vector for some reason)


// begin memcpy (from musl)
#if defined(_DEBUG) 
void* memcpy(void* dest, const void* src, size_t n)
{
  unsigned char* d = (unsigned char*)dest;
  const unsigned char* s = (const unsigned char*)src;

  for(; n; n--) *d++ = *s++;
	return dest;
}
#endif
// end memcpy

// begin various windows entry points
void entry();

void DllMain(){}
void main(){ entry(); }
int WINAPI WinMain (
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
    )
{
  entry();
}
//void WinMain(){ entry(); }

// end various windows entry points
#endif

//#include <vector>

//#define EASTL_EABASE_DISABLED
//#include <EASTL/string.h>

static char* strmsg = "hello tiny world";
void entry()
{
  char* mem = (char*)malloc(17);
  memcpy(mem, strmsg, 17);
  printf(mem);
  free(mem);
}


//extern int WINAPI WinMain (
//    _In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPSTR lpCmdLine,
//    _In_ int nShowCmd
//    );
