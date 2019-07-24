#include <iostream>
#include <stdexcept>

#include "NonPosInvIdx.h"

template<class vtype>
void printVec(std::vector<vtype> &v){
  for(auto &val : v){
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

// Receives directory path with only text files inside
int main(int argc, char **argv) {
  if(argc <= 1){
    throw std::runtime_error("Documents directory missing (First argument)");
  }

  std::cout << "Documents directory: " << argv[1] << std::endl;
  DocumentsHandler documentsHandler(true);

  documentsHandler.initDocumentsFromDirectory(argv[1]);
  documentsHandler.scanWords();
  documentsHandler.debugPrintScannedWords();

  NonPosInvIdx invertedIndex(documentsHandler);

  std::string wordToQuery("UNO");
  auto docFreqWTQ = invertedIndex.documentFrequency(documentsHandler, wordToQuery);
  std::cout << wordToQuery << " DOCUMENT FREQUENCY = " << docFreqWTQ << std::endl;

  int idx_to_query = 3;
  auto secondSI = invertedIndex.ithDoc(documentsHandler, wordToQuery, idx_to_query);
  std::cout << idx_to_query << "th " << wordToQuery << ": " << secondSI << std::endl;

  auto &wtHandler = invertedIndex.getWTHandler();
  auto &internalWT = wtHandler.getInternalStructure();

  std::cout << "Term list intersection\n";



  auto firstWord = "SI";
  auto secondWord = "HOLA";

  auto r1 = invertedIndex.termListIntersectionByDocNames(documentsHandler, firstWord, secondWord);
  auto r1_ = invertedIndex.termListIntersection(documentsHandler, firstWord, secondWord);
  std::cout << "r1: ";
  printVec(r1);
  std::cout << "r1_: ";
  printVec(r1_);

  auto r2 = invertedIndex.termListUnionByDocNames(documentsHandler, firstWord, secondWord);
  std::cout << "r2: ";
  printVec(r2);
  auto r2_ = invertedIndex.termListUnion(documentsHandler, firstWord, secondWord);
  std::cout << "r2_: ";
  printVec(r2_);


  std::cout << "Document names: " << std::endl;
  const auto &documentNames = documentsHandler.getDocumentNames();
  for(int i = 0; i < documentNames.size(); i++){
    std::cout << i << ": " << documentNames[i] << std::endl;
  }


  return 0;
}