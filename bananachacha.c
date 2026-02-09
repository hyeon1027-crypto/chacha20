#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>

uint32_t a, b, c, d = 0;

uint8_t buffer[64] = { 0, };

uint32_t matrix[16] = { 0, };

uint32_t keystream[16] = { 0, };

uint8_t key[32] = 
{ 
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};
uint32_t counter = 1;
uint8_t nonce[12] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a, 0x00, 0x00, 0x00, 0x00 };

uint8_t tmp[64] = { 0, };

uint32_t state[16] = 
{
	0x61707865, 0x3320646e, 0x79622d32, 0x6b206574
};

void init_state()
{
	for (int i = 0; i < 8; i++)
	{
		state[4 + i] = (uint32_t)key[0 + (4 * i)] | ((uint32_t)key[1 + (4 * i)] << 8) | ((uint32_t)key[2 + (4 * i)] << 16) | ((uint32_t)key[3 + (4 * i)] << 24);
	}

	state[12] = counter;

	for (int i = 0; i < 3; i++)
	{
		state[13 + i] = (uint32_t)nonce[0 + (4 * i)] | ((uint32_t)nonce[1 + (4 * i)] << 8) | ((uint32_t)nonce[2 + (4 * i)] << 16) | ((uint32_t)nonce[3 + (4 * i)] << 24);
	}

	for (int i = 0; i < 16; i++)
	{
		matrix[i] = state[i];
	}
}

void quarter_round(int e, int f, int g, int h)
{
	/*
	a = 0x11111111;
	b = 0x01020304;
	c = 0x9b8d6f43;
	d = 0x01234567;
	*/

	a = state[e];
	b = state[f];
	c = state[g];
	d = state[h];

	/*
	printf("%08x %08x %08x %08x", a, b, c, d);
	printf("\n\n");
	*/

	a += b; d ^= a; d = (d << 16) | (d >> (32 - 16));
	c += d; b ^= c; b = (b << 12) | (b >> (32 - 12));
	a += b; d ^= a; d = (d << 8) | (d >> (32 - 8));
	c += d; b ^= c; b = (b << 7) | (b >> (32 - 7));

	state[e] = a;
	state[f] = b;
	state[g] = c;
	state[h] = d;

	printf("%08x %08x %08x %08x\n", a, b, c, d);
}

void quarter_decode(int e, int f, int g, int h)
{
	/*
	a = 0x11111111;
	b = 0x01020304;
	c = 0x9b8d6f43;
	d = 0x01234567;
	*/

	a = state[e];
	b = state[f];
	c = state[g];
	d = state[h];

	/*
	printf("%08x %08x %08x %08x", a, b, c, d);
	printf("\n\n");
	*/

	b = (b << (32 - 7)) | (b >> 7); b ^= c; c -= d;
	d = (d << (32 - 8)) | (d >> 8); d ^= a; a -= b;
	b = (b << (32 - 12)) | (b >> 12); b ^= c; c -= d;
	d = (d << (32 - 16)) | (d >> 16); d ^= a; a -= b;
	
	/*
	a -= b; d ^= a; d = (d << (32 - 16)) | (d >> 16);
	c -= d; b ^= c; b = (b << (32 - 12)) | (b >> 12);
	a -= b; d ^= a; d = (d << (32 - 8)) | (d >> 8);
	c -= d; b ^= c; b = (b << (32 - 7)) | (b >> 7);
	*/

	state[e] = a;
	state[f] = b;
	state[g] = c;
	state[h] = d;

	printf("%08x %08x %08x %08x\n", a, b, c, d);
}

void column_round()
{
	printf("\n\nColumn Round\n\n");
	quarter_round(0, 4, 8, 12);
	quarter_round(1, 5, 9, 13);
	quarter_round(2, 6, 10, 14);
	quarter_round(3, 7, 11, 15);
}

void diagonal_round()
{
	printf("\n\nDiagonal Round\n\n");
	quarter_round(0, 5, 10, 15);
	quarter_round(1, 6, 11, 12);
	quarter_round(2, 7, 8, 13);
	quarter_round(3, 4, 9, 14);
}

void KeyStream()
{
	for (int i = 0; i < 16; i++)
	{
		keystream[i] = state[i] + matrix[i];
	}

	for (int j = 0; j < 16; j++)
	{
		tmp[0 + (4 * j)] = ((keystream[j]) & 0xFF);
		tmp[1 + (4 * j)] = ((keystream[j] >> 8) & 0xFF);
		tmp[2 + (4 * j)] = ((keystream[j] >> 16) & 0xFF);
		tmp[3 + (4 * j)] = ((keystream[j] >> 24) & 0xFF);
	}

	printf("\n\nChange_little_endian\n\n");

	for (int k = 0; k < 4; k++)
	{
		printf("%02x%02x%02x%02x ", tmp[0 + (16 * k)], tmp[1 + (16 * k)], tmp[2 + (16 * k)], tmp[3 + (16 * k)]);
		printf("%02x%02x%02x%02x ", tmp[4 + (16 * k)], tmp[5 + (16 * k)], tmp[6 + (16 * k)], tmp[7 + (16 * k)]);
		printf("%02x%02x%02x%02x ", tmp[8 + (16 * k)], tmp[9 + (16 * k)], tmp[10 + (16 * k)], tmp[11 + (16 * k)]);
		printf("%02x%02x%02x%02x\n", tmp[12 + (16 * k)], tmp[13 + (16 * k)], tmp[14 + (16 * k)], tmp[15 + (16 * k)]);
	}

	for (int i = 0; i < strlen(buffer); i++)
	{
		buffer[i] = tmp[i] ^ buffer[i];
	}
}

int main()
{
	// 암호화 과정
	init_state();

	for (int i = 0; i < 10; i++)
	{
		column_round();
		diagonal_round();
	}

	printf("\n\nBefore_Result\n\n");

	for (int k = 0; k < 4; k++)
	{
		printf("%08x %08x %08x %08x\n", state[0 + (4 * k)], state[1 + (4 * k)], state[2 + (4 * k)], state[3 + (4 * k)]);
	}

	KeyStream();


	return 0;
}