#include <iostream>
#include <stdexcept>

#include "NonPosInvIdx.h"
#include "DocumentsHandler.h"

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
  DocumentsHandler documentsHandler(true);

  documentsHandler.initDocumentsFromDirectory(dir_path);
  documentsHandler.scanWords();
  documentsHandler.debugPrintScannedWords();

  NonPosInvIdx invertedIndex(documentsHandler);

  std::string &wordToQuery = word1;
  auto docFreqWTQ = invertedIndex.documentFrequency(documentsHandler, wordToQuery);
  std::cout << wordToQuery << " Document frequency = " << docFreqWTQ << std::endl;

  int idx_to_query = 1;
  auto secondSI = invertedIndex.ithDocName(documentsHandler, wordToQuery, idx_to_query);
  std::cout << "#" << idx_to_query << " " << wordToQuery << ": " << secondSI << std::endl;

  auto &wtHandler = invertedIndex.getWTHandler();
  auto &internalWT = wtHandler.getInternalStructure();

  std::cout << "Term list intersection\n";

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
  for(int i = 0; i < documentNames.size(); i++){
    std::cout << i << ": " << documentNames[i] << std::endl;
  }


  return 0;
}