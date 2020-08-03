#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "bigint.h"

static int parce_bigint(const char* digit_str, BigInt* bigint)
{
	if (PTR_IS_NULL(digit_str) || PTR_IS_NULL(bigint)) return -1;

	int i, j;
	for (i = 0, j = bigint->sign ? 1 : 0; i < bigint->len; i++, j++)
		*(bigint->bigint + i) = CHAR2INT(*(digit_str + j));

	return 0;
}

static bool get_sign(const char* digit)
{
	return IS_SIGN(*digit) ? 1 : 0;
}

static void reverse(char* digit, uI len)
{
	char* start = digit;
	char* end = digit + (len - 1);

	while (start < end) {
		*start ^= *end;
		*end ^= *start;
		*start ^= *end;

		start++;
		end--;
	}
}

static uI get_len(char* const digit, bool sign, bool is_stdin)
{
	if (PTR_IS_NULL(digit)) return -1;

	uI len = strlen(digit);

	if (!is_stdin)
		if (sign) return len - 1; // skip '-'

	//if we came here from scanf. For example: 19890\n\0
	if ('\n' == *(digit + (len - 1))) {
		digit[len - 1] = '\0'; //skip '\n'

		if (sign) return len - 2; // 'skip 'minus' and '\0';
		else      return len - 1; // 'skip '\0';
	}
	else {
		if (sign) return len - 1;
	}

	return len;
}

static int digit_is_valid(const char* digit, bool sign, uI len)
{
	if (PTR_IS_NULL(digit)) return 0;

	if (sign) {
		if (!isdigit(*(digit + 1)) || '0' == *(digit + 1))
			goto invalid;
	}
	else {
		if (1 < len && '0' == *digit)
			goto invalid;
	}

	int idx;
	for (idx = sign ? 1 : 0; idx < len; idx++)
		if (!isdigit(*(digit + idx))) goto invalid;

	return 1;

invalid:
		fprintf(stderr, "Invalid digit, check input valid\n");
		return 0;
}

static uI discard_unnecessary_zeros(const BigInt* const c, uI len)
{
	uI null = 0;
	int first_digit = *(c->bigint + (len - 1));
	int* bigint = c->bigint;

	if (0 == first_digit) {
		int idx;
		for (idx = len - 1; idx >= 0; idx--) {
			if (0 == *(bigint + idx))
				null++;
			else
				return len - null;
		}
	}
	else {
		return len;
	}
}

static int initialize(char* const digit, BigInt * const bigint, bool is_stdin) 
{
	if (PTR_IS_NULL(digit) || PTR_IS_NULL(bigint)) return -1;

	bigint->sign = get_sign(digit);
	bigint->len  = get_len(digit, bigint->sign, is_stdin);

	if (!digit_is_valid(digit, bigint->sign, bigint->len)) {
		fprintf(stderr, "digit_is_valid failed\n");
		return -1;
	}

	char* reverse_str = digit;
	if (bigint->sign)
		reverse_str = digit + 1; //skip '-'

	reverse(reverse_str, bigint->len);

	if (0 != parce_bigint(digit, bigint)) {
		fprintf(stderr, "parce_bigint failed\n");
		return -1;
	}

	return 0;
}

