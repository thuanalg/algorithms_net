#include <stdio.h>
#include <gmp.h>
#include "gmp_example.h"
int main_01();
int main_library() {
    mpz_t a, b, sum, prod, quo;

    // Khởi tạo các biến GMP (tự động cấp phát bộ nhớ)
    mpz_init(a);
    mpz_init(b);
    mpz_init(sum);
    mpz_init(prod);
    mpz_init(quo);

    // Gán giá trị lớn cho a và b từ chuỗi
    mpz_set_str(a, "987654321987654321987654321", 10);  // Cơ số 10
    mpz_set_str(b, "123456789123456789123456789", 10);

    // Cộng
    mpz_add(sum, a, b);

    // Nhân
    mpz_mul(prod, a, b);

    // Chia
    mpz_tdiv_q(quo, a, b);

    // In kết quả
    printf("a = ");
    mpz_out_str(stdout, 10, a);
    printf("\n");

    printf("b = ");
    mpz_out_str(stdout, 10, b);
    printf("\n");

    printf("a + b = ");
    mpz_out_str(stdout, 10, sum);
    printf("\n");

    printf("a * b = ");
    mpz_out_str(stdout, 10, prod);
    printf("\n");

	

    printf("a / b = ");
    mpz_out_str(stdout, 10, quo);
    printf("\n");

    // Giải phóng bộ nhớ
    mpz_clear(a);
    mpz_clear(b);
    mpz_clear(sum);
    mpz_clear(prod);
    mpz_clear(quo);
	
	main_01();
    return 0;
}

#include <stdio.h>
#include <gmp.h>

int main_01() {
    mpz_t result;

    // Khởi tạo biến kiểu mpz_t
    mpz_init(result);

    // Tính 2^1000: mpz_ui_pow_ui(result, base, exponent)
    mpz_ui_pow_ui(result, 2, 30000);

    // In kết quả ra màn hình
    gmp_printf("2^30000 = %Zd\n", result);

    // Giải phóng vùng nhớ
    mpz_clear(result);

    return 0;
}

void wrap_mpz_init(mpz_ptr a) 	{return mpz_init(a);}
void wrap_mpz_clear(mpz_ptr a)  {return mpz_clear(a);}