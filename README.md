# **Overview**

The task is implemented in 2 programming languages, each in its own subfolder. For each language, build and run instructions are below.

The program generates a map file according to a few parameters, and saves it to a file (ex: map.bmp or map.png). It then traverses the map according to the restrictions in the problem descriptions, prints the path to the console, and generates an image representing the path by overlaying the path on the blue channel of the image. The path image is then also saved (ex: path.bmp or path.png)

> *Note: The default map generation settings may not always produce a traversable map. Use the included map_example file if you want to be sure it is traversable. Use the `--help` argument to find out how to pass a pregenerated map*

</br>

# **C**

## To compile:
Run the following command in the `c` subdirectory,
```
gcc main.c utils.c bmp_lib.c queue.c -O2 -o main -lm
```
OR, use the included `run.sh` script on linux
> Tested with gcc version 11.4.0 in Ubuntu WSL, but it *should* work on any non-ancient version of gcc, on any platform

## To run:
```
./main
```
Use `./main --help` for a list of options

The C version does not include a cost map.

> Supports 24-bit BMP files
</br>

# **Rust**

## To compile/run:
Run the following command in the `rust` subdirectory:
```
cargo run
```
> Tested with cargo version 1.9.0 on Windows 11, but it *should* work on any version that implements rustedition2024 (>=1.85.0), on any platform

To pass arguments to the program, use
```
cargo run -- {arg1} {arg2} ...
```
Examples:
```
cargo run -- --map map_example.png
```
For more information on avaliable arguments, run:
```
cargo run -- --help
```

> Supports BMPs and PNGs[^1]
</br>

# **Python**

## Setup:
Included in the `python` subdirectory are 3 setup scripts, one for batch (Windows CMD), one for PowerShell, and one for bash
Run them to set up a venv and install the dependencies

Usage:
```
> setup.bat
> .\setup.ps1
> ./setup.sh
```

## Run:
After running the setup script, the venv will be automatically activated. From there just run `python main.py` with any arguments you may want to pass.
> Tested with python version 3.10.6 on Windows, but it *should* work on any non-ancient version of Python, on any platform that supports the `pillow` library</br>
Examples:
```
python main.py --map map_example.png
python main.py --help
```

> Supports BMPs and PNGs[^2]
> The python implementation is basically a 1-1 translation of the Rust one.

</br>

# **C++**

## To compile:
Run the following command in the `cpp` subdirectory,
```
g++ main.cpp utils.cpp bmp_lib.cpp -O2 -o main
```
OR, use the included `run.sh` script on linux
> Tested with g++ version 11.4.0 in Ubuntu WSL, but it *should* work on any non-ancient version of g++, on any platform

## To run:
```
./main
```
Use `./main --help` for a list of options

> Supports 24-bit BMP files</br>
> This version is basically a 1-1 translation of the C version, but using classes, and implementing a cost map</br>
> It's also the least well tested because i ran out of time</br>



[^1]: Technically can support any image format that the `image` crate supports, but it has to be lossless. Just specify the extension with the `--format` argument (ex: "cargo run -- --format png")
[^2]: Technically can support any image format that the `pillow` library supports, but you should *really* use a lossless format. Just specify the extension with the `--format` argument (ex: "cargo run -- --format png")



