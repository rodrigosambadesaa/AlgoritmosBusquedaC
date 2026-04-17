#include "vector_dinamico.h"

#include <stdlib.h>

int vector_crear(vector_t *vector, size_t tam)
{
    size_t i;

    if (vector == NULL)
    {
        return 0;
    }

    vector->datos = (elemento_t *)malloc(tam * sizeof(elemento_t));
    if (vector->datos == NULL)
    {
        vector->tam = 0;
        return 0;
    }

    vector->tam = tam;
    for (i = 0; i < tam; i++)
    {
        vector->datos[i] = 0;
    }

    return 1;
}

void vector_liberar(vector_t *vector)
{
    if (vector == NULL)
    {
        return;
    }

    free(vector->datos);
    vector->datos = NULL;
    vector->tam = 0;
}

size_t vector_tamano(const vector_t *vector)
{
    if (vector == NULL)
    {
        return 0;
    }

    return vector->tam;
}

int vector_asignar(vector_t *vector, size_t posicion, elemento_t valor)
{
    if (vector == NULL || vector->datos == NULL || posicion >= vector->tam)
    {
        return 0;
    }

    vector->datos[posicion] = valor;
    return 1;
}

int vector_recuperar(const vector_t *vector, size_t posicion, elemento_t *valor)
{
    if (vector == NULL || vector->datos == NULL || valor == NULL || posicion >= vector->tam)
    {
        return 0;
    }

    *valor = vector->datos[posicion];
    return 1;
}

void vector_intercambiar(vector_t *vector, size_t izquierda, size_t derecha)
{
    elemento_t temporal;

    if (vector == NULL || vector->datos == NULL || izquierda >= vector->tam || derecha >= vector->tam)
    {
        return;
    }

    temporal = vector->datos[izquierda];
    vector->datos[izquierda] = vector->datos[derecha];
    vector->datos[derecha] = temporal;
}

int vector_esta_ordenado(const vector_t *vector)
{
    size_t i;

    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
    {
        return 1;
    }

    for (i = 1; i < vector->tam; i++)
    {
        if (vector->datos[i - 1] > vector->datos[i])
        {
            return 0;
        }
    }

    return 1;
}