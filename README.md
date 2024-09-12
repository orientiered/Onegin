# Onegin

Sorts lines in given file in ascending order

Then sorts lines starting from their end

Then prints original text

Ignores all symbols except alphabet, also doesn't distinguish uppercase and lowercase

## Usage

```
./main -flags
```
Available flags:

1. `-i (--input)` - next argument must be name of input file
    + By default `Romeo and Juliet.txt`
2. `-o (--output)` - next argument must be name of output file
    + By default `stdout`
3. `-s (--sort)` - next argument must be name of sort algorithm
    + By default quick sort (`qsort`)
    + You can also use bubble (`bubble`), insertion (`insertion`) and shell (`shell`) sorts
4. `-t (--time)` - runs sorting algorithm 15 times (hardcoded) and prints average time
Example:
```
./main --sort bubble -tio onegin.txt out.txt
```
