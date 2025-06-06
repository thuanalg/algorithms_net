#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "simplelog.h"
#include <sp_cppalg.h>
#include <sp_calg.h>
using namespace std;
class AAA {
public:
	AAA();
	~AAA();
protected:
private:
	int a;
	int b;
	std::string str;
};
int main(int argc, char *argv) {
	int k = 0;
	k = sizeof(AAA);
	fprintf(stdout, "kk= :%d, sizeof(std::string): %d.\n", 
		k, sizeof(std::string));
	sp_cppalg_greedy(0, 0, 0, 0);
	return EXIT_SUCCESS;
}