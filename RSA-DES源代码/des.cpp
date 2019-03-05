#include "des.h"
#include<iostream>
using namespace std;
#include<bitset>
#include<stdlib.h>
#include<time.h>
extern int E[], P[], IPC[], shiftBitCounts[], PC[], IP[], IP_1[];
extern int S_BOX[8][4][16];
extern string input;
extern string output;
extern bitset<64> initkey;
extern bitset<48> subkey[8];
extern bitset<64> IV;
//F函数
bitset<32> f_function(const bitset<32> &R, const bitset<48> &K){
    bitset<48> expandedR;
    //扩展置换
    for(int i = 0; i < 48; i++)
        expandedR[47-i] = R[31-E[i]];
    //expandedR与K执行异或运算
    expandedR = expandedR ^ K;
    //S盒置换
    bitset<32> output;
    for(int i = 0, j = 0; i < 48; i += 6, j += 4){
        int row = expandedR[47-i]*2 + expandedR[47-i-5];
        int col = expandedR[47-i-1]*8 + expandedR[47-i-2]*4 + expandedR[47-i-3]*2 + expandedR[47-i-4]*1;
        bitset<4> temp(S_BOX[i/6][row][col]);
        output[31-j] = temp[3];
        output[31-j-1] = temp[2];
        output[31-j-2] = temp[1];
        output[31-j-3] = temp[0];

    }
    //P置换
    bitset<32> tmp = output;
    for(int i = 0; i < 32; i++)
        output[31-i] = tmp[31-P[i]];
    return output;


}

//循环左移
bitset<28> leftshift(bitset<28> bits, int count){
    bitset<28> temp;
    for(int i = 0; i < 28; i++)
        temp[i] = bits[27-i];
    temp = (temp >> count) | (temp << (28 - count));
    for(int i = 0; i < 28; i++)
        bits[27-i] = temp[i];
    return bits;

}

//生成子密钥
void getsubkey(const bitset<64> &key){
    //获得C和D
    bitset<56> initkey;
    for(int i = 0; i < 56; i++)
        initkey[55-i] = key[63-IPC[i]];
    bitset<28> C;
    bitset<28> D;
    for(int i = 0; i < 56; i++){
        if(i < 28)
            C[27-i] = initkey[55-i];
        else
            D[55-i] = initkey[55-i];
    }
    //8 rounds
    for(int round = 0; round < 8; round++){
        C = leftshift(C, shiftBitCounts[round]);
        D = leftshift(D, shiftBitCounts[round]);
        bitset<56> newkey;
        for(int i = 0; i < 56; i++){
            if(i < 28)
                newkey[55-i] = C[27-i];
            else
                newkey[55-i] = D[55-i];
        }
        bitset<48> res;
        for(int i = 0; i < 48; i++)
            res[47-i] = newkey[55-PC[i]];
        subkey[round] = res;

    }

}

bitset<64> string_to_bitset(string s){
    bitset<64> res;
    for(int i = 0; i < 8; i++)
        for(int j = 0; j < 8; j++)
            res[i*8+j] = ((s[7-i] >> j) & 0x01);
    return res;

}

string bitset_to_string(bitset<64> bit){
    string res;
    for(int i=63; i>=0; i=i-8){
        int temp=0;
        for(int j=0; j>-8; j--)
            temp = (temp<<1)+bit[i+j];
        res+=(char(temp));
    }

    return res;
}


bitset<64> binary_to_bitset(string s){
    bitset<64> res;
    for(int i=0; i<64; i++)
        res[i] = s[63-i]-'0';
    return res;
}

string bitset_to_binary(bitset<64> bit){
    string res;
    for(int i=63; i>=0; i--)
        res+=bit[i]+'0';
    return res;
}

//DES加密
bitset<64> encrypt(const bitset<64> &text){
    //初始置换IP
    bitset<64> initreplace;
    for(int i = 0; i < 64; i++)
        initreplace[63-i] = text[63-IP[i]];
    //8轮
    bitset<32> currentL;
    bitset<32> currentR;
    bitset<32> nextL;
    for(int i = 0; i < 64; i++)
        if(i<32) currentL[31-i] = initreplace[63-i];
        else currentR[63-i] = initreplace[63-i];

    for(int round = 0; round < 8; round++){
        nextL = currentR;
        currentR = currentL ^ f_function(currentR, subkey[round]);
        currentL = nextL;

    }
    //左右32位合并
    bitset<64> merge;
    for(int i = 0; i < 64; i++)
        if(i<32) merge[63-i] = currentL[31-i];
        else merge[63-i] = currentR[63-i];
    //最后IP_1置换
    bitset<64> res;
    for(int i = 0; i < 64; i++)
        res[63-i] = merge[63-IP_1[i]];
    return res;

}

//DES解密
bitset<64> decode(const bitset<64> &text){
    //初始置换IP
    bitset<64> initreplace;
    for(int i = 0; i < 64; i++)
        initreplace[63-i] = text[63-IP[i]];

    //8轮
    bitset<32> currentL;
    bitset<32> currentR;
    bitset<32> nextR;
    for(int i = 0; i < 64; i++)
        if(i<32) currentL[31-i] = initreplace[63-i];
        else currentR[63-i] = initreplace[63-i];

    for(int round = 0; round < 8; round++){
        nextR = currentL;
        currentL = currentR ^ f_function(currentL, subkey[7-round]);
        currentR = nextR;

    }
    //左右32位合并
    bitset<64> merge;
    for(int i = 0; i < 64; i++)
        if(i<32) merge[63-i] = currentL[31-i];
        else merge[63-i] = currentR[63-i];
    //最后IP_1置换
    bitset<64> res;
    for(int i = 0; i < 64; i++)
        res[63-i] = merge[63-IP_1[i]];
    return res;

}

string generate_01(int len){
    string res;
    srand((unsigned)time(NULL));
    for(int i = 0; i < len; i++)
        res+=char(rand() % 2 + '0');
    return res;
}

