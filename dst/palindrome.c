#include <stdio.h>
int num[10];
int weishu;
int main() {
	int n;
	scanf("%d", &n);
	while(n) {
		num[weishu ++] = n % 10;
		n /= 10;
	}
	int flag = 1;
	for(int i = 0; i < weishu / 2; ++ i) {
		if(num[i] != num[weishu - 1 - i])
			flag = 0;
	}
	if (flag) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
