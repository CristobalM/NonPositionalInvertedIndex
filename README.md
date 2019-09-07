# Compact Non-positional inverted index


This project implements the compact structure "Non-positional inverted index"
described in the book **Compact Data Structures** by **Gonzalo Navarro**,
specifically at the section 6.4.3.

This structure is implemented on top of the **Simon Gog's** Succinct Wavelet Tree
which can be found on his [Succinct Data Structure Library](https://github.com/simongog/sdsl-lite).

## Build

* First make sure SDSL is installed in ~/include and ~/lib as specified by its repository.
* Alternatively, SDSL install location can be changed from CMakeLists.txt

* <experimental/filesystem> is a dependency of this project, make sure you have
C++17 and your compiler up to date. This is known to work under GCC 7.4.0.

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
./multiple_query_index --index_path="INDEX_PATH/INDEX_NAME" --queries_file_path=PATH_TO_QUERIES_FILE
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

## Performance evaluation

On the [blogger](http://u.cs.biu.ac.il/~koppel/BlogCorpus.htm) dataset,
on average, this index performs 559 AND queries per second and 179 OR queries per second. Each query had 3 words, consisting of a phrase inside one of the documents.

That corpus has 19320 documents and 1277708 unique words within those documents.

The compact wavelet tree uses 69.6 MB in disk.
Other additional data required for the index uses 8.2 MB in disk
The listing of words and documents in plain text uses 25 MB in disk, but could be further compressed.

The uncompressed dataset occupies 806,2 MB in disk.