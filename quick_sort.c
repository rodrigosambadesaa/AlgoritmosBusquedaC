#include "quick_sort.h"

static size_t particionar(vector_t *vector, size_t izquierda, size_t derecha)
{
    elemento_t pivote = vector->datos[derecha];
    size_t i = izquierda;
    size_t j;

    for (j = izquierda; j < derecha; j++)
    {
        if (vector->datos[j] <= pivote)
        {
            vector_intercambiar(vector, i, j);
            i++;
        }
    }

    vector_intercambiar(vector, i, derecha);
    return i;
}

static void quick_sort_rec(vector_t *vector, size_t izquierda, size_t derecha)
{
    size_t pivote;

    if (izquierda >= derecha)
    {
        return;
    }

    pivote = particionar(vector, izquierda, derecha);

    if (pivote > 0)
    {
        quick_sort_rec(vector, izquierda, pivote - 1);
    }
    quick_sort_rec(vector, pivote + 1, derecha);
}

void quick_sort(vector_t *vector)
{
    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
    {
        return;
    }

    quick_sort_rec(vector, 0, vector->tam - 1);
}
