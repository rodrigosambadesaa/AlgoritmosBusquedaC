#include "big_decimal.h"

#include <ctype.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

static char *str_dup_local(const char *s)
{
    size_t len;
    char *out;

    if (s == NULL)
    {
        return NULL;
    }

    len = strlen(s);
    out = (char *)malloc(len + 1);
    if (out == NULL)
    {
        return NULL;
    }

    memcpy(out, s, len + 1);
    return out;
}

static int is_zero_digits(const char *digits)
{
    size_t i;

    if (digits == NULL)
    {
        return 1;
    }

    for (i = 0; digits[i] != '\0'; i++)
    {
        if (digits[i] != '0')
        {
            return 0;
        }
    }

    return 1;
}

static int bd_normalize(BigDecimal *bd)
{
    size_t len;
    size_t leading;

    if (bd == NULL || bd->digits == NULL)
    {
        return 0;
    }

    len = strlen(bd->digits);
    if (len == 0)
    {
        free(bd->digits);
        bd->digits = str_dup_local("0");
        if (bd->digits == NULL)
        {
            return 0;
        }
        bd->precision = 1;
        bd->scale = 0;
        bd->sign = 1;
        return 1;
    }

    while (bd->scale > 0 && len > 1 && bd->digits[len - 1] == '0')
    {
        bd->digits[len - 1] = '\0';
        len--;
        bd->scale--;
    }

    leading = 0;
    while (leading + 1 < len && bd->digits[leading] == '0')
    {
        leading++;
    }

    if (leading > 0)
    {
        memmove(bd->digits, bd->digits + leading, len - leading + 1);
        len -= leading;
    }

    if (is_zero_digits(bd->digits))
    {
        bd->digits[0] = '0';
        bd->digits[1] = '\0';
        bd->precision = 1;
        bd->scale = 0;
        bd->sign = 1;
        return 1;
    }

    if (bd->scale > (int)len)
    {
        size_t add = (size_t)(bd->scale - (int)len);
        char *new_digits = (char *)malloc(len + add + 1);
        if (new_digits == NULL)
        {
            return 0;
        }
        memset(new_digits, '0', add);
        memcpy(new_digits + add, bd->digits, len + 1);
        free(bd->digits);
        bd->digits = new_digits;
        len += add;
    }

    bd->precision = (int)strlen(bd->digits);
    return 1;
}

static BigDecimal *bd_new_empty(void)
{
    BigDecimal *bd = (BigDecimal *)malloc(sizeof(BigDecimal));

    if (bd == NULL)
    {
        return NULL;
    }

    bd->digits = NULL;
    bd->sign = 1;
    bd->precision = 0;
    bd->scale = 0;
    return bd;
}

static int cmp_int_str(const char *a, const char *b)
{
    size_t la;
    size_t lb;

    if (a == NULL || b == NULL)
    {
        return 0;
    }

    while (*a == '0' && a[1] != '\0')
    {
        a++;
    }
    while (*b == '0' && b[1] != '\0')
    {
        b++;
    }

    la = strlen(a);
    lb = strlen(b);

    if (la > lb)
    {
        return 1;
    }
    if (la < lb)
    {
        return -1;
    }

    if (strcmp(a, b) > 0)
    {
        return 1;
    }
    if (strcmp(a, b) < 0)
    {
        return -1;
    }

    return 0;
}

static char *int_add_str(const char *a, const char *b)
{
    size_t la = strlen(a);
    size_t lb = strlen(b);
    size_t max = (la > lb) ? la : lb;
    char *rev = (char *)malloc(max + 2);
    char *out;
    size_t ia = la;
    size_t ib = lb;
    size_t k = 0;
    int carry = 0;

    if (rev == NULL)
    {
        return NULL;
    }

    while (ia > 0 || ib > 0 || carry != 0)
    {
        int da = (ia > 0) ? (a[--ia] - '0') : 0;
        int db = (ib > 0) ? (b[--ib] - '0') : 0;
        int sum = da + db + carry;
        rev[k++] = (char)('0' + (sum % 10));
        carry = sum / 10;
    }

    out = (char *)malloc(k + 1);
    if (out == NULL)
    {
        free(rev);
        return NULL;
    }

    for (ia = 0; ia < k; ia++)
    {
        out[ia] = rev[k - 1 - ia];
    }
    out[k] = '\0';
    free(rev);
    return out;
}

