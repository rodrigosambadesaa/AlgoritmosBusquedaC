#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "quick_sort.h"
#include "vector_dinamico.h"

#define TAMANO_INICIAL 10000UL
#define TAMANO_FINAL 1000000UL
#define PASO 10000UL
#define REPETICIONES 3UL
#define ARCHIVO_SALIDA "tiempos_quick_sort.tsv"
#define ARCHIVO_SALIDA_LEGADO "tiemposFibonacciRecursivo.txt"

static void inicializar_vector_aleatorio(vector_t *vector)
{
    size_t i;
    for (i = 0; i < vector_tamano(vector); i++)
    {
        vector_asignar(vector, i, rand());
    }
}

static double medir_quick_sort_segundos(vector_t *vector)
{
    clock_t inicio;
    clock_t fin;

    inicio = clock();
    quick_sort(vector);
    fin = clock();

    return (double)(fin - inicio) / (double)CLOCKS_PER_SEC;
}

int main(void)
{
    size_t n;
    size_t repeticion;
    FILE *archivo_salida;
    FILE *archivo_salida_legado;

    srand((unsigned int)time(NULL));

    archivo_salida = fopen(ARCHIVO_SALIDA, "w");
    if (archivo_salida == NULL)
    {
        fprintf(stderr, "No se pudo abrir %s para escritura.\n", ARCHIVO_SALIDA);
        return EXIT_FAILURE;
    }

    archivo_salida_legado = fopen(ARCHIVO_SALIDA_LEGADO, "w");
    if (archivo_salida_legado == NULL)
    {
        fclose(archivo_salida);
        fprintf(stderr, "No se pudo abrir %s para escritura.\n", ARCHIVO_SALIDA_LEGADO);
        return EXIT_FAILURE;
    }

    fprintf(archivo_salida, "n\ttiempo_segundos\n");
    fprintf(archivo_salida_legado, "n\ttiempo_segundos\n");

    for (n = TAMANO_INICIAL; n <= TAMANO_FINAL; n += PASO)
    {
        double acumulado = 0.0;

        for (repeticion = 0; repeticion < REPETICIONES; repeticion++)
        {
            vector_t vector;
            if (!vector_crear(&vector, n))
            {
                fclose(archivo_salida_legado);
                fclose(archivo_salida);
                fprintf(stderr, "No se pudo reservar memoria para n=%lu.\n", (unsigned long)n);
                return EXIT_FAILURE;
            }

            inicializar_vector_aleatorio(&vector);
            acumulado += medir_quick_sort_segundos(&vector);

            if (!vector_esta_ordenado(&vector))
            {
                vector_liberar(&vector);
                fclose(archivo_salida_legado);
                fclose(archivo_salida);
                fprintf(stderr, "Error: quick sort no ordeno correctamente para n=%lu.\n", (unsigned long)n);
                return EXIT_FAILURE;
            }

            vector_liberar(&vector);
        }

        fprintf(archivo_salida, "%lu\t%.6f\n", (unsigned long)n, acumulado / (double)REPETICIONES);
        fprintf(archivo_salida_legado, "%lu\t%.6f\n", (unsigned long)n, acumulado / (double)REPETICIONES);
        printf("%lu\t%.6f\n", (unsigned long)n, acumulado / (double)REPETICIONES);
    }

    fclose(archivo_salida_legado);
    fclose(archivo_salida);
    return EXIT_SUCCESS;
}
