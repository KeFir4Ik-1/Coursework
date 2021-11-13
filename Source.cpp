
#include <stdio.h>
#include<iostream>
#include <chrono>
#include <algorithm>


//вынести в .h/.cpp
struct key
{
	unsigned int Key[8];
};


//вынести в .h/.cpp
//и почитайте про union
union block
{
	struct {
		unsigned int Left, Right;
	};
	unsigned char bytes[8];
	unsigned int uints[2];
	unsigned long long int ulong;
};

bool operator==(const block& a, const block& b) {
	return a.ulong == b.ulong;
}


//вынести в .h/.cpp
using namespace std;
class encryption {
private:
	/*таблица должна быть static - поскольку она одна для всех объектов класса.
	и constexpr - поскольку она известна на момент сборки и потом николда не меняется.
	если таблица constexpr - компилятор и будет эту таблицу создавать, он просто радостно подставит необходимые значения где надо*/

	/*спросите Глазкова, как можно эту таблицу превратить в [4][256]*/
	static constexpr int pi[8][16] = {
	 { 12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1 },
	 { 6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15 },
	 { 11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0 },
	 { 12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11 },
	 { 7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12 },
	 { 5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0 },
	 { 8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7 },
	 { 1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2 }
	};

	key _key;

	//от объекта не зависит -> static
	// если изменить табличку на [8][256], то можно реальзовать проще
	static unsigned int table(unsigned int x)
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


	// constexpr, т.к. выход функции жестко зависит от простого аргумента. 
	// constexpr облегчает работу компилятора
	static constexpr unsigned int shift11(unsigned int x) 
	{
		return ((x << 11) ^ (x >> (32 - 11)));
	};


	block Magma_encrypt(block X, key Key) {//лучше void(block& x). ключ должен быть полем объекта     
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

	block Magma_decrypt(block X, key Key) //лучше void(block& x). ключ должен быть полем объекта 
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
	encryption(const key& key) {
		_key = key;
	}

	void encrypt(block* b, size_t blocks_count) {
		// ???
	}

	void decrypt(block* b, size_t blocks_count) {
		// ???
	}
};


block random_block() {
	block b;
	b.Left = rand();
	b.Right = rand();
	return b;
}

std::unique_ptr<block[]> get_random_data(size_t size) {
	auto data = std::make_unique<block[]>(size);
	std::generate_n(data.get(), size, random_block); //заполняет массив случайными числами
	return data;
}

std::unique_ptr<block[]> get_copy(const block* data, size_t size) {
	auto copy = std::make_unique<block[]>(size);
	std::copy_n(data, size, copy.get()); //копирует из одног массива в другой
	return copy;
}

using milliseconds = std::chrono::duration<float, std::milli>; //миллисекунды с дробной частью
int main() {
	const size_t count = 1024;
	auto data = get_random_data(count);
	auto copy = get_copy(data.get(), count);
	key k{}; /*ключ из 0*/
	encryption e(k);

	//в С++ время замеряется вот так
	auto start = std::chrono::high_resolution_clock::now(); 
	e.encrypt(copy.get(), count);;
	auto end = std::chrono::high_resolution_clock::now();
	//end-start будет по умолчанию в наносекундах
	auto time = std::chrono::duration_cast<milliseconds>(end - start); //приводим наносекунды к нашим миллисекундам
	std::cout << "Encryption time " << time.count() << " ms" << std::endl; 

	start = std::chrono::high_resolution_clock::now(); 
	e.decrypt(copy.get(), count);;
	end = std::chrono::high_resolution_clock::now();
	time = std::chrono::duration_cast<milliseconds>(end - start);
	std::cout << "Decryption time " << time.count() << " ms" << std::endl;

	bool ok = std::equal( //проверка на равенство начальных данных и данных после шифрования/расшифрования
		data.get(), data.get() + count, //начальный и конечный итераторы массива 1
		copy.get(), copy.get() + count  //начальный и конечный итераторы массива 2
	);

	std::cout << (ok ? "Result is ok" : "!!!!Result is WRONG!!!!!")<<std::endl;
}