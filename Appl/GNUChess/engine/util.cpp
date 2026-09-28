/* Prepared for PC/GEOS on 2026-09-18. SPDX-License-Identifier: GPL-3.0-or-later */
/* GNU Chess PC/GEOS portability support. GPL-3.0-or-later. */
#include "util.h"
#ifdef __WATCOMC__
extern "C" void _pascal GCFatal(const char *reason);
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
static const gc_int PoolSize=49152L;
bool pool_init(){
 if(Pool)return true;
 Pool=(char*)malloc((unsigned short)PoolSize);
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
