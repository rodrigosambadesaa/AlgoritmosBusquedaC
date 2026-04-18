#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "quick_sort.h"
#include "vector_dinamico.h"

#define TAMANO_INICIAL 1000UL
#define TAMANO_FINAL 20000UL
#define PASO 1000UL
#define REPETICIONES 3UL
#define ARCHIVO_SALIDA "tiempos_ordenacion.tsv"
#define ARCHIVO_SALIDA_LEGADO "tiemposFibonacciRecursivo.txt"

typedef void (*algoritmo_ordenacion_t)(vector_t *);

static BigDecimal *crear_big_decimal_aleatorio(void)
{
    size_t digitos_enteros = 1U + (size_t)(rand() % 35);
    size_t digitos_decimales = (size_t)(rand() % 35);
    size_t capacidad = digitos_enteros + digitos_decimales + 4U;
    char *buffer = (char *)malloc(capacidad);
    size_t i = 0;
    BigDecimal *valor;

    if (buffer == NULL)
    {
        return NULL;
    }

    if (rand() % 2 == 0)
    {
        buffer[i++] = '-';
    }

    buffer[i++] = (char)('1' + (rand() % 9));
    for (; i < digitos_enteros + ((buffer[0] == '-') ? 1U : 0U); i++)
    {
        buffer[i] = (char)('0' + (rand() % 10));
    }

    if (digitos_decimales > 0)
    {
        size_t j;
        buffer[i++] = '.';
        for (j = 0; j < digitos_decimales; j++)
        {
            buffer[i++] = (char)('0' + (rand() % 10));
        }
    }

    buffer[i] = '\0';
    valor = bd_create_from_str(buffer);
    free(buffer);
    return valor;
}

static void inicializar_vector_aleatorio(vector_t *vector)
{
    size_t i;
    for (i = 0; i < vector_tamano(vector); i++)
    {
        elemento_t valor = crear_big_decimal_aleatorio();
        if (valor == NULL)
        {
            fprintf(stderr, "Error creando big decimal aleatorio en posicion %lu.\n", (unsigned long)i);
            continue;
        }
        if (!vector_asignar(vector, i, valor))
        {
            fprintf(stderr, "Error asignando big decimal aleatorio en posicion %lu.\n", (unsigned long)i);
        }
        bd_free(valor);
    }
}

static int copiar_vector(vector_t *destino, const vector_t *origen)
{
    size_t i;

    if (destino == NULL || origen == NULL || destino->datos == NULL || origen->datos == NULL)
    {
        return 0;
    }

    if (destino->tam != origen->tam)
    {
        return 0;
    }

    for (i = 0; i < origen->tam; i++)
    {
        if (!vector_asignar(destino, i, origen->datos[i]))
        {
            return 0;
        }
    }

    return 1;
}

static double medir_algoritmo_segundos(vector_t *vector, algoritmo_ordenacion_t algoritmo)
{
    clock_t inicio;
    clock_t fin;

    inicio = clock();
    algoritmo(vector);
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

    fprintf(archivo_salida, "n\tquick_sort\tmerge_sort\theap_sort\tbubble_sort\n");
    fprintf(archivo_salida_legado, "n\ttiempo_segundos\n");

    for (n = TAMANO_INICIAL; n <= TAMANO_FINAL; n += PASO)
    {
        double acumulado_quick = 0.0;
        double acumulado_merge = 0.0;
        double acumulado_heap = 0.0;
        double acumulado_bubble = 0.0;

        for (repeticion = 0; repeticion < REPETICIONES; repeticion++)
        {
            vector_t base;
            vector_t v_quick;
            vector_t v_merge;
            vector_t v_heap;
            vector_t v_bubble;

            if (!vector_crear(&base, n) || !vector_crear(&v_quick, n) || !vector_crear(&v_merge, n) || !vector_crear(&v_heap, n) || !vector_crear(&v_bubble, n))
            {
                vector_liberar(&base);
                vector_liberar(&v_quick);
                vector_liberar(&v_merge);
                vector_liberar(&v_heap);
                vector_liberar(&v_bubble);
                fclose(archivo_salida_legado);
                fclose(archivo_salida);
                fprintf(stderr, "No se pudo reservar memoria para n=%lu.\n", (unsigned long)n);
                return EXIT_FAILURE;
            }

            inicializar_vector_aleatorio(&base);
            copiar_vector(&v_quick, &base);
            copiar_vector(&v_merge, &base);
            copiar_vector(&v_heap, &base);
            copiar_vector(&v_bubble, &base);

            acumulado_quick += medir_algoritmo_segundos(&v_quick, quick_sort);
            acumulado_merge += medir_algoritmo_segundos(&v_merge, merge_sort);
            acumulado_heap += medir_algoritmo_segundos(&v_heap, heap_sort);
            acumulado_bubble += medir_algoritmo_segundos(&v_bubble, bubble_sort);

            if (!vector_esta_ordenado(&v_quick) || !vector_esta_ordenado(&v_merge) || !vector_esta_ordenado(&v_heap) || !vector_esta_ordenado(&v_bubble))
            {
                vector_liberar(&base);
                vector_liberar(&v_quick);
                vector_liberar(&v_merge);
                vector_liberar(&v_heap);
                vector_liberar(&v_bubble);
                fclose(archivo_salida_legado);
                fclose(archivo_salida);
                fprintf(stderr, "Error: algun algoritmo no ordeno correctamente para n=%lu.\n", (unsigned long)n);
                return EXIT_FAILURE;
            }

            vector_liberar(&base);
            vector_liberar(&v_quick);
            vector_liberar(&v_merge);
            vector_liberar(&v_heap);
            vector_liberar(&v_bubble);
        }

        fprintf(archivo_salida,
                "%lu\t%.6f\t%.6f\t%.6f\t%.6f\n",
                (unsigned long)n,
                acumulado_quick / (double)REPETICIONES,
                acumulado_merge / (double)REPETICIONES,
                acumulado_heap / (double)REPETICIONES,
                acumulado_bubble / (double)REPETICIONES);

        fprintf(archivo_salida_legado, "%lu\t%.6f\n", (unsigned long)n, acumulado_quick / (double)REPETICIONES);

        printf("%lu\tQ:%.6f\tM:%.6f\tH:%.6f\tB:%.6f\n",
               (unsigned long)n,
               acumulado_quick / (double)REPETICIONES,
               acumulado_merge / (double)REPETICIONES,
               acumulado_heap / (double)REPETICIONES,
               acumulado_bubble / (double)REPETICIONES);
    }

    fclose(archivo_salida_legado);
    fclose(archivo_salida);
    return EXIT_SUCCESS;
}
