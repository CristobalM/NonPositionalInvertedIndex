#include <iostream>


#include <stdexcept>

#include "DocumentsHandler.h"
#include "InvertedIndex.hpp"
#include "WTHGog.hpp"
#include "BVHGog.hpp"

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

  documentsHandler.debugPrintScannedWords();
  InvertedIndex<DocumentsHandler, WTHGog<>, BVHGog<>> invertedIndex(documentsHandler);

  std::string wordToQuery("UNO");
  auto docFreqHola = invertedIndex.documentFrequency(documentsHandler, wordToQuery);
  std::cout << wordToQuery << " DOCUMENT FREQUENCY = " << docFreqHola << std::endl;

  int idx_to_query = 3;

  auto secondSI = invertedIndex.ithDoc(documentsHandler, wordToQuery, idx_to_query);
  std::cout << idx_to_query << "th " << wordToQuery << ": " << secondSI << std::endl;

  auto &wtHandler = invertedIndex.getWTHandler();

  std::cout << "RANK (0, t=0) = " << wtHandler.rank(0, 0) << std::endl;
  std::cout << "RANK (13, t=0) = " << wtHandler.rank(13, 0) << std::endl;

  auto &internalWT = wtHandler.getInternalStructure();
  std::cout << "wt.sigma: " <<internalWT.sigma << std::endl;


  return 0;
}