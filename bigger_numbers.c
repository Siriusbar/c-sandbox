#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BIGINT_BASE 1000000000U /* 10^9 */
#define BIGINT_BASE_DIGITS 9
#define BIGINT_MAX_WORDS 256

typedef struct {
    unsigned int words[BIGINT_MAX_WORDS];
    size_t len; /* number of used words */
} BigInt;

static void bigint_trim(BigInt *n) {
    while (n->len > 0 && n->words[n->len - 1] == 0) {
        n->len--;
    }
}

static BigInt bigint_from_uint(unsigned int v) {
    BigInt n;
    n.len = 0;
    if (v > 0) {
        n.words[n.len++] = v;
    }
    return n;
}

static BigInt bigint_from_string(const char *s) {
    BigInt n = bigint_from_uint(0);
    size_t slen = strlen(s);
    size_t i = 0;

    while (i < slen && s[i] == '0') {
        i++;
    }
    if (i == slen) {
        return n;
    }

    for (; i < slen; i++) {
        unsigned int digit = (unsigned int)(s[i] - '0');
        if (digit > 9) {
            fprintf(stderr, "Invalid digit in input: %c\n", s[i]);
            exit(1);
        }
        /* n = n * 10 + digit */
        unsigned long long carry = digit;
        size_t k = 0;
        for (; k < n.len; k++) {
            unsigned long long val = (unsigned long long)n.words[k] * 10ULL + carry;
            n.words[k] = (unsigned int)(val % BIGINT_BASE);
            carry = val / BIGINT_BASE;
        }
        if (carry != 0) {
            if (n.len >= BIGINT_MAX_WORDS) {
                fprintf(stderr, "BigInt overflow (increase BIGINT_MAX_WORDS)\n");
                exit(1);
            }
            n.words[n.len++] = (unsigned int)carry;
        }
        if (n.len == 0) {
            n.len = 1;
            n.words[0] = 0;
        }
    }
    bigint_trim(&n);
    return n;
}

static BigInt bigint_add(const BigInt *a, const BigInt *b) {
    BigInt out = bigint_from_uint(0);
    size_t max_len = a->len > b->len ? a->len : b->len;
    unsigned long long carry = 0;
    size_t i = 0;

    for (; i < max_len; i++) {
        unsigned long long av = i < a->len ? a->words[i] : 0;
        unsigned long long bv = i < b->len ? b->words[i] : 0;
        unsigned long long sum = av + bv + carry;
        if (out.len >= BIGINT_MAX_WORDS) {
            fprintf(stderr, "BigInt overflow (increase BIGINT_MAX_WORDS)\n");
            exit(1);
        }
        out.words[out.len++] = (unsigned int)(sum % BIGINT_BASE);
        carry = sum / BIGINT_BASE;
    }
    if (carry != 0) {
        if (out.len >= BIGINT_MAX_WORDS) {
            fprintf(stderr, "BigInt overflow (increase BIGINT_MAX_WORDS)\n");
            exit(1);
        }
        out.words[out.len++] = (unsigned int)carry;
    }
    bigint_trim(&out);
    return out;
}

static void bigint_print(const BigInt *n) {
    if (n->len == 0) {
        printf("0");
        return;
    }
    printf("%u", n->words[n->len - 1]);
    for (size_t i = n->len - 1; i-- > 0;) {
        printf("%0*u", BIGINT_BASE_DIGITS, n->words[i]);
    }
}

int main(void) {
    const char *a_str = "1234567890123456712312389012345678901234567890";
    const char *b_str = "9876543210987654321123123098765432109876543210";

    BigInt a = bigint_from_string(a_str);
    BigInt b = bigint_from_string(b_str);
    BigInt sum = bigint_add(&a, &b);

    printf("a = ");
    bigint_print(&a);
    printf("\n");

    printf("b = ");
    bigint_print(&b);
    printf("\n");

    printf("a + b = ");
    bigint_print(&sum);
    printf("\n");

    return 0;
}
