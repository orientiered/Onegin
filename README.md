# Onegin

Sorts lines in given file in ascending order

Then sorts lines starting from their end

Then prints original text

Ignores all symbols except alphabet, also doesn't distinguish uppercase and lowercase

## Build

Debug version build:
```
make clean
make
```
To build release version (x3 faster than debug) use `BUILD=RELEASE`

To build for Windows, specify `SYSTEM=WIN`
```
make BUILD=RELEASE SYSTEM=WIN CC=clang
```

You can choose compiler with `CC=` (default is `g++`)
### Dependencies
1. `make`
2. `g++` or another C/C++ compiler
3. `python3`, `matplotlib` and `tkinter-m`, if you wan't to use `-g` flag

## Usage

```
./main -flags
```
Available flags:

1. `-i (--input)` - next argument must be name of input file
    + By default `OneginText.txt`
2. `-o (--output)` - next argument must be name of output file
    + By default `stdout`
3. `-s (--sort)` - next argument must be name of sort algorithm
    + By default my realization  of quick sort (`qsort`)
    + You can also use bubble (`bubble`), insertion (`insertion`), shell (`shell`) and stdlib qsort (`cqsort`) sorts
4. `-t (--time)` - runs sorting algorithm 50 times (hardcoded) and prints average time
5. `-g (--graph)` - `-t` flag generates file with all sorting times, this flag will build graph of it using `matplotlib`
Example:
```
./main --sort bubble -tio onegin.txt out.txt
```
