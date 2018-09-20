#include <stdio.h>

int main() {
int *p_array;
int *g_array;
p_array = (int *)my_bestfit_malloc(sizeof(int)*50);
if (p_array == NULL) {
	printf("Malloc has failed");
	return 1;
}
g_array =(int *) my_bestfit_malloc(sizeof(int)*25);
if (g_array == NULL) {
	printf("Malloc has faild g");
	return 2;
}
printf("%d\n", g_array);
my_free(g_array);
my_free(p_array);
}
