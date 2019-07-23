#include <iostream>


#include <stdexcept>

#include "DocumentsHandler.h"
#include "InvertedIndex.hpp"
#include "WTHGog.hpp"
#include "BVHGog.hpp"

#include <sdsl/wavelet_trees.hpp>

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

  auto printVec = [](std::vector<std::string> &inputV){
    for(auto &val : inputV){
      std::cout << val << " ";
    }
    std::cout << std::endl;
  };



  auto r1 = invertedIndex.termListIntersectionByDocNames(documentsHandler, "SI", "UNO");
  std::cout << "r1: ";
  printVec(r1);




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