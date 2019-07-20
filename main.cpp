#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>

#include <stdexcept>

#include "DocumentsHandler.h"
#include "InvertedIndex.h"
#include "WTHGogHandler.h"

int main(int argc, char **argv) {

  /*
  sdsl::int_vector<> iv(10);
  iv[2] = 1;
  sdsl::wt_int<> wt();
  */

  if(argc <= 1){
    throw std::runtime_error("Documents directory missing (First argument)");
  }

  std::cout << "Documents directory: " << argv[1] << std::endl;
  DocumentsHandler documentsHandler(true);

  documentsHandler.initDocumentsFromDirectory(argv[1]);
  documentsHandler.scanWords();
  //documentsHandler.debugPrintScannedWords();

  InvertedIndex<DocumentsHandler, WTHGogHandler> invertedIndex(documentsHandler);


  return 0;
}