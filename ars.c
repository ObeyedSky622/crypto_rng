#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static const uint8_t sbox[256] = {
    // 0     1    2      3     4    5     6     7      8    9     A      B    C     D     E F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};


#define getSBoxValue(num) (sbox[(num)])
typedef uint8_t state_t[16];
static void SubBytes(state_t* state) {
	uint8_t i, j;
	for (i = 0; i < 16; ++i) {
		(*state)[i] = getSBoxValue((*state)[i]);
	}
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows(state_t* state) {
	uint8_t temp;

	// Rotate first row 1 columns to left
	temp         = (*state)[1];
	(*state)[1]  = (*state)[5];
	(*state)[5]  = (*state)[9];
	(*state)[9]  = (*state)[13];
	(*state)[13] = temp;

	// Rotate second row 2 columns to left
	temp         = (*state)[2];
	(*state)[2]  = (*state)[10];
	(*state)[10] = temp;

	temp         = (*state)[6];
	(*state)[6]  = (*state)[14];
	(*state)[14] = temp;

	// Rotate third row 3 columns to left
	temp         = (*state)[3];
	(*state)[3]  = (*state)[15];
	(*state)[15] = (*state)[11];
	(*state)[11] = (*state)[7];
	(*state)[7]  = temp;
}

static uint8_t xtime(uint8_t x) { return ((x << 1) ^ (((x >> 7) & 1) * 0x1b)); }

// MixColumns function mixes the columns of the state matrix
static void MixColumns(state_t* state) {
	uint8_t i;
	uint8_t Tmp, Tm, t;
	// iteration 1
	t   = (*state)[0];
	Tmp = (*state)[0] ^ (*state)[1] ^ (*state)[2] ^ (*state)[3];
	Tm  = (*state)[0] ^ (*state)[1];
	Tm  = xtime(Tm);
	(*state)[0] ^= Tm ^ Tmp;
	Tm = (*state)[1] ^ (*state)[2];
	Tm = xtime(Tm);
	(*state)[1] ^= Tm ^ Tmp;
	Tm = (*state)[2] ^ (*state)[3];
	Tm = xtime(Tm);
	(*state)[2] ^= Tm ^ Tmp;
	Tm = (*state)[3] ^ t;
	Tm = xtime(Tm);
	(*state)[3] ^= Tm ^ Tmp;
	// iteration 2
	t   = (*state)[4];
	Tmp = (*state)[4] ^ (*state)[5] ^ (*state)[6] ^ (*state)[7];
	Tm  = (*state)[4] ^ (*state)[5];
	Tm  = xtime(Tm);
	(*state)[4] ^= Tm ^ Tmp;
	Tm = (*state)[5] ^ (*state)[6];
	Tm = xtime(Tm);
	(*state)[5] ^= Tm ^ Tmp;
	Tm = (*state)[6] ^ (*state)[7];
	Tm = xtime(Tm);
	(*state)[6] ^= Tm ^ Tmp;
	Tm = (*state)[7] ^ t;
	Tm = xtime(Tm);
	(*state)[7] ^= Tm ^ Tmp;
	// iteration 3 i = 2
	t   = (*state)[8];
	Tmp = (*state)[8] ^ (*state)[9] ^ (*state)[10] ^ (*state)[11];
	Tm  = (*state)[8] ^ (*state)[9];
	Tm  = xtime(Tm);
	(*state)[8] ^= Tm ^ Tmp;
	Tm = (*state)[9] ^ (*state)[10];
	Tm = xtime(Tm);
	(*state)[9] ^= Tm ^ Tmp;
	Tm = (*state)[10] ^ (*state)[11];
	Tm = xtime(Tm);
	(*state)[10] ^= Tm ^ Tmp;
	Tm = (*state)[11] ^ t;
	Tm = xtime(Tm);
	(*state)[11] ^= Tm ^ Tmp;
	// iteration 4 i = 3
	t   = (*state)[12];
	Tmp = (*state)[12] ^ (*state)[13] ^ (*state)[14] ^ (*state)[15];
	Tm  = (*state)[12] ^ (*state)[13];
	Tm  = xtime(Tm);
	(*state)[12] ^= Tm ^ Tmp;
	Tm = (*state)[13] ^ (*state)[14];
	Tm = xtime(Tm);
	(*state)[13] ^= Tm ^ Tmp;
	Tm = (*state)[14] ^ (*state)[15];
	Tm = xtime(Tm);
	(*state)[14] ^= Tm ^ Tmp;
	Tm = (*state)[15] ^ t;
	Tm = xtime(Tm);
	(*state)[15] ^= Tm ^ Tmp;
}
static uint64_t key_split_low(uint8_t* key) {
	uint64_t key_low;

	for (int i = 8; i < 16; i++) {
		key_low = key_low | key[i];
	}
	return key_low;
}

static uint64_t key_split_high(uint8_t* key) {
	uint64_t key_high;

	for (int i = 8; i < 16; i++) {
		key_high = key_high | key[i];
	}
	return key_high;
}

static void key_update(uint64_t key_high, uint64_t key_low) {
	key_low  = key_low + 0x9E3779B97F4A7C15;
	key_high = key_high + 0xBB67AE8584CAA73B;
}
static void cipherFunc(uint8_t* input, uint8_t* ouput, uint8_t* key) {
	uint8_t tempBlock[16];
	for (int i = 0; i < 16; i++) {
		tempBlock[i] = input[i] ^ key[i];
	}
	uint64_t key_low  = key_split_low(key);
	uint64_t key_high = key_split_high(key);

	for (uint8_t round = 0; round < 5; ++round) {
		SubBytes(tempBlock);
		ShiftRows(tempBlock);
		MixColumns(tempBlock);
		key_update(key_high, key_low);
	}
	SubBytes(tempBlock);
	ShiftRows(tempBlock);
	key_update(key_high, key_low);
}
int main() {

	// should work like this:
	// declare an input and pointer for output as well as a key
	uint8_t* output;
	uint8_t input[16];
	uint8_t key[16];
	input[0]  = 0x00;
	input[1]  = 0x01;
	input[2]  = 0x02;
	input[3]  = 0x03;
	input[4]  = 0x04;
	input[5]  = 0x05;
	input[6]  = 0x06;
	input[7]  = 0x07;
	input[8]  = 0x08;
	input[9]  = 0x09;
	input[10] = 0x0a;
	input[11] = 0x0b;
	input[12] = 0x0c;
	input[13] = 0x0d;
	input[14] = 0x0e;
	input[15] = 0x0f;

	key[0]  = 0x0f;
	key[1]  = 0x0e;
	key[2]  = 0x0d;
	key[3]  = 0x0c;
	key[4]  = 0x0b;
	key[5]  = 0x0a;
	key[6]  = 0x09;
	key[7]  = 0x08;
	key[8]  = 0x07;
	key[9]  = 0x06;
	key[10] = 0x05;
	key[11] = 0x04;
	key[12] = 0x03;
	key[13] = 0x02;
	key[14] = 0x01;
	key[15] = 0x00;
	// let cipher do the work, only function not tested are split key functions
	cipherFunc(input, output, key);


	for (int i = 0; i < 16; i++) {
		printf("output[%d]: %x\n", i, output[i]);
	}
	return 0;
}