static char *int_sub_str_abs(const char *a, const char *b)
{
    size_t la = strlen(a);
    size_t lb = strlen(b);
    char *rev = (char *)malloc(la + 1);
    char *out;
    size_t ia = la;
    size_t ib = lb;
    size_t k = 0;
    int borrow = 0;

    if (rev == NULL)
    {
        return NULL;
    }

    while (ia > 0)
    {
        int da = a[--ia] - '0';
        int db = (ib > 0) ? (b[--ib] - '0') : 0;
        int val = da - db - borrow;

        if (val < 0)
        {
            val += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        rev[k++] = (char)('0' + val);
    }

    while (k > 1 && rev[k - 1] == '0')
    {
        k--;
    }

    out = (char *)malloc(k + 1);
    if (out == NULL)
    {
        free(rev);
        return NULL;
    }

    for (ia = 0; ia < k; ia++)
    {
        out[ia] = rev[k - 1 - ia];
    }
    out[k] = '\0';
    free(rev);
    return out;
}

static char *append_zeros(const char *digits, int zeros)
{
    size_t len;
    char *out;

    if (digits == NULL || zeros < 0)
    {
        return NULL;
    }

    len = strlen(digits);
    out = (char *)malloc(len + (size_t)zeros + 1);
    if (out == NULL)
    {
        return NULL;
    }

    memcpy(out, digits, len);
    memset(out + len, '0', (size_t)zeros);
    out[len + (size_t)zeros] = '\0';
    return out;
}

static int compare_abs_aligned(const BigDecimal *a, const BigDecimal *b)
{
    int scale_max;
    int extra_a;
    int extra_b;
    int len_a;
    int len_b;
    int i;
    char da;
    char db;

    scale_max = (a->scale > b->scale) ? a->scale : b->scale;
    extra_a = scale_max - a->scale;
    extra_b = scale_max - b->scale;
    len_a = a->precision + extra_a;
    len_b = b->precision + extra_b;

    if (len_a > len_b)
    {
        return 1;
    }
    if (len_a < len_b)
    {
        return -1;
    }

    for (i = 0; i < len_a; i++)
    {
        da = (i < a->precision) ? a->digits[i] : '0';
        db = (i < b->precision) ? b->digits[i] : '0';

        if (da > db)
        {
            return 1;
        }
        if (da < db)
        {
            return -1;
        }
    }

    return 0;
}

static BigDecimal *bd_add_abs(const BigDecimal *a, const BigDecimal *b, int sign)
{
    BigDecimal *out;
    char *ad;
    char *bd;
    char *sum;
    int scale_max;

    scale_max = (a->scale > b->scale) ? a->scale : b->scale;
    ad = append_zeros(a->digits, scale_max - a->scale);
    bd = append_zeros(b->digits, scale_max - b->scale);
    if (ad == NULL || bd == NULL)
    {
        free(ad);
        free(bd);
        return NULL;
    }

    sum = int_add_str(ad, bd);
    free(ad);
    free(bd);
    if (sum == NULL)
    {
        return NULL;
    }

    out = bd_new_empty();
    if (out == NULL)
    {
        free(sum);
        return NULL;
    }

    out->digits = sum;
    out->sign = sign;
    out->scale = scale_max;
    out->precision = (int)strlen(sum);

    if (!bd_normalize(out))
    {
        bd_free(out);
        return NULL;
    }

    return out;
}

static BigDecimal *bd_sub_abs(const BigDecimal *a, const BigDecimal *b, int sign_if_a_ge_b)
{
    BigDecimal *out;
    char *ad;
    char *bd;
    char *sub;
    int scale_max;
    int cmp;
    const char *top;
    const char *bot;
    int sign;

    scale_max = (a->scale > b->scale) ? a->scale : b->scale;
    ad = append_zeros(a->digits, scale_max - a->scale);
    bd = append_zeros(b->digits, scale_max - b->scale);
    if (ad == NULL || bd == NULL)
    {
        free(ad);
        free(bd);
        return NULL;
    }

    cmp = cmp_int_str(ad, bd);
    if (cmp >= 0)
    {
        top = ad;
        bot = bd;
        sign = sign_if_a_ge_b;
    }
    else
    {
        top = bd;
        bot = ad;
        sign = -sign_if_a_ge_b;
    }

    sub = int_sub_str_abs(top, bot);
    free(ad);
    free(bd);
    if (sub == NULL)
    {
        return NULL;
    }

    out = bd_new_empty();
    if (out == NULL)
    {
        free(sub);
        return NULL;
    }

    out->digits = sub;
    out->sign = sign;
    out->scale = scale_max;
    out->precision = (int)strlen(sub);

    if (!bd_normalize(out))
    {
        bd_free(out);
        return NULL;
    }

    return out;
}

static char *int_mul_str(const char *a, const char *b)
{
    size_t la = strlen(a);
    size_t lb = strlen(b);
    size_t i;
    size_t j;
    int *tmp;
    char *out;
    size_t start = 0;

    tmp = (int *)calloc(la + lb, sizeof(int));
    if (tmp == NULL)
    {
        return NULL;
    }

    for (i = la; i > 0; i--)
    {
        for (j = lb; j > 0; j--)
        {
            int da = a[i - 1] - '0';
            int db = b[j - 1] - '0';
            int sum = tmp[i + j - 1] + da * db;
            tmp[i + j - 1] = sum % 10;
            tmp[i + j - 2] += sum / 10;
        }
    }

    while (start + 1 < la + lb && tmp[start] == 0)
    {
        start++;
    }

    out = (char *)malloc((la + lb - start) + 1);
    if (out == NULL)
    {
        free(tmp);
        return NULL;
    }

    for (i = start; i < la + lb; i++)
    {
        out[i - start] = (char)('0' + tmp[i]);
    }
    out[la + lb - start] = '\0';

    free(tmp);
    return out;
}

static char *strip_leading_zeros_new(const char *s)
{
    size_t i = 0;

    while (s[i] == '0' && s[i + 1] != '\0')
    {
        i++;
    }

    return str_dup_local(s + i);
}

static int int_divmod_str(const char *numerator, const char *denominator, char **q_out)
{
    size_t i;
    size_t nlen;
    char *q;
    char *rem;

    if (cmp_int_str(denominator, "0") == 0)
    {
        return 0;
    }

    nlen = strlen(numerator);
    q = (char *)malloc(nlen + 1);
    rem = str_dup_local("0");
    if (q == NULL || rem == NULL)
    {
        free(q);
        free(rem);
        return 0;
    }

    for (i = 0; i < nlen; i++)
    {
        char *rem10;
        char add_digit[2];
        char *new_rem;
        int digit = 0;

        rem10 = append_zeros(rem, 1);
        free(rem);
        if (rem10 == NULL)
        {
            free(q);
            return 0;
        }
        rem = rem10;
        rem[strlen(rem) - 1] = '0';

        add_digit[0] = numerator[i];
        add_digit[1] = '\0';
        new_rem = int_add_str(rem, add_digit);
        free(rem);
        if (new_rem == NULL)
        {
            free(q);
            return 0;
        }
        rem = new_rem;

        while (cmp_int_str(rem, denominator) >= 0)
        {
            char *tmp = int_sub_str_abs(rem, denominator);
            if (tmp == NULL)
            {
                free(rem);
                free(q);
                return 0;
            }
            free(rem);
            rem = tmp;
            digit++;
        }

        q[i] = (char)('0' + digit);
    }

    q[nlen] = '\0';
    free(rem);

    *q_out = strip_leading_zeros_new(q);
    free(q);
    return *q_out != NULL;
}

BigDecimal *bd_create_from_str(const char *str)
{
    BigDecimal *bd;
    size_t i;
    size_t len;
    int sign = 1;
    int seen_dot = 0;
    int scale = 0;
    int started = 0;
    char *digits;
    size_t k = 0;

    if (str == NULL)
    {
        return NULL;
    }

    while (isspace((unsigned char)*str))
    {
        str++;
    }

    if (*str == '+')
    {
        str++;
    }
    else if (*str == '-')
    {
        sign = -1;
        str++;
    }

    len = strlen(str);
    digits = (char *)malloc(len + 2);
    if (digits == NULL)
    {
        return NULL;
    }

    for (i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '.')
        {
            if (seen_dot)
            {
                free(digits);
                return NULL;
            }
            seen_dot = 1;
            continue;
        }

        if (!isdigit((unsigned char)str[i]))
        {
            if (isspace((unsigned char)str[i]))
            {
                break;
            }
            free(digits);
            return NULL;
        }

        digits[k++] = str[i];
        started = 1;
        if (seen_dot)
        {
            scale++;
        }
    }

    while (str[i] != '\0')
    {
        if (!isspace((unsigned char)str[i]))
        {
            free(digits);
            return NULL;
        }
        i++;
    }

    if (!started)
    {
        free(digits);
        return NULL;
    }

    digits[k] = '\0';

    bd = bd_new_empty();
    if (bd == NULL)
    {
        free(digits);
        return NULL;
    }

    bd->digits = digits;
    bd->sign = sign;
    bd->precision = (int)k;
    bd->scale = scale;

    if (!bd_normalize(bd))
    {
        bd_free(bd);
        return NULL;
    }

    return bd;
}

