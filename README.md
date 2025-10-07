# C

### To compile:
Run the following command in the c subdirectory,
```
gcc main.c utils.c bmp_lib.c queue.c -O2 -o main -lm
```
OR, use the included `run.sh` script on linux

### To run:
```
./main
```
Use `./main --help` for a list of options

The C version does not include a cost map.

> _Note: The default map generation settings may not always produce a traversable map. Use the included map_example file if you want to be sure it is traversable_