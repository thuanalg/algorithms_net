
#include "sp_alg_vector.h"
int main__() {
    int* data = 0;
    int count = 0;
    SP_ALGORITHMS_NET_GENERIC_ST* p = 0;
    int a = 75, b = 10, c = 15, i = 0;
    int arr[5] = {1, 2, 3, 4, 5};
    int arr1[5] = {110, 12, 13, 4, 51};
    sp_apl_vector_append(p, int, &a,1, 2);
    sp_apl_vector_append(p, int, &b,1, 2);
    sp_apl_vector_append(p, int, &a,1, 2);
    sp_apl_vector_append(p, int, &c,1, 2);

    //sp_apl_vector_add(p, int, arr,5, 10);
    //sp_apl_vector_add(p, int, arr1,5, 10);
    sp_apl_vector_append(p, int, arr1,5, 10);
    //sp_apl_vector_add(p, int, arr1,5, 10);
    //sp_apl_vector_add(p, int, arr1,5, 10);

    data = (int*)p->data;
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *( data + i) );
    }
    sp_apl_vector_quicksort(p, sp_apl_quicksort_int);
    spllog(0, "\n\n\n");
    //sp_apl_vector_rem(p, int, 3, 5);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    //sp_apl_vector_clear(p);
    //count = p->pl / sizeof(int);
    //for (i = 0; i < count; ++i) {
    //    spllog(1, "12345678, %d", *(data + i));
    //}
    //sp_apl_vector_append(p, int, arr1, 5, 10);
    //count = p->pl / sizeof(int);
    //for (i = 0; i < count; ++i) {
    //    spllog(1, "12345678, %d", *(data + i));
    //}
    spllog(0, "---------"); spllog(0, ""); spllog(0, ""); spllog(0, "");
    sp_apl_vector_quicksort(p, sp_apl_quicksort_int);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    spllog(0, "--------------------------"); spllog(0, ""); spllog(0, "");
    sp_apl_vector_insert(p, int, arr1, 2, 1, 10);
    sp_apl_vector_insert(p, int, arr1, 2000, 5, 10);
    sp_apl_vector_quicksort(p, sp_apl_quicksort_int);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    spllog(0, "");
    sp_alg_free(p);
    return 0;
}

int main() {
    int* data = 0;
    int count = 0;
    SP_ALGORITHMS_NET_GENERIC_ST* p = 0;
    int a = 75, b = 10, c = 15, i = 0;
    int arr[5] = { 1, 2, 3, 4, 5 };
    int arr1[5] = { 110, 12, 110, 4, 51 };


    sp_apl_vector_append(p, int, arr1, 5, 10);

    data = (int*)p->data;
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    sp_apl_vector_quicksort(p, sp_apl_quicksort_int);
    spllog(0, "\n\n\n");
    //sp_apl_vector_rem(p, int, 3, 5);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }

    spllog(0, "---------"); spllog(0, ""); spllog(0, ""); spllog(0, "");
    sp_apl_vector_quicksort(p, sp_apl_quicksort_int);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    spllog(0, "--------------------------"); spllog(0, ""); spllog(0, "");

    spllog(0, "---------"); spllog(0, ""); spllog(0, ""); spllog(0, "");
    //#define __sp_apl_vector_rem__(__target__, __type__, __index__, __nitem__, __nstep__)
    sp_apl_vector_rem(p, int, 3, 3, 10);
    count = p->pl / sizeof(int);
    for (i = 0; i < count; ++i) {
        spllog(1, "12345678, %d", *(data + i));
    }
    spllog(0, "--------------------------"); spllog(0, ""); spllog(0, "");

    spllog(0, "");
    sp_alg_free(p);
    return 0;
}