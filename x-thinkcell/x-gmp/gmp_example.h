#ifndef ___GMP_EXAMPLE__
#define ___GMP_EXAMPLE__                 
#include "gmp.h"

#ifndef UNIX_LINUX
#ifndef __GMP_EXAMPLE_STATIC_LOG__
#ifdef EXPORT_DLL_API_GMP_EXAMPLE
#define DLL_API_GMP_EXAMPLE              __declspec(dllexport)
#else
#define DLL_API_GMP_EXAMPLE              __declspec(dllimport)
#endif
#else
#define DLL_API_GMP_EXAMPLE              
#endif
#else
#define DLL_API_GMP_EXAMPLE              
#endif /*! UNIX_LINUX */
#ifdef __cplusplus
extern "C" {
#endif
int main_library();
int main_01();
void wrap_mpz_init(mpz_ptr);
void wrap_mpz_clear(mpz_ptr);
#ifdef __cplusplus
}
#endif
/*gcc -shared -o libgmp_example.dll gmp_example.c -Wl,--out-implib,libgmp_example.lib -L./ -lgmp -DEXPORT_DLL_API_GMP_EXAMPLE*/


#endif
