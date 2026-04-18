
#ifndef VECTOR_DINAMICO_H
#define VECTOR_DINAMICO_H

#include <stddef.h>
#include "big_decimal.h"

typedef BigDecimal *elemento_t;

typedef struct
{
    elemento_t *datos;
    size_t tam;
} vector_t;

int vector_crear(vector_t *vector, size_t tam);
void vector_liberar(vector_t *vector);
size_t vector_tamano(const vector_t *vector);
int vector_asignar(vector_t *vector, size_t posicion, elemento_t valor);
int vector_recuperar(const vector_t *vector, size_t posicion, elemento_t *valor);
void vector_intercambiar(vector_t *vector, size_t izquierda, size_t derecha);
int vector_esta_ordenado(const vector_t *vector);

#endif