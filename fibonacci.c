#include <stdio.h>

int main(){
	int fib1 = 0;
	int fib2 = 1;

	for(int i=0;i<10;++i){
		printf("%d\n",fib1);
		int temp = fib1 + fib2;
		fib1 = fib2;
		fib2 = temp;
	}

	return 0;

}
