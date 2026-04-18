#ifndef BIG_DECIMAL_H
#define BIG_DECIMAL_H

#include <stdbool.h>
#include <stdio.h>

/**
 * Estructura BigDecimal
 * Representa un numero decimal de precision arbitraria.
 * Ejemplo: 123.45 se puede almacenar como el entero 12345 con escala 2.
 */
typedef struct
{
    char *digits;  /* Array de caracteres que contiene solo numeros (la mantisa) */
    int sign;      /* 1 para positivo, -1 para negativo */
    int precision; /* Numero total de digitos significativos */
    int scale;     /* Numero de digitos a la derecha del punto decimal */
} BigDecimal;

/* --- Constructores y Destructores --- */

/* Crea un BigDecimal a partir de una cadena (ej: "123.456") */
BigDecimal *bd_create_from_str(const char *str);

/* Crea un BigDecimal a partir de un entero */
BigDecimal *bd_create_from_int(long long value);

/* Libera la memoria ocupada por el BigDecimal */
void bd_free(BigDecimal *bd);

/* --- Operaciones Aritmeticas --- */

/* Suma: res = a + b */
BigDecimal *bd_add(const BigDecimal *a, const BigDecimal *b);

/* Resta: res = a - b */
BigDecimal *bd_subtract(const BigDecimal *a, const BigDecimal *b);

/* Multiplicacion: res = a * b */
BigDecimal *bd_multiply(const BigDecimal *a, const BigDecimal *b);

/* Division: res = a / b (con un limite de precision especificado) */
BigDecimal *bd_divide(const BigDecimal *a, const BigDecimal *b, int limit_precision);

/* --- Comparaciones --- */

/* Compara a y b. Retorna: 1 si a > b, -1 si a < b, 0 si a == b */
int bd_compare(const BigDecimal *a, const BigDecimal *b);

/* --- Utilidades --- */

/* Retorna una representacion en cadena del numero */
char *bd_to_string(const BigDecimal *bd);

/* Imprime el numero por consola */
void bd_print(const BigDecimal *bd);

#endif /* BIG_DECIMAL_H */