BigDecimal *bd_create_from_int(long long value)
{
    char buffer[64];

    snprintf(buffer, sizeof(buffer), "%lld", value);
    return bd_create_from_str(buffer);
}

void bd_free(BigDecimal *bd)
{
    if (bd == NULL)
    {
        return;
    }

    free(bd->digits);
    bd->digits = NULL;
    free(bd);
}

BigDecimal *bd_add(const BigDecimal *a, const BigDecimal *b)
{
    if (a == NULL || b == NULL)
    {
        return NULL;
    }

    if (a->sign == b->sign)
    {
        return bd_add_abs(a, b, a->sign);
    }

    return bd_sub_abs(a, b, a->sign);
}

BigDecimal *bd_subtract(const BigDecimal *a, const BigDecimal *b)
{
    BigDecimal temp;

    if (a == NULL || b == NULL)
    {
        return NULL;
    }

    temp = *b;
    temp.sign = -temp.sign;
    return bd_add(a, &temp);
}

BigDecimal *bd_multiply(const BigDecimal *a, const BigDecimal *b)
{
    BigDecimal *out;
    char *digits;

    if (a == NULL || b == NULL)
    {
        return NULL;
    }

    digits = int_mul_str(a->digits, b->digits);
    if (digits == NULL)
    {
        return NULL;
    }

    out = bd_new_empty();
    if (out == NULL)
    {
        free(digits);
        return NULL;
    }

    out->digits = digits;
    out->sign = a->sign * b->sign;
    out->scale = a->scale + b->scale;
    out->precision = (int)strlen(digits);

    if (!bd_normalize(out))
    {
        bd_free(out);
        return NULL;
    }

    return out;
}

