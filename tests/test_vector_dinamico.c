#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "vector_dinamico.h"

int main(void)
{
    vector_t v = {0};
    BigDecimal *input = bd_create_from_int(42);
    elemento_t borrowed = NULL;
    assert(input != NULL);
    assert(vector_crear(&v, 0));
    assert(vector_tamano(&v) == 0);
    assert(vector_redimensionar(&v, 2));
    assert(!vector_crear(&v, 4)); /* No pierde recursos anteriores. */
    assert(vector_tamano(&v) == 2);
    assert(vector_asignar(&v, 0, input));
    assert(vector_recuperar(&v, 0, &borrowed));
    assert(borrowed != input);
    assert(bd_compare(borrowed, input) == 0);
    assert(vector_asignar(&v, 1, borrowed)); /* Alias: se copia antes de liberar. */
    assert(v.datos[1] != v.datos[0]);
    assert(vector_asignar(&v, 0, v.datos[0])); /* Autoasignacion segura. */
    assert(bd_compare(v.datos[0], input) == 0);
    assert(vector_reservar(&v, 40));
    assert(vector_capacidad(&v) >= 40);
    assert(vector_redimensionar(&v, 8));
    for (size_t i = 2; i < 8; ++i)
    {
        BigDecimal *zero = bd_create_from_int(0);
        assert(zero != NULL);
        assert(v.datos[i] != NULL && bd_compare(v.datos[i], zero) == 0);
        bd_free(zero);
    }
    vector_intercambiar(&v, SIZE_MAX, 0);
    assert(bd_compare(v.datos[0], input) == 0);
    assert(vector_redimensionar(&v, 1));
    assert(vector_tamano(&v) == 1);
    assert(!vector_reservar(&v, SIZE_MAX));
    assert(vector_tamano(&v) == 1);
    assert(vector_agregar(&v, input));
    assert(v.datos[1] != input);
    assert(vector_redimensionar(&v, 0));
    assert(vector_agregar(&v, input));
    assert(vector_tamano(&v) == 1);
    vector_liberar(&v);
    vector_liberar(&v);
    bd_free(input);
    puts("vector_dinamico BigDecimal: OK");
    return 0;
}
