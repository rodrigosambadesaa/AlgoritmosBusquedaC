#ifndef VECTOR_DINAMICO_H
#define VECTOR_DINAMICO_H

#include <stddef.h>
#include "big_decimal.h"

typedef BigDecimal *elemento_t;

typedef struct
{
    elemento_t *datos;
    size_t tam;
    size_t capacidad;
} vector_t;

/* Crear sobre un objeto nuevo; liberar antes de volver a crearlo.
 * Los elementos tienen propiedad exclusiva del vector, son BigDecimal
 * inicializados a cero y vector_asignar siempre clona el argumento.
 * vector_recuperar entrega una referencia prestada.
 */
int vector_crear(vector_t *vector, size_t tam);
void vector_liberar(vector_t *vector);
size_t vector_tamano(const vector_t *vector);
int vector_asignar(vector_t *vector, size_t posicion, elemento_t valor);
int vector_recuperar(const vector_t *vector, size_t posicion, elemento_t *valor);
void vector_intercambiar(vector_t *vector, size_t izquierda, size_t derecha);
int vector_esta_ordenado(const vector_t *vector);

/* Cambios seguros de capacidad y tamano; devuelven 0 si no hay memoria
 * o la capacidad es imposible, conservando el contenido anterior.
 * Al crecer se crean valores BigDecimal cero; al reducir se liberan.
 */
int vector_reservar(vector_t *vector, size_t capacidad);
int vector_redimensionar(vector_t *vector, size_t tam);
size_t vector_capacidad(const vector_t *vector);
/* Copia el valor, el llamador conserva su propiedad. */
int vector_agregar(vector_t *vector, elemento_t valor);

#endif
