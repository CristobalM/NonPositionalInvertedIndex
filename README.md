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

* <experimental/filesystem> is a dependency of this project, make sure you have
C++17 and your compiler up to date. This is known to work under g++ 7.4.0.

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

## Build inverted index

```
./build_inverted_index --index_name=INDEX_NAME --read_from_dir=PATH_TO_DOCUMENTS_DIR --save_location=PATH_TO_SAVE_LOCATION
```


## Run single query

```
./single_query_index --index_path="INDEX_PATH/INDEX_NAME" --query_type="AND|OR" --query_input=LIST_OF_WORDS_SEPARATED_BY_SPACE
```

* For index_path, concatenate PATH_TO_SAVE_LOCATION/INDEX_NAME from build_inverted_index
* --query_type must be one of "AND" or "OR"
* --query_input must be quoted if there are more than one word, for example: --query_input="one two three"


## Run multiple queries

```
--index_path="INDEX_PATH/INDEX_NAME" --queries_file_path=PATH_TO_QUERIES_FILE
```

The queries file has the following syntax:

Each line corresponds to a query, where the first word specifies which
query to run.

Possible queries:

* AND term1 term2 term3 ...
* OR term1 term2 term3 ...
* DOCUMENT_FREQUENCY term
* ITH_DOCUMENT term index_i

Where each query means:

* AND finds the documents where all of term1 term2 term3 ... appear in
* OR finds the documents where any of term1 term2 term3 ... appear in
* DOCUMENT_FREQUENCY stands for the number of documents where term appears
* ITH_DOCUMENT gives the ith document of the word specified by term, ordered by the word frequency in each document.