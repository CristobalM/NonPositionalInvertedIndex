//
// Created by Cristobal Miranda, 2019
//
#include <iostream>
#include <string>
#include "DocumentsHandler.hpp"
#include "NonPosInvIdx.hpp"


template<class vtype>
void printVec(std::vector<vtype> &v){
  for(auto &val : v){
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

int main() {

  std::string wordToQuery = "dolor";
  int idx_to_query = 1;

  auto loadedDocHandlerP = DocumentsHandler::load("documentsHandler");
  auto &loadedDocHandler = *loadedDocHandlerP;

  auto loadedIdxP = NonPosInvIdx::load(".", "simpleIndex");
  auto &loadedIdx = loadedIdxP;



  std::cout << "On restored index" << std::endl;

  auto docFreqWTQ2 = loadedIdx.documentFrequency(loadedDocHandler, wordToQuery);
  std::cout << wordToQuery << " Document frequency = " << docFreqWTQ2 << std::endl;

  auto secondSI2 = loadedIdx.ithDocName(loadedDocHandler, wordToQuery, idx_to_query);
  std::cout << "#" << idx_to_query << " " << wordToQuery << ": " << secondSI2 << std::endl;

  std::cout << "Pairwise term operations\n";

  auto firstWord2 = "dolor";
  auto secondWord2 = "Lorem";

  auto and_fpath2 = loadedIdx.termListIntersectionByDocNames(loadedDocHandler, firstWord2, secondWord2);
  auto and_indexes2 = loadedIdx.termListIntersection(loadedDocHandler, firstWord2, secondWord2);

  std::cout << "AND (file paths): ";
  printVec(and_fpath2);
  std::cout << "AND (document indexes): ";
  printVec(and_indexes2);

  auto or_fpath2 = loadedIdx.termListUnionByDocNames(loadedDocHandler, firstWord2, secondWord2);
  auto or_indexes2 = loadedIdx.termListUnion(loadedDocHandler, firstWord2, secondWord2);

  std::cout << "OR (file paths): ";
  printVec(or_fpath2);
  std::cout << "OR (document indexes): ";
  printVec(or_indexes2);


  return 0;
}