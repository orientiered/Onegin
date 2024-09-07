# Onegin

Sorts lines in given file in ascending order

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
    + By default insertion sort (`insertion`), you can also use bubble sort(`bubble`)

Example:
```
./main --sort bubble -io onegin.txt out.txt
```