int uint64_to_bigint(unsigned long long digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%llu", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int int64_to_bigint(signed long long digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%lli", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int uint32_to_bigint(unsigned long digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%lu", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int int32_to_bigint(signed long digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%li", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int uint16_to_bigint(unsigned short digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%hu", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int int16_to_bigint(signed short digit, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char buf[BUFSIZE] = "";
	sprintf(buf, "%hi", digit);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

static int bigint_max(const BigInt* const a, const BigInt* const b)
{
	uI len = MAX(a->len, b->len);

	int idx;
	for (idx = len - 1; idx >= 0; idx--)
		if (*(a->bigint + idx) > (*(b->bigint + idx)))
			return 1;
		else if (*(b->bigint + idx) > (*(a->bigint + idx)))
			return 2;

	return 0;
}

void add_bigint(BigInt* const a, BigInt* const b, BigInt* const c)
{
	uI len = MAX(a->len, b->len);

	int ab = 0;

	if (a->sign && b->sign) c->sign = 1;

	if (a->sign && !b->sign) {
		a->sign = 0;
		b->sign = 0;

		ded_bigint(a, b, c);

		if (a->len > b->len || 1 == bigint_max(a, b)) {
			c->sign = 1;
		}
		else if (b->len > a->len || 2 == bigint_max(a, b)) {
			c->sign = 0;
		}
		else if (a->len == b->len) {
			c->len = 1; //for print 0
			c->sign = 0;
		}

		return;
	}

	if (!a->sign && b->sign) {
		a->sign = 0;
		b->sign = 0;

		ded_bigint(a, b, c);

		if (a->len > b->len || 1 == bigint_max(a, b)) {
			c->sign = 0;
		}
		else if (b->len > a->len || 2 == bigint_max(a, b)) {
			c->sign = 1;
		}
		else if (a->len == b->len) {
			c->sign = 0;
			c->len = 1; //for print 0
		}

		return;
	}

	int* bp = b->bigint;
	int* ap = a->bigint;
	int* cp = c->bigint;

	int idx;
	for (idx = 0; idx < len; idx++) {
		ab = *(ap + idx) + *(bp + idx);

		if (BASE <= ab) {
			*(cp + idx) += ab % BASE;
			*(cp + (idx + 1)) = ab / BASE;
		}
		else {
			*(cp + idx) += ab % BASE;
			if (BASE <= *(cp + idx)) {
				*(cp + (idx + 1)) = *(cp + idx) / BASE;
				*(cp + idx) = *(cp + idx) % BASE;
			}
		}
	}

	c->len = *(cp + idx) ? len + 1 : len;
}

void ded_bigint(BigInt* const a, BigInt* const b, BigInt* const c)
{
	uI len = MAX(a->len, b->len);

	int* ap = NULL;
	int* bp = NULL;

	//'a->bigint' and 'b->bigint' are positive digits -> sign == 0;
	if (!a->sign && !b->sign) {
		if (a->len < b->len || 2 == bigint_max(a, b)) {
			ap = b->bigint;
			bp = a->bigint;
			c->sign = 1;
		}
		else if (a->len > b->len || 1 == bigint_max(a, b)) {
			ap = a->bigint;
			bp = b->bigint;
			c->sign = 0;
		}
		else if (a->len == b->len && 0 == bigint_max(a, b)){
			*c->bigint = 0;
			c->sign = 0;
			c->len = 0;
			return;
		}
	}

	//'a->bigint' and 'b->bigint' are negative digits -> sign == 1;
	if (a->sign && b->sign) {
		//a = -555; b = -25; (-555 - (-25) -> -555 + 25 = -530 - > sign = 1(minus))
		if (a->len > b->len || 1 == bigint_max(a, b)) {
			ap = a->bigint;
			bp = b->bigint;
			c->sign = 1;
		}
		//a = -25; b = -555;
		else if (b->len > a->len || 2 == bigint_max(a, b)) {
			ap = b->bigint;
			bp = a->bigint;
			c->sign = 0;
		}
		//a = -555; b = -555;
		else if (b->len == a->len) {
			*c->bigint = 0;
			c->len = 1;
			c->sign = 0;
			return;
		}
	}

	//'a->bigint' is a negative digit -> sign == 1;
	//'b->bigint' is a positive digit -> sign == 0;
	//or
	//'a->bigint' is a positive digit -> sign == 0;
	//'b->bigint' is a negative digit -> sign == 1;
	if (a->sign && !b->sign || !a->sign && b->sign) {
		//For example: -555 - 25 = -580 - > sign = 1(minus))
		add_bigint(a, b, c);

		if (a->sign)
			c->sign = 1;
		else
			c->sign = 0;

		c->len = discard_unnecessary_zeros(c, c->len);
		return;
	}

	int* cp = c->bigint;
	int ab = 0;
	int idx;

	for (idx = 0; idx < len; idx++) {
		ab = *(ap + idx) - *(bp + idx);

		if (0 > ab) {
			*(cp + idx) = (*(ap + idx) + BASE) - *(bp + idx);
			*(ap + (idx + 1)) -= 1; 
		}
		else {
			*(cp + idx) = ab;
		}
	}

	uI current_len = len;
	c->len = discard_unnecessary_zeros(c, current_len);
}

/*!! This function allocates some memory, but don't free
/*Need to free memory after using this function !!*/
BigInt* init_bigint(const char* digit)
{
	if (PTR_IS_NULL(digit)) return NULL;

	BigInt* bigint = malloc(sizeof(BigInt));
	assert(NULL != bigint);

	char buf[BUFSIZE];
	memcpy(buf, digit, BUFSIZE);

	if (0 != initialize(buf, bigint, 0)) {
		fprintf(stderr, "intit_bgi failed\n");
		return NULL;
	}

	return bigint;
}

int scan_bigint(BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return -1;

	char digit[BUFSIZE] = "";
	if (!fgets(digit, sizeof(digit), stdin)) {
		fprintf(stderr, "fgets failed\n");
		return -1;
	}

	if (0 != initialize(digit, bigint, 1)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

/*!! This function allocates some memory, but don't free
/*Need to free memory after using this function !!*/
char * bigint_to_string(BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint)) return NULL;

	if (!bigint->len) {
		*bigint->bigint = 0;
		bigint->len += 1;
	}

	size_t malloc_len = (size_t)bigint->len;

	if (bigint->sign) malloc_len += 1; //'-'

	char* str = malloc(malloc_len);
	assert(NULL != str);

	int i, j;
	for (i = bigint->sign ? 1 : 0, j = 0; j < bigint->len; i++, j++)
		*(str + i) = INT2CHAR(*(bigint->bigint + j));

	if (bigint->sign) {
		reverse(str + 1, bigint->len);
		*str = '-';
		return str;
	}

	reverse(str, bigint->len);
	return str;
}

int string_to_bigint(const char* const str, BigInt* const bigint)
{
	if (PTR_IS_NULL(bigint) || PTR_IS_NULL(str)) return -1;

	char buff[BUFSIZE] = "";
	memcpy(buff, str, strlen(str));

	if (0 != initialize(buff, bigint, 0)) {
		fprintf(stderr, "initialize failed\n");
		return -1;
	}

	return 0;
}

int main(void)
{
	//TESTS
	BigInt a = {0};
	BigInt b = {0};
	BigInt c = {0};

	// if (0 != scan_bigint(&a) || 0 != scan_bigint(&b))
	// {
	// 	fprintf(stderr, "la_scanf_stdin failed\n");
	// 	return -1;
	// }

	unsigned int num = 9090;

	uint32_to_bigint(num, &b);

	string_to_bigint("-15999", &a);
	//string_to_bigint("9090", &b);

	add_bigint(&a, &b, &c);

	printf("result: %s\n", bigint_to_string(&c));
	//free(str);

	return 0;
}