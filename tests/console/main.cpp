#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "simplelog.h"
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
	fprintf(stdout, "kk= :%d.\n", k);
	return EXIT_SUCCESS;
}