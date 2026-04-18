#include "quick_sort.h"

#include <stdlib.h>

static size_t particionar(vector_t *vector, size_t izquierda, size_t derecha)
{
    elemento_t pivote = vector->datos[derecha];
    size_t i = izquierda;
    size_t j;

    for (j = izquierda; j < derecha; j++)
    {
        if (bd_compare(vector->datos[j], pivote) <= 0)
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

static void merge(vector_t *vector, size_t izquierda, size_t medio, size_t derecha, elemento_t *auxiliar)
{
    size_t i = izquierda;
    size_t j = medio + 1;
    size_t k = izquierda;

    while (i <= medio && j <= derecha)
    {
        if (bd_compare(vector->datos[i], vector->datos[j]) <= 0)
        {
            auxiliar[k++] = vector->datos[i++];
        }
        else
        {
            auxiliar[k++] = vector->datos[j++];
        }
    }

    while (i <= medio)
    {
        auxiliar[k++] = vector->datos[i++];
    }

    while (j <= derecha)
    {
        auxiliar[k++] = vector->datos[j++];
    }

    for (k = izquierda; k <= derecha; k++)
    {
        vector->datos[k] = auxiliar[k];
    }
}

static void merge_sort_rec(vector_t *vector, size_t izquierda, size_t derecha, elemento_t *auxiliar)
{
    size_t medio;

    if (izquierda >= derecha)
    {
        return;
    }

    medio = izquierda + (derecha - izquierda) / 2;
    merge_sort_rec(vector, izquierda, medio, auxiliar);
    merge_sort_rec(vector, medio + 1, derecha, auxiliar);
    merge(vector, izquierda, medio, derecha, auxiliar);
}

void merge_sort(vector_t *vector)
{
    elemento_t *auxiliar;

    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
    {
        return;
    }

    auxiliar = (elemento_t *)malloc(vector->tam * sizeof(elemento_t));
    if (auxiliar == NULL)
    {
        return;
    }

    merge_sort_rec(vector, 0, vector->tam - 1, auxiliar);
    free(auxiliar);
}

static void heapify(vector_t *vector, size_t tam, size_t raiz)
{
    size_t mayor = raiz;
    size_t izquierda = (2 * raiz) + 1;
    size_t derecha = (2 * raiz) + 2;

    if (izquierda < tam && bd_compare(vector->datos[izquierda], vector->datos[mayor]) > 0)
    {
        mayor = izquierda;
    }

    if (derecha < tam && bd_compare(vector->datos[derecha], vector->datos[mayor]) > 0)
    {
        mayor = derecha;
    }

    if (mayor != raiz)
    {
        vector_intercambiar(vector, raiz, mayor);
        heapify(vector, tam, mayor);
    }
}

void heap_sort(vector_t *vector)
{
    size_t i;

    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
    {
        return;
    }

    for (i = (vector->tam / 2); i > 0; i--)
    {
        heapify(vector, vector->tam, i - 1);
    }

    for (i = vector->tam; i > 1; i--)
    {
        vector_intercambiar(vector, 0, i - 1);
        heapify(vector, i - 1, 0);
    }
}

void bubble_sort(vector_t *vector)
{
    size_t i;
    size_t j;
    int intercambio;

    if (vector == NULL || vector->datos == NULL || vector->tam < 2)
    {
        return;
    }

    for (i = 0; i < vector->tam - 1; i++)
    {
        intercambio = 0;
        for (j = 0; j < vector->tam - i - 1; j++)
        {
            if (bd_compare(vector->datos[j], vector->datos[j + 1]) > 0)
            {
                vector_intercambiar(vector, j, j + 1);
                intercambio = 1;
            }
        }

        if (!intercambio)
        {
            break;
        }
    }
}
