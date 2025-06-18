# Algoritmos meméticos para el MC-TTRP

En este repositorio se encuentran todos los archivos referentes a la implementación, pruebas y análisis de resultados de mi TFG: "Algoritmos meméticos para el MC-TTRP". En este TFG se busca la implementación de un algoritmo memético para la resolución del problema de rutas de camiones y tráileres multicompartimento o MC-TTRP. Se mostrará cómo utilizar el repositorio.

## Como se utiliza

Como estamos utilizando C++, será necesario encontrarse en un entorno que permita la compilación de archivos de C mediante el compilador g++. Para ello, se recomienda estar en un entorno Linux. En caso de encontrarse en Windows, será necesario la utilización de herramientas como WSL o una máquina virtual.

- Para realizar la **compilación** se puede usar directamente el archivo `Makefile` con el siguiente comando:
```
make
```
- Utilizando este comando, se generará un ejecutable denominado `MC_memetic.out`, que se podrá **ejecutar** mediante el siguiente comando:
```
./MC_memetic [ruta_archivo]
```
En el caso de que se desee compilar de manera manual, se puede hacer con el siguiente comando:
```
g++ -Iheaders -fopenmp -Wall -o [nombre_ejecutable] main.cpp utils.cpp memetic_core.cpp
```
Dando lugar a un fichero ejecutable con el nombre deseado.

## Descripción de los ficheros

Se dividirá en los tipos de archivo, comenzando por los archivos *source* de C++. Las cabeceras de los ficheros se encuentran en el directorio `headers`. Los archivos principales de C++ son:
- `utils.cpp`: Este archivo contiene las implementaciones de las clases necesarias para el funcionamiento del código, además de alguna otra función utilizada, como la que genera la matriz de distancias.
- `memetic_core.cpp`: Este archivo contiene la implementación de todos los métodos utilizados para la implementación del algoritmo genético.
- `main.cpp`: Este archivo contiene la ejecución del programa, utilizando las funciones anteriormente implementadas.

Existe un archivo de Python:
- `result_analysis.ipynb`: En esta libreta de Jupyter se muestran los resultados y análisis de las soluciones, utilizando Markdown, estos resultados están explicados y complementan el documento pdf del TFG.

Finalmente, tenemos múltiples carpetas variadas:
- `instances`: Esta carpeta contiene todas las instancias de los problemas utilizadas para las pruebas del algoritmo. Estas instancias han sido extraidas de https://github.com/LauraDavilaPena/MC-TTRP_heuristics.
- `metrics`: Ficheros de tipo csv que contienen los resultados obtenidos.
- `misc`: Carpeta variada, que contiene algunos archivos utilizados en a lo largo del desarrollo del TFG. Como ejemplo, aquí se incluyen los *scripts* utilizados para la ejecución del programa en el CESGA.
- `LaTeX_Files`: Ficheros de LaTeX que componen el TFG. 