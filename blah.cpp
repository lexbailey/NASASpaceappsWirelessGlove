#include <math.h>
#include <stdio.h>

using namespace std;

void blah(){
	int i = 0;
	for(i=0;i>-100;i--){
		printf("Blah %d\n",i);
	}
}

int main(void){
	int i = 0;
	int x = 0;
	//blah();
	while(x<100){
		printf("Main %d\n",i++ %= 5);
		
		x++;
	}
	return 0;
}