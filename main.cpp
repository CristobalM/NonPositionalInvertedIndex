#include <iostream>


#include <stdexcept>

#include "DocumentsHandler.h"
#include "InvertedIndex.h"
#include "WTHGog.h"
#include "BVHGog.h"

int main(int argc, char **argv) {



  /*
  sdsl::int_vector<> iv(10);
  iv[2] = 1;
  sdsl::wt_int<> wt();
  */

  /*
  if(argc <= 1){
    throw std::runtime_error("Documents directory missing (First argument)");
  }

  std::cout << "Documents directory: " << argv[1] << std::endl;
  DocumentsHandler documentsHandler(true);

  documentsHandler.initDocumentsFromDirectory(argv[1]);
  documentsHandler.scanWords();
  //documentsHandler.debugPrintScannedWords();

  InvertedIndex<DocumentsHandler, WTHGog> invertedIndex(documentsHandler);
   */

  BVHGog bvhGog(10);
  bvhGog.bitset(5);
  bvhGog.bitset(3);
  bvhGog.bitset(2);
  bvhGog.bitset(10);
  //bvhGog.bitset(1);
  // 0 1 1 0 1 0 0 0 0 1
  bvhGog.buildStructures();
  std::cout << bvhGog.rank(10) << std::endl;
  std::cout << bvhGog.rank_0(10) << std::endl;
  std::cout << bvhGog.select_0(1) << std::endl;
  std::cout << bvhGog.select_1(1) << std::endl;
  std::cout << bvhGog.select_1(4) << std::endl;
  std::cout << bvhGog.select_0(4) << std::endl;
  //std::cout << bvhGog.select_0(6) << std::endl;
  //std::cout << bvhGog.select_0(7) << std::endl;
  //std::cout << bvhGog.select_1(6) << std::endl;
  //bvhGog.select_1(0);

  return 0;
}