# Algoritmos de Ordenacion en C

Practica comparativa de rendimiento entre cuatro algoritmos de ordenacion implementados en C:

- Quick Sort
- Merge Sort
- Heap Sort
- Bubble Sort

El vector dinamico trabaja con un tipo `big_decimal_t` propio (arbitrario en longitud), por lo que se pueden ordenar valores con cualquier numero de digitos en la parte entera y decimal.

## Estructura del proyecto

- main.c: benchmark comparativo y generacion de tiempos.
- quick_sort.c: implementaciones de Quick, Merge, Heap y Bubble.
- quick_sort.h: declaraciones de algoritmos.
- big_decimal.c y big_decimal.h: TAD BigDecimal de precision arbitraria.
- vector_dinamico.c y vector_dinamico.h: estructura base del vector.
- generar_grafica.py: crea una grafica comparativa unica.
- informe_algoritmos_busqueda.md: informe tecnico ampliado.

## Compilacion

En Windows (MSYS2/MinGW o gcc compatible):

```bash
gcc -std=c11 -O2 -Wall -Wextra -Wpedantic main.c quick_sort.c vector_dinamico.c big_decimal.c -o quick_sort_benchmark.exe
```

## Ejecucion del benchmark

```bash
./quick_sort_benchmark.exe
```

Salida principal:

- tiempos_ordenacion.tsv

Salida legado:

- tiemposFibonacciRecursivo.txt (contiene columna de Quick Sort para compatibilidad)

## Generacion de grafica comparativa

```bash
python generar_grafica.py
```

Archivo generado:

- grafica_comparativa_ordenacion.png

## Complejidad teorica

- Quick Sort: promedio O(n log n), peor O(n^2), memoria promedio O(log n).
- Merge Sort: O(n log n) estable, memoria O(n).
- Heap Sort: O(n log n), memoria O(1) adicional.
- Bubble Sort: O(n^2), memoria O(1).

## Nota sobre los rangos del benchmark

Para poder comparar en una sola grafica y mantener tiempos razonables, el rango se ajusta a:

- n desde 1000 hasta 20000
- paso 1000
- 3 repeticiones por tamaño

Esto evita que Bubble Sort dispare el tiempo total de ejecucion a niveles poco practicos.
