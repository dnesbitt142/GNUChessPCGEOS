/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess PC/GEOS portability support. GPL-3.0-or-later. */
#include "util.h"
#ifdef __WATCOMC__
extern "C" {
#include <heap.h>
}
extern "C" void _pascal GCFatal(const char *reason);

/* The published SDK copy uses 32-bit MOVSD on newer builds. */
extern "C" void __declspec(naked) __far gcCopy(void);
#pragma aux gcCopy "GCCOPY";
extern "C" void __declspec(naked) __far gcCopy(void)
{
    _asm {
        push bp
        mov bp,sp
        push ds
        push es
        push si
        push di
        /* Pascal far call: count, source and destination follow the frame. */
        mov cx,[bp+6]
        lds si,[bp+8]
        les di,[bp+12]
        cld
        shr cx,1
        rep movsw
        jnc gc_copy_done
        movsb
    gc_copy_done:
        mov ax,[bp+12]
        mov dx,es
        pop di
        pop si
        pop es
        pop ds
        pop bp
        retf 10
    }
}

/* The SDK's published signed long division helper uses 386 instructions.
 * Use the Ensemble 1.x port's restoring division for full-width divisors.
 * ABI: DX:AX dividend and quotient, CX:BX divisor and remainder. */
extern "C" void __declspec(naked) __far gcDiv(void);
#pragma aux gcDiv "GCI4D";
extern "C" void __declspec(naked) __far gcDiv(void)
{
    _asm {
        push bp
        mov bp,sp
        sub sp,8
        push si
        push di

        mov si,dx
        mov di,dx
        xor di,cx
        mov [bp-2],si
        mov [bp-4],di
        test dx,8000h
        jz gc_divisor
        neg ax
        adc dx,0
        neg dx
    gc_divisor:
        test cx,8000h
        jz gc_unsigned
        neg bx
        adc cx,0
        neg cx
    gc_unsigned:
        or cx,cx
        jnz gc_full_width

        /* Divide the high word first. Its remainder feeds the low word. */
        mov si,ax
        mov ax,dx
        xor dx,dx
        div bx
        mov di,ax
        mov ax,si
        div bx
        mov bx,dx
        mov dx,di
        jmp gc_signs

    gc_full_width:
        mov [bp-6],bx
        mov [bp-8],cx
        xor si,si
        xor di,di
        mov cx,32
    gc_next_bit:
        shl ax,1
        rcl dx,1
        rcl si,1
        rcl di,1
        cmp di,[bp-8]
        ja gc_subtract
        jb gc_continue
        cmp si,[bp-6]
        jb gc_continue
    gc_subtract:
        sub si,[bp-6]
        sbb di,[bp-8]
        or ax,1
    gc_continue:
        loop gc_next_bit
        mov bx,si
        mov cx,di

    gc_signs:
        test word ptr [bp-4],8000h
        jz gc_remainder
        neg ax
        adc dx,0
        neg dx
    gc_remainder:
        test word ptr [bp-2],8000h
        jz gc_return
        neg bx
        adc cx,0
        neg cx
    gc_return:
        pop di
        pop si
        mov sp,bp
        pop bp
        retf
    }
}
#else
#include <cstdarg>
#endif
void gc_format_ply(char *p,gc_int n){
 char b[12]; gc_int i=0,j=0;
 do{b[i++]=(char)('0'+n%10);n/=10;}while(n);
 while(i)p[j++]=b[--i];
 p[j++]=' ';p[j++]='1';p[j]=0;
}
namespace engine {
// This application allocates the engine once and never resizes its caches.
// A process-lifetime arena permits a clean, pre-initialization OOM failure.
static char *Pool=0;
static gc_int PoolUsed=0;
// ATTENTION: The fixed arena fits the two boards, current caches and options
// (34260 bytes at startup). Recheck the budget if those capacities change.
static const gc_int PoolSize=34336L;
bool pool_init(){
#ifdef __WATCOMC__
 MemHandle arenaH;
#endif
 if(Pool)return true;
#ifdef __WATCOMC__
 // The engine retains far pointers into the arena between messages.
 arenaH=MemAlloc((word)PoolSize,HF_FIXED,HAF_ZERO_INIT);
 if(!arenaH)return false;
 Pool=(char*)MemDeref(arenaH);
#else
 Pool=(char*)malloc((unsigned short)PoolSize);
#endif
 return Pool!=0;
}
gc_int pool_used(){return PoolUsed;}
void *my_malloc(gc_int n){
 char *p;
 n=(n+3)&~3L;
 if(!Pool || n<=0 || n>PoolSize-PoolUsed)my_fatal("GNU Chess allocation arena exhausted");
 p=Pool+(unsigned short)PoolUsed;PoolUsed+=n;return p;
}
void my_free(void *p){
 // All objects have process lifetime; GEOS releases the owning geode's heap.
 (void)p;
}
void my_fatal(const char *format,...){
#ifdef __WATCOMC__
 GCFatal(format);
#else
 va_list a;va_start(a,format);vfprintf(stderr,format,a);va_end(a);fputc('\n',stderr);abort();
#endif
}
bool my_string_empty(const char *s){return s==0 || *s==0;}
bool my_string_equal(const char *a,const char *b){return strcmp(a,b)==0;}
char *my_strdup(const char *s){char *p=(char*)my_malloc(strlen(s)+1);strcpy(p,s);return p;}
void my_string_clear(const char **p){if(*p)my_free((void*)*p);*p=0;}
void my_string_set(const char **p,const char *s){my_string_clear(p);*p=my_strdup(s);}
void send(const char *format,...){(void)format;}
}
