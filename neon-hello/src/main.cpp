
#include <stdio.h>
#include <arm_neon.h>


int main(int argc, char **argv) {

	int i;
	unsigned char a[100];
	unsigned char b[100];
	unsigned char res[100];
	unsigned char inx[8] = {4,2,6,4,7,9,0,2};
	for (i=0; i<100; i++)
	{
		a[i] = i;
		b[i] = i + 100;
		res[i] = 211;
	}
	uint8x8x3_t pixel1 = vld3_u8 (a);
	uint8x8x3_t pixel2 = vld3_u8 (b);
	uint8x8_t input = vld1_u8 (b);

	uint8x8_t index = vld1_u8 (inx);

	uint8x8_t result = vtbl3_u8 (pixel2, index);
	vst1_u8 (res, result);

	printf ("Result: ");
	for (i=0; i<32; i++)
	{
		if (!(i%10))
			printf ("\n");

		printf("%d ", res[i]);
	}
	printf ("\n\n");
	return 0;
}
