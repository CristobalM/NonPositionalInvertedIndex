#include <iostream>


#include <stdexcept>

#include "DocumentsHandler.h"
#include "InvertedIndex.hpp"
#include "WTHGog.hpp"
#include "BVHGog.hpp"

#include <sdsl/wavelet_trees.hpp>

int main(int argc, char **argv) {

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

  //std::cout << "RANK (0, t=0) = " << wtHandler.rank(0, 0) << std::endl;
  //std::cout << "RANK (13, t=0) = " << wtHandler.rank(13, 0) << std::endl;

  auto &internalWT = wtHandler.getInternalStructure();
  std::cout << "wt.sigma: " <<internalWT.sigma << std::endl;

  std::cout << "Internal concatenation of the wt bvs: \n";
  for(int i = 0; i < internalWT.tree.size(); i++){
    std::cout << internalWT.tree[i] << "";
  }
  std::cout << std::endl;


  std::cout << "Term list intersection\n";

  auto printVecS = [](std::vector<std::string> &inputV){
    for(auto &val : inputV){
      std::cout << val << " ";
    }
    std::cout << std::endl;
  };
  auto printVecI = [](std::vector<int> &inputV){
    for(auto &val : inputV){
      std::cout << val << " ";
    }
    std::cout << std::endl;
  };


  auto firstWord = "SI";
  auto secondWord = "HOLA";

  auto r1 = invertedIndex.termListIntersectionByDocNames(documentsHandler, firstWord, secondWord);
  auto r1_ = invertedIndex.termListIntersection(documentsHandler, firstWord, secondWord);
  std::cout << "r1: ";
  printVecS(r1);
  std::cout << "r1_: ";
  printVecI(r1_);

  auto r2 = invertedIndex.termListUnionByDocNames(documentsHandler, firstWord, secondWord);
  std::cout << "r2: ";
  printVecS(r2);
  auto r2_ = invertedIndex.termListUnion(documentsHandler, firstWord, secondWord);
  std::cout << "r2_: ";
  printVecI(r2_);


  std::cout << "Document names: " << std::endl;
  const auto &documentNames = documentsHandler.getDocumentNames();
  for(int i = 0; i < documentNames.size(); i++){
    std::cout << i << ": " << documentNames[i] << std::endl;
  }

/*

  std::vector<uint32_t> inputWT = {7,1,0,0,3,0,1,2,0,1,0,3,4,5,6,7};
  WTHGog<> pruebaWT(inputWT);

  auto &wt = pruebaWT.getInternalStructure();

  std::cout << "PRUEBA \n";
  std::cout << wt.tree << std::endl;




  auto printNodeInfo = [](const std::string& name, sdsl::wt_int<>::node_type &node){
    std::cout << name <<" offset = " << node.offset << std::endl;
    std::cout << name << " size = " << node.size << std::endl;
    std::cout << name << " level = " << node.level << std::endl;
  };


  auto root = wt.root();

  //printNodeInfo("root", root);

  auto [lc, rc] = wt.expand(root);

  //printNodeInfo("left child root", lc);

  auto rk_test1 = pruebaWT.innerBVRank(root, 1);
  std::cout << "rk_test_1 = " << rk_test1 << std::endl;


  //std::cout << "PRUEBA: " << wt.tree_rank(1) << std::endl;

  auto rk_test2 = pruebaWT.innerBVRank_0(lc, 4);
  std::cout << "rk_test2 = " << rk_test2 << std::endl;

*/


  return 0;
}