#include <iostream>
#include <stdexcept>

#include "NonPosInvIdx.hpp"
#include "DocumentsHandler.hpp"

template<class vtype>
void printVec(std::vector<vtype> &v){
  for(auto &val : v){
    std::cout << val << " ";
  }
  std::cout << std::endl;
}

// Receives directory path with only text files inside
int main(int argc, char **argv) {
  if(argc <= 3){
    throw std::runtime_error("Expected syntax: ./example_1 DIRECTORY_PATH WORD_1 WORD_2");
  }

  std::string dir_path = argv[1];
  std::string word1 = argv[2];
  std::string word2 = argv[3];

  std::cout << "Documents directory: " << dir_path << std::endl;
  DocumentsHandler documentsHandler;

  documentsHandler.initDocumentsFromDirectory(dir_path);
  documentsHandler.scanWords();
  documentsHandler.debugPrintScannedWords();

  NonPosInvIdx invertedIndex(documentsHandler, "simpleIndex");

  std::string &wordToQuery = word1;
  auto docFreqWTQ = invertedIndex.documentFrequency(documentsHandler, wordToQuery);
  std::cout << wordToQuery << " Document frequency = " << docFreqWTQ << std::endl;

  int idx_to_query = 1;
  auto secondSI = invertedIndex.ithDocName(documentsHandler, wordToQuery, idx_to_query);
  std::cout << "#" << idx_to_query << " " << wordToQuery << ": " << secondSI << std::endl;

  std::cout << "Pairwise term operations\n";

  auto &firstWord = word1;
  auto &secondWord = word2;

  auto and_fpath = invertedIndex.termListIntersectionByDocNames(documentsHandler, firstWord, secondWord);
  auto and_indexes = invertedIndex.termListIntersection(documentsHandler, firstWord, secondWord);

  std::cout << "AND (file paths): ";
  printVec(and_fpath);
  std::cout << "AND (document indexes): ";
  printVec(and_indexes);

  auto or_fpath = invertedIndex.termListUnionByDocNames(documentsHandler, firstWord, secondWord);
  auto or_indexes = invertedIndex.termListUnion(documentsHandler, firstWord, secondWord);

  std::cout << "OR (file paths): ";
  printVec(or_fpath);
  std::cout << "OR (document indexes): ";
  printVec(or_indexes);


  std::cout << "\n\nDocument names: " << std::endl;
  const auto &documentNames = documentsHandler.getDocumentNames();
  for(auto i = 0ul; i < documentNames.size(); i++){
    std::cout << i << ": " << documentNames[i] << std::endl;
  }

  invertedIndex.save(".");
  documentsHandler.save("documentsHandler");

  auto loadedDocHandlerP = DocumentsHandler::load("documentsHandler");
  auto &loadedDocHandler = *loadedDocHandlerP;

  auto loadedIdxP = NonPosInvIdx::load(".", "simpleIndex");
  auto &loadedIdx = loadedIdxP;

  std::cout << "\n\n\nOn restored index" << std::endl;

  auto docFreqWTQ2 = loadedIdx.documentFrequency(loadedDocHandler, wordToQuery);
  std::cout << wordToQuery << " Document frequency = " << docFreqWTQ2 << std::endl;

  auto secondSI2 = loadedIdx.ithDocName(loadedDocHandler, wordToQuery, idx_to_query);
  std::cout << "#" << idx_to_query << " " << wordToQuery << ": " << secondSI << std::endl;

  std::cout << "Pairwise term operations\n";

  auto &firstWord2 = word1;
  auto &secondWord2 = word2;

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

  auto thirdWord = "dictum";

  std::vector<std::string> query_3{firstWord2, secondWord2, thirdWord};

  auto and_fpath3 = loadedIdx.termListIntersectionByDocNames(loadedDocHandler, query_3);
  auto and_indexes3 = loadedIdx.termListIntersection(loadedDocHandler, query_3);

  std::cout << "(More than 2)\nAND (file paths): ";
  printVec(and_fpath3);
  std::cout << "AND (document indexes): ";
  printVec(and_indexes3);


  auto or_fpath3 = loadedIdx.termListUnionByDocNames(loadedDocHandler, query_3);
  auto or_indexes3 = loadedIdx.termListUnion(loadedDocHandler, query_3);

  std::cout << "OR (file paths): ";
  printVec(or_fpath3);
  std::cout << "OR (document indexes): ";
  printVec(or_indexes3);



  return 0;
}