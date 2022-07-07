#include <iostream>

using namespace std;

#define VECTOR_LEN 4

__declspec(align(16)) float buffer[400];

void add(float *a, float *b, float *c)
{
    int i;
    for (i = 0; i < 4; i++) {
        c[i] = a[i] + b[i];
    }
}

// Assembly
void asm_add(float *a, float *b, float *c)
{
    __asm {
    mov eax, a
    mov edx, b
    mov ecx, c
    movaps xmm0, XMMWORD PTR [eax]
    addps xmm0, XMMWORD PTR [edx]
    movaps XMMWORD PTR [ecx], xmm0
    }
}

// Intrinsics
#include <xmmintrin.h>
void intrinsics_add(float *a, float *b, float *c)
{
    __m128 t0, t1;
    t0 = _mm_load_ps(a);
    t1 = _mm_load_ps(b);
    t0 = _mm_add_ps(t0, t1);
    _mm_store_ps(c, t0);
}

// Classes
#include <fvec.h>
void classes_add(float *a, float *b, float *c)
{
    F32vec4 *av=(F32vec4 *) a;
    F32vec4 *bv=(F32vec4 *) b;
    F32vec4 *cv=(F32vec4 *) c;
    *cv=*av + *bv;
}

// Automatic Vectorization (supported by The Intel C++ Compiler)
// Compile this code using the -QAX and -QRESTRICT switches of the Intel C++ Compiler, version 4.0 or later.
// void auto_add (float *restrict a, float *restrict b, float *restrict c)
// {
//     int i;
//     for (i = 0; i < 4; i++) {
//         c[i] =   a[i] + b[i];
//     }
// }

int main(){
    float a[VECTOR_LEN], b[VECTOR_LEN], c[VECTOR_LEN];
    cout << sizeof(float) << endl;
    int i;
    for (i = 0; i < VECTOR_LEN ; i++) {
        a[i] = b[i] = c[i] = 3.2;
    }
    cout << c[0] << endl;
    classes_add(a, b, c);
    cout << c[0] << endl;
}