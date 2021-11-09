#include <ctime>
#include <stdio.h>
#include<iostream>
using namespace std;
class encryption {
private:
	struct key
	{
		unsigned int Key[8];
	};

	struct block
	{
		unsigned int Left, Right;
	};
	int pi0[16] = { 12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1 };
	int pi1[16] = { 6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15 };
	int pi2[16] = { 11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0 };
	int pi3[16] = { 12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11 };
	int pi4[16] = { 7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12 };
	int pi5[16] = { 5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0 };
	int pi6[16] = { 8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7 };
	int pi7[16] = { 1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2 };
	int* pi[8] = { pi0, pi1, pi2, pi3, pi4, pi5, pi6, pi7 };
	unsigned int table(unsigned int x)
	{
		unsigned int y = 0;
		char j;
		int x1 = 0, x2 = 0;
		for (int i = 0; i <= 4; i++)
		{
			j = (x << (8 * (i + 1)));
			x1 = j >> 4;
			x2 = j << 4;
			y <<= 4;
			y ^= pi[2 * i][x1];
			y <<= 4;
			y ^= pi[2 * i + 1][x2];
		}
		return y;
	}

	unsigned int shift11(unsigned int x)
	{
		return ((x << 11) ^ (x >> (32 - 11)));
	};


	block Magma_encrypt(block X, key Key) {
		for (int i = 0; i < 23; i++) {													// (L,R) -> (R,L) -> (R, L  after the transformation)
			swap(X.Right, X.Left);
			X.Right = X.Right ^ shift11(table(X.Left + Key.Key[i % 8]));
		}
		for (int i = 7; i > 0; i--) {													//reverse order key
			swap(X.Right, X.Left);
			X.Right = X.Left ^ shift11(table(X.Left + Key.Key[i]));
		}

		X.Left = X.Left ^ shift11(table(X.Right + Key.Key[0]));							//(L,R) -> (L  after the transformation, R)
		return X;
	}

	block Magma_decrypt(block X, key Key)
	{
		for (int i = 0; i < 8; i++)													//reverse order key
		{
			swap(X.Right, X.Left);
			X.Right = X.Right ^ shift11(table(X.Left + Key.Key[i % 8]));
		}

		for (int i = 23; i > 0; i--) {												//direct order 	
			swap(X.Right, X.Left);
			X.Right = X.Right ^ shift11(table(X.Left + Key.Key[i % 8]));
		}
		X.Left = X.Left ^ shift11(table(X.Right + Key.Key[0]));						//(L,R) -> (L  after the transformation, R)

		return X;
	}


public:

};
int main() {

	unsigned int start_time = clock();







	unsigned int end_time = clock();
	unsigned int search_time = end_time - start_time;
	return 0;
}