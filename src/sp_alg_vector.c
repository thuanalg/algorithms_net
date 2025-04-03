#include "sp_alg_vector.h"

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
static int sp_apl_quicksort_int_cmp_partition(int *arr, int low, int hi);
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int sp_apl_hello() {
	return 0;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int sp_apl_vector_quicksort(SP_ALGORITHMS_NET_GENERIC_ST* v, SPL_VECTOR_CMP_CALLBACK f) {
	/**/
	do {
		if (!v) {
			break;
		}
		if (v->pl < 1) {
			break;
		}
		f(v->data, 0, v->pl / v->sz - 1);
	} while (0);
	return 0;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
int sp_apl_quicksort_int_cmp(void*arr, int i, int j) {
	//int a = *((int*)arr + i);
	//int b = *((int*)arr +j);
	//return a > b;
	return *((int*)arr + i) > *((int*)arr + j);
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-j+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

int sp_apl_quicksort_int(void* obj, int l, int h)
{
	int ret = 0;
	int* arr = (int*)obj;
	if (l < h)
	{
		int j = sp_apl_quicksort_int_cmp_partition(arr, l, h);
		sp_apl_quicksort_int(obj, l, j - 1);
		sp_apl_quicksort_int(obj, j + 1, h);
	}
	return ret;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-j+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/
#define sp_apl_swap_val_int(__arr__, __i__, __j__) {\
;*(__arr__ + __i__) += *(__arr__ + __j__);\
;*(__arr__ + __j__) = *(__arr__ + __i__) - *(__arr__ + __j__);\
;*(__arr__ + __i__) -= *(__arr__ + __j__);\
;;\
}

int sp_apl_quicksort_int_cmp_partition(int* data, int low, int hi) {
	int i, j;
	i = low + 1;
	j = hi;
	while (i < j)
	{
		while (i < j)
		{
			/* *(data + j) > pivot; */
			if (sp_apl_quicksort_int_cmp(data, low, j)) break;
			j--;
		}
		while (i < j)
		{
			/*  pivot > *(data + i); */
			if (sp_apl_quicksort_int_cmp(data, i, low)) break;
			i++;
		}
		if (i < j) {
			sp_apl_swap_val_int(data, low, j);
		}
	}

	if (i == j)
	{
		//if (data[low] > data[j])
		if ( sp_apl_quicksort_int_cmp( data, low, j) /**/)
		{
			sp_apl_swap_val_int(data, low, j);
		}
		//else if (data[low] < data[j])
		else {
			j--;
		}
	} 

	return j;
}
/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/