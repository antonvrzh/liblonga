#ifndef BIGINT_HEADER
#define BIGINT_HEADER

#define NUM_DIGITS 10 //for optimization(now it is not implemented)
#define BASE       10
#define BUFSIZE    1024

#define PTR_IS_NULL(_ptr_) (NULL == _ptr_)
#define IS_SIGN(_sign_)    ('-' == _sign_)
#define MAX(_a_, _b_)      (_a_ > _b_ ? _a_ : _b_)
#define INT2CHAR(_n_)      (_n_ + '0')
#define CHAR2INT(_s_)      (_s_ - '0')

typedef unsigned int uI;
typedef long long lli;
typedef unsigned long long llu;

typedef struct
{
	bool sign;
	uI len;
	int bigint[BUFSIZE];
} BigInt;

int string_to_bigint(const char* const str, BigInt* const bigint);
char * bigint_to_string(BigInt* const bigint);
int scan_bigint(BigInt* const bigint);
BigInt* init_bigint(const char* digit);
void ded_bigint(BigInt* const a, BigInt* const b, BigInt* const c);
void add_bigint(BigInt* const a, BigInt* const b, BigInt* const c);
int int16_to_bigint(signed short digit, BigInt* const bigint);
int uint16_to_bigint(unsigned short digit, BigInt* const bigint);
int int32_to_bigint(signed long digit, BigInt* const bigint);
int uint32_to_bigint(unsigned long digit, BigInt* const bigint);
int int64_to_bigint(signed long long digit, BigInt* const bigint);
int uint64_to_bigint(unsigned long long digit, BigInt* const bigint);

#endif