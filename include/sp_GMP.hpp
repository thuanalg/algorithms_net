#ifndef __sp_GMP__
#define __sp_GMP__
#pragma warning(disable : 4146)
#include <gmp.h>
#include <stdlib.h>

#define EXPONENT_BILLION	1000000000
int sp_GMP_test()
{
	printf("GMP version: %s\n", gmp_version);
	mpz_t result;
	mpz_init(result);

	mpz_ui_pow_ui(result, 2, EXPONENT_BILLION);
	FILE *fp = fopen("D:/x/result_03.txt", "w+");
	char *data = (char *)malloc(EXPONENT_BILLION);
	memset(data, 0, EXPONENT_BILLION);
	if (fp) {
		size_t ttt = gmp_snprintf(
		    data, EXPONENT_BILLION, "2^EXPONENT_BILLION:\n%Zd\n", result);
		fwrite(data, 1, ttt, fp);
		fclose(fp);
	}
	mpz_clear(result);
	free(data);
	return 0;
}



#endif // __sp_GMP__
