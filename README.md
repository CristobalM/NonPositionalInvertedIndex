# Compact Non-positional inverted index

**(This work is under progress)**

This project implements the compact structure "Non-positional inverted index"
described in the book **Compact Data Structures** by **Gonzalo Navarro**,
specifically at the section 6.4.3.

This structure is implemented on top of the **Simon Gog's** Succinct Wavelet Tree
which can be found on his [Succinct Data Structure Library](https://github.com/simongog/sdsl-lite).

## Current progress

### Done
* Read documents inside a directory
* Make wrapper for Gog's Wavelet Tree
* Build Wavelet Tree
* Implement basic inverted index operations
* Sort terms by revelance order
* Parametrize the relevance metric
* Implement **AND**
* Implement **OR**
* Add disk storage support.
* Extend AND and OR for more than two words.


### To do

* Compare to another inverted index.


## Build

* First make sure SDSL is installed in ~/include and ~/lib as specified by its repository.
* Alternatively, SDSL install location can be changed from CMakeLists.txt

From the root directory of this project run:

```
mkdir build
cd build
cmake ..
make
```

## Run example
```
cd build
./example_1 ../examples/lorem_ipsum dolor Lorem
```