BigDecimal *bd_divide(const BigDecimal *a, const BigDecimal *b, int limit_precision)
{
    BigDecimal *out;
    char *num;
    char *den;
    char *quot;

    if (a == NULL || b == NULL || limit_precision < 0)
    {
        return NULL;
    }

    if (cmp_int_str(b->digits, "0") == 0)
    {
        return NULL;
    }

    num = append_zeros(a->digits, b->scale + limit_precision);
    den = append_zeros(b->digits, a->scale);
    if (num == NULL || den == NULL)
    {
        free(num);
        free(den);
        return NULL;
    }

    if (!int_divmod_str(num, den, &quot))
    {
        free(num);
        free(den);
        return NULL;
    }

    free(num);
    free(den);

    out = bd_new_empty();
    if (out == NULL)
    {
        free(quot);
        return NULL;
    }

    out->digits = quot;
    out->sign = a->sign * b->sign;
    out->scale = limit_precision;
    out->precision = (int)strlen(quot);

    if (!bd_normalize(out))
    {
        bd_free(out);
        return NULL;
    }

    return out;
}

int bd_compare(const BigDecimal *a, const BigDecimal *b)
{
    int cmp_abs;

    if (a == NULL || b == NULL)
    {
        return 0;
    }

    if (a->sign > b->sign)
    {
        return 1;
    }
    if (a->sign < b->sign)
    {
        return -1;
    }

    cmp_abs = compare_abs_aligned(a, b);
    if (a->sign < 0)
    {
        return -cmp_abs;
    }

    return cmp_abs;
}

char *bd_to_string(const BigDecimal *bd)
{
    size_t len_digits;
    size_t out_len;
    char *out;
    size_t i = 0;
    int int_digits;

    if (bd == NULL || bd->digits == NULL)
    {
        return NULL;
    }

    len_digits = strlen(bd->digits);
    out_len = len_digits + 4;
    if (bd->scale > bd->precision)
    {
        out_len += (size_t)(bd->scale - bd->precision);
    }

    out = (char *)malloc(out_len);
    if (out == NULL)
    {
        return NULL;
    }

    if (bd->sign < 0 && !is_zero_digits(bd->digits))
    {
        out[i++] = '-';
    }

    int_digits = bd->precision - bd->scale;
    if (bd->scale == 0)
    {
        memcpy(out + i, bd->digits, len_digits + 1);
        return out;
    }

    if (int_digits > 0)
    {
        memcpy(out + i, bd->digits, (size_t)int_digits);
        i += (size_t)int_digits;
        out[i++] = '.';
        memcpy(out + i, bd->digits + int_digits, (size_t)bd->scale);
        i += (size_t)bd->scale;
    }
    else
    {
        int zeros = -int_digits;
        out[i++] = '0';
        out[i++] = '.';
        while (zeros-- > 0)
        {
            out[i++] = '0';
        }
        memcpy(out + i, bd->digits, len_digits);
        i += len_digits;
    }

    out[i] = '\0';
    return out;
}

void bd_print(const BigDecimal *bd)
{
    char *s = bd_to_string(bd);

    if (s == NULL)
    {
        printf("(null)");
        return;
    }

    printf("%s", s);
    free(s);
}
