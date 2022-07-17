
# My CPP Utils

This repository contains a variety of useful C++ utilities. Currently, it contains.


## Contents

- Concepts:
  - Printable, if `<<` is a defined operator.
  - Readable, if `>>` is a defined operator.
- Vector Operations:
  - Printing a vector (in different formats, see vector_print_* globals).
  - Reading a vector with a known number of elements.
- Fast IO: Sets up fast input and output for competitive programming.
- Range: An implementation of python's range() function, as an int returing iterator.


## Building and Testing

A part of the reason why I am sharing this project is because I am proud of the
self created `Makefile`. It's use is as follow:

`make` or `make build` in order to produce the `my_utils.a` library file. This compiles
and links all files in the `src/` directory

`make test` will compile and run all the tests in the `tests/` directory. If a
test returns something other than 0, the test command will fail.

`make clean` removes the `build/` directory and all its contents. All artefacts produced
by the Makefile are somewhere in the `build/` directory.


## Usage

Either include specific headers as you see fit, or use the catchall `my_utils.h` header.

Use the `-I <path>` flag with your compiler, setting the path to this directory's include
folder.

Also, add `build/my_utils.a` as a source when compiling. `build/my_utils.a` must
be built, of course.

Feel free to copy and move the include folder and `my_utils.a` to somewhere convenient for your projects.
