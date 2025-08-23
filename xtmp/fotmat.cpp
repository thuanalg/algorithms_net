#if 0
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <format>
#include <stdio.h>
#include <string.h>
using namespace std::chrono;
using namespace std;
#if 1
#define __spl_log_buf_level_cpp20__(__lv__, ___fmttt___, ...) { \
	auto __strcpp20__ = std::format(___fmttt___, ##__VA_ARGS__);   \
		/* fprintf(stdout, "%s\n", __strcpp20__.c_str()); */                                                     \
	}
#endif
#define TEST_NUMBER		1000000
int main()
{
	auto tid = 1000;
	int i = 0;
	int myrange = TEST_NUMBER * 1;
	auto now0 = system_clock::now();
	while (i < myrange) {
		__spl_log_buf_level_cpp20__(
		    0, "[{}] [{}] [TID:{}] {}", 3232, 1, tid, "test");
		++i;
	}
	auto now1 = system_clock::now();
	auto elapsed = now1 - now0;
	return 0;
}
#endif