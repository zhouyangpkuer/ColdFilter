#ifndef _PARAMS_H
#define _PARAMS_H

#include <cstdint>

//#define MAX_INSERT_PACKAGE 32000000
//#define MAX_HASH_NUM 20
#define STR_LEN 8

typedef const unsigned char cuc;
typedef unsigned int(*funPtr)(const unsigned char *, unsigned int);

struct HashFunction {
    HashFunction();
    static uint32_t Str2Int(cuc *str, uint32_t hidx, uint32_t len = STR_LEN);
    static funPtr hash_function[14];
};

#define mix(a, b, c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

uint32_t BOB1(cuc *str, uint32_t len) {
    //register ub4 a,b,c,len;
    uint32_t a, b, c;
    uint32_t initval = 2;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16) + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16) + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16) + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch (len)              /* all the case statements fall through */{
    case 11: c += ((uint32_t) str[10] << 24);
    case 10: c += ((uint32_t) str[9] << 16);
    case 9 : c += ((uint32_t) str[8] << 8);
        /* the first byte of c is reserved for the length */
    case 8 : b += ((uint32_t) str[7] << 24);
    case 7 : b += ((uint32_t) str[6] << 16);
    case 6 : b += ((uint32_t) str[5] << 8);
    case 5 : b += str[4];
    case 4 : a += ((uint32_t) str[3] << 24);
    case 3 : a += ((uint32_t) str[2] << 16);
    case 2 : a += ((uint32_t) str[1] << 8);
    case 1 : a += str[0];
        /* case 0: nothing left to add */
    }
    mix(a, b, c);
    /*-------------------------------------------- report the result */
    return c;
}

uint32_t BOB2(cuc *str, uint32_t len) {
    //register ub4 a,b,c,len;
    uint32_t a, b, c;
    uint32_t initval = 31;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16) + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16) + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16) + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch (len)              /* all the case statements fall through */{
    case 11: c += ((uint32_t) str[10] << 24);
    case 10: c += ((uint32_t) str[9] << 16);
    case 9 : c += ((uint32_t) str[8] << 8);
        /* the first byte of c is reserved for the length */
    case 8 : b += ((uint32_t) str[7] << 24);
    case 7 : b += ((uint32_t) str[6] << 16);
    case 6 : b += ((uint32_t) str[5] << 8);
    case 5 : b += str[4];
    case 4 : a += ((uint32_t) str[3] << 24);
    case 3 : a += ((uint32_t) str[2] << 16);
    case 2 : a += ((uint32_t) str[1] << 8);
    case 1 : a += str[0];
        /* case 0: nothing left to add */
    }
    mix(a, b, c);
    /*-------------------------------------------- report the result */
    return c;
}

uint32_t
BOB3(cuc *str, uint32_t len) {
    //register ub4 a,b,c,len;
    uint32_t a, b, c;
    uint32_t initval = 73;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16) + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16) + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16) + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch (len)              /* all the case statements fall through */{
    case 11: c += ((uint32_t) str[10] << 24);
    case 10: c += ((uint32_t) str[9] << 16);
    case 9 : c += ((uint32_t) str[8] << 8);
        /* the first byte of c is reserved for the length */
    case 8 : b += ((uint32_t) str[7] << 24);
    case 7 : b += ((uint32_t) str[6] << 16);
    case 6 : b += ((uint32_t) str[5] << 8);
    case 5 : b += str[4];
    case 4 : a += ((uint32_t) str[3] << 24);
    case 3 : a += ((uint32_t) str[2] << 16);
    case 2 : a += ((uint32_t) str[1] << 8);
    case 1 : a += str[0];
        /* case 0: nothing left to add */
    }
    mix(a, b, c);
    /*-------------------------------------------- report the result */
    return c;
}
uint32_t
BOB4(cuc *str, uint32_t len) {
    //register ub4 a,b,c,len;
    uint32_t a, b, c;
    uint32_t initval = 127;
    /* Set up the internal state */
    //len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* the previous hash value */

    /*---------------------------------------- handle most of the key */
    while (len >= 12) {
        a += (str[0] + ((uint32_t) str[1] << 8) + ((uint32_t) str[2] << 16) + ((uint32_t) str[3] << 24));
        b += (str[4] + ((uint32_t) str[5] << 8) + ((uint32_t) str[6] << 16) + ((uint32_t) str[7] << 24));
        c += (str[8] + ((uint32_t) str[9] << 8) + ((uint32_t) str[10] << 16) + ((uint32_t) str[11] << 24));
        mix(a, b, c);
        str += 12;
        len -= 12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c += len;
    switch (len)              /* all the case statements fall through */{
    case 11: c += ((uint32_t) str[10] << 24);
    case 10: c += ((uint32_t) str[9] << 16);
    case 9 : c += ((uint32_t) str[8] << 8);
        /* the first byte of c is reserved for the length */
    case 8 : b += ((uint32_t) str[7] << 24);
    case 7 : b += ((uint32_t) str[6] << 16);
    case 6 : b += ((uint32_t) str[5] << 8);
    case 5 : b += str[4];
    case 4 : a += ((uint32_t) str[3] << 24);
    case 3 : a += ((uint32_t) str[2] << 16);
    case 2 : a += ((uint32_t) str[1] << 8);
    case 1 : a += str[0];
        /* case 0: nothing left to add */
    }
    mix(a, b, c);
    /*-------------------------------------------- report the result */
    return c;
}

uint32_t RSHash(cuc *str, uint32_t len) {
    uint32_t b = 378551;
    uint32_t a = 63689;
    uint64_t hash = 0;
    for (uint32_t i = 0; i < len; ++i) {
        hash = hash * a + str[i];
        a = a * b;
    }
    return (uint32_t) hash;
}

uint32_t JSHash(cuc *str, uint32_t len) {
    uint64_t hash = 1315423911;
    for (uint32_t i = 0; i < len; ++i) {
        hash ^= ((hash << 5) + str[i] + (hash >> 2));
    }
    return (uint32_t) hash;
}

uint32_t PJWHash(cuc *str, uint32_t len) {
    uint64_t BitsInUnsignedInt = (uint64_t) 32;
    uint64_t ThreeQuarters = (uint64_t) ((BitsInUnsignedInt * 3) / 4);
    uint64_t OneEighth = (uint64_t) (BitsInUnsignedInt / 8);
    uint64_t HighBits = (uint64_t) (0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
    uint64_t hash = 0;
    uint64_t test = 0;
    for (uint32_t i = 0; i < len; i++) {
        hash = (hash << OneEighth) + str[i];
        if ((test = hash & HighBits) != 0) {
            hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
        }
    }
    return (uint32_t) hash;
}

uint32_t ELFHash(cuc *str, uint32_t len) {
    uint64_t hash = 0;
    uint64_t x = 0;
    for (uint32_t i = 0; i < len; ++i) {
        hash = (hash << 4) + str[i];
        if ((x = hash & 0xF0000000L) != 0) {
            hash ^= (x >> 24);
        }
        hash &= ~x;
    }
    return (uint32_t) hash;
}

uint32_t BKDRHash(cuc *str, uint32_t len) {
    uint64_t seed = 131;
    uint64_t hash = 0;
    for (uint32_t i = 0; i < len; ++i) {
        hash = (hash * seed) + str[i];
    }
    return (uint32_t) hash;
}

uint32_t SDBMHash(cuc *str, uint32_t len) {
    uint64_t hash = 0;
    for (uint32_t i = 0; i < len; ++i) {
        hash = str[i] + (hash << 6) + (hash << 16) - hash;
    }
    return (uint32_t) hash;
}

uint32_t DJBHash(cuc *str, uint32_t len) {
    uint64_t hash = 5381;
    for (uint32_t i = 0; i < len; ++i) {
        hash = ((hash << 5) + hash) + str[i];
    }
    return (uint32_t) hash;
}

uint32_t DEKHash(cuc *str, uint32_t len) {
    uint64_t hash = len;
    for (uint32_t i = 0; i < len; ++i) {
        hash = ((hash << 5) ^ (hash << 27)) ^ str[i];
    }
    return (uint32_t) hash;
}

uint32_t APHash(cuc *str, uint32_t len) {
    uint64_t hash = 0xAAAAAAAA;
    for (uint32_t i = 0; i < len; ++i) {
        if ((i & 1) == 0) {
            hash ^= ((hash << 7) ^ str[i] * (hash >> 3));
        } else {
            hash ^= (~(((hash << 11) + str[i]) ^ (hash >> 5)));
        }
    }
    return (uint32_t) hash;
}

unsigned int (*HashFunction::hash_function[])(const unsigned char *, unsigned int) = {
    &BOB1, &BOB2, &BOB3, &BOB4,
    &RSHash, &JSHash,
    &PJWHash, &ELFHash,
    &BKDRHash, &SDBMHash,
    &DJBHash, &DEKHash,
    &APHash, &DJBHash
};

HashFunction::HashFunction() {
/*
	hash_function[0] = &BOB1;
	hash_function[1] = &BOB2;
	hash_function[2] = &BOB3;
	hash_function[3] = &BOB4;
	hash_function[4] = &RSHash;
	hash_function[5] = &JSHash;
	hash_function[6] = &PJWHash;
	hash_function[7] = &ELFHash;
	hash_function[8] = &BKDRHash;
	hash_function[9] = &SDBMHash;
	hash_function[10] = &DJBHash;
	hash_function[11] = &DEKHash;
	hash_function[12] = &APHash;
	hash_function[13] = &DJBHash;
*/
}

uint32_t HashFunction::Str2Int(cuc *str, uint32_t hidx, uint32_t len) {
    return hash_function[hidx](str, len);
}

#endif //_PARAMS_H