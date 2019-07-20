# Compact Non-positional inverted index

**(This work is under progress)**

This project implements the compact structure "Non-positional inverted index"
described in the Book **Compact Data Structures** by **Gonzalo Navarro**,
specifically at the section 6.4.3:

This structure is implemented on top of the **Simon Gog's** Succinct Wavelet Tree
which can be found on his [Succinct Data Structure Library](https://github.com/simongog/sdsl-lite).

## Current progress

### Done
* Read documents inside a directory

### To do
* Make wrapper for Gog's Wavelet Tree
* Build Wavelet Tree
* Implement basic inverted index operations
* Implement **AND**
* Implement **OR**
* Sort terms by revelance order
* Parametrize the relevance metric
* Group terms by significance