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
In average a document occupies 41.7 KB.

## Comparison to other indexes

More indexes were also built on the ones provided by the [pisa-engine](https://github.com/pisa-engine/pisa).
Those indexes are Elias-Fano, Block-Interpolative and QMX, described in [https://pisa.readthedocs.io/en/latest/compress_index.html](https://pisa.readthedocs.io/en/latest/compress_index.html).


The following results are each one an average from several runs:

### This index

AND: 559 queries per second

OR: 179 queries per second

Space: 69.6 MB in disk

### Elias-Fano

AND: 1607 queries per second

OR: 1470 queries per second

Space: 32.5 MB in disk

### Block-Interpolative

AND: 1030 queries per second

OR: 857 queries per second

Space: 31.9 MB in disk

### QMX

AND: 2177 queries per second

OR: 1583 queries per second

Space: 41 MB in disk


### Analysis

This means that the index implemented here is much slower than the ones from pisa-engine
and uses more space. This could mean two things, one is that the implementation here
is not good and can be further improved, other is that the representation is not good enough
to be used as an inverted index, which I think is less likely than the first option.

In terms of space, the disappointing (comparative) result is likely because the alphabet of documents and terms is being represented by
32 bits integers. Here we have 19320 documents, and using a 16 bit integer to represent them can reduce the space by half and then it would be competitive
with the other indexes.


### Other scripts

Some scripts were also created to process the text files

* [documents-processing-scripts](https://github.com/CristobalM/documents-processing-scripts):
For document cleaning and to generate query files.

* [pisa_formatter](https://github.com/CristobalM/pisa_formatter): To convert documents into the format accepted by pisa-engine
