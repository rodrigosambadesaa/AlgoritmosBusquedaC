import csv
from pathlib import Path

import matplotlib.pyplot as plt


BASE_DIR = Path(__file__).resolve().parent
ARCHIVO_DATOS = BASE_DIR / "tiempos_quick_sort.tsv"
ARCHIVO_GRAFICA = BASE_DIR / "quick_sort_grafica.png"


def cargar_datos(ruta):
    tamanos = []
    tiempos = []

    with ruta.open("r", encoding="utf-8") as archivo:
        lector = csv.DictReader(archivo, delimiter="\t")
        for fila in lector:
            tamanos.append(int(fila["n"]))
            tiempos.append(float(fila["tiempo_segundos"]))

    return tamanos, tiempos


def main():
    tamanos, tiempos = cargar_datos(ARCHIVO_DATOS)

    plt.figure(figsize=(11, 6))
    plt.plot(tamanos, tiempos, color="#0b7285", linewidth=2.0, marker="o", markersize=3)
    plt.title("Benchmark de Quick Sort (promedio de 3 ejecuciones)")
    plt.xlabel("n (numero de elementos)")
    plt.ylabel("tiempo (s)")
    plt.grid(True, linestyle="--", alpha=0.35)
    plt.tight_layout()
    plt.savefig(ARCHIVO_GRAFICA, dpi=140)


if __name__ == "__main__":
    main()
