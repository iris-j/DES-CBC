#ifndef DES_H
#define DES_H
#pragma once
#include<iostream>
#include<bitset>
#include<string>
using namespace std;

//定义全局变量


bitset<32> f_function(const bitset<32> &, const bitset<48> &);
bitset<28> leftshift(bitset<28>, int);
void getsubkey(const bitset<64> &);
bitset<64> string_to_bitset(string);
string bitset_to_string(bitset<64>);
bitset<64> encrypt(const bitset<64> &);
bitset<64> decode(const bitset<64> &);
string bitset_to_binary(bitset<64>);
bitset<64> binary_to_bitset(string);

string generate_01(int);





#endif // DES_H
