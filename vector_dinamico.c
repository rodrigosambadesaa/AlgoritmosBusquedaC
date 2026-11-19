#include "vector_dinamico.h"
#include <stdint.h>
#include <stdlib.h>

static BigDecimal *bd_clone_local(const BigDecimal *src)
{
    BigDecimal *out;
    char *texto;
    if (src == NULL)
        return NULL;
    texto = bd_to_string(src);
    if (texto == NULL)
        return NULL;
    out = bd_create_from_str(texto);
    free(texto);
    return out;
}

int vector_reservar(vector_t *vector, size_t capacidad)
{
    elemento_t *nuevo;
    if (vector == NULL)
        return 0;
    if (capacidad <= vector->capacidad)
        return 1;
    if (capacidad > SIZE_MAX / sizeof(*vector->datos))
        return 0;
    nuevo = (elemento_t *)realloc(vector->datos, capacidad * sizeof(*vector->datos));
    if (nuevo == NULL)
        return 0;
    vector->datos = nuevo;
    vector->capacidad = capacidad;
    return 1;
}

int vector_redimensionar(vector_t *vector, size_t tam)
{
    size_t i, capacidad;
    if (vector == NULL)
        return 0;
    if (tam < vector->tam)
    {
        for (i = tam; i < vector->tam; ++i)
        {
            bd_free(vector->datos[i]);
            vector->datos[i] = NULL;
        }
    }
    else if (tam > vector->tam)
    {
        if (tam > vector->capacidad)
        {
            capacidad = vector->capacidad ? vector->capacidad : 1;
            while (capacidad < tam)
            {
                if (capacidad > SIZE_MAX / 2)
                {
                    capacidad = tam;
                    break;
                }
                capacidad *= 2;
            }
            if (!vector_reservar(vector, capacidad))
                return 0;
        }
        for (i = vector->tam; i < tam; ++i)
        {
            vector->datos[i] = bd_create_from_int(0);
            if (vector->datos[i] == NULL)
            {
                size_t j;
                for (j = vector->tam; j < i; ++j)
                {
                    bd_free(vector->datos[j]);
                    vector->datos[j] = NULL;
                }
                return 0;
            }
        }
    }
    vector->tam = tam;
    return 1;
}

int vector_crear(vector_t *vector, size_t tam)
{
    if (vector == NULL)
        return 0;
    vector->datos = NULL;
    vector->tam = 0;
    vector->capacidad = 0;
    return vector_redimensionar(vector, tam);
}

void vector_liberar(vector_t *vector)
{
    size_t i;
    if (vector == NULL)
        return;
    for (i = 0; i < vector->tam; ++i)
        bd_free(vector->datos[i]);
    free(vector->datos);
    vector->datos = NULL;
    vector->tam = 0;
    vector->capacidad = 0;
}

size_t vector_tamano(const vector_t *vector)
{
    return vector == NULL ? 0 : vector->tam;
}

size_t vector_capacidad(const vector_t *vector)
{
    return vector == NULL ? 0 : vector->capacidad;
}

int vector_asignar(vector_t *vector, size_t posicion, elemento_t valor)
{
    BigDecimal *copia;
    if (vector == NULL || vector->datos == NULL || posicion >= vector->tam || valor == NULL)
        return 0;
    copia = bd_clone_local(valor);
    if (copia == NULL)
        return 0;
    bd_free(vector->datos[posicion]);
    vector->datos[posicion] = copia;
    return 1;
}

int vector_agregar(vector_t *vector, elemento_t valor)
{
    size_t previo;
    if (vector == NULL || valor == NULL || vector->tam == SIZE_MAX)
        return 0;
    previo = vector->tam;
    if (!vector_redimensionar(vector, previo + 1))
        return 0;
    if (!vector_asignar(vector, previo, valor))
    {
        (void)vector_redimensionar(vector, previo);
        return 0;
    }
    return 1;
}

int vector_recuperar(const vector_t *vector, size_t posicion, elemento_t *valor)
{
    if (vector == NULL || vector->datos == NULL || valor == NULL || posicion >= vector->tam)
        return 0;
    *valor = vector->datos[posicion];
    return 1;
}

void vector_intercambiar(vector_t *vector, size_t izquierda, size_t derecha)
{
    elemento_t temporal;
    if (vector == NULL || vector->datos == NULL ||
        izquierda >= vector->tam || derecha >= vector->tam)
        return;
    temporal = vector->datos[izquierda];
    vector->datos[izquierda] = vector->datos[derecha];
    vector->datos[derecha] = temporal;
}

int vector_esta_ordenado(const vector_t *vector)
{
    size_t i;
    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
        return 1;
    for (i = 1; i < vector->tam; ++i)
        if (bd_compare(vector->datos[i - 1], vector->datos[i]) > 0)
            return 0;
    return 1;
}
