//
// Created by Cristobal Miranda, 2019
//

#include <iostream>
#include "NonPosInvIdx.h"
#include "DocumentsHandler.h"

int main(int argc, char **argv){
  if(argc <= 3){
    throw std::runtime_error("Expected syntax: ./mifluz_comparison DIRECTORY_PATH WORD_1 WORD_2");
  }

  std::string dir_path = argv[1];
  std::string word1 = argv[2];
  std::string word2 = argv[3];

  std::cout << "Documents directory: " << dir_path << std::endl;
  DocumentsHandler documentsHandler;

  documentsHandler.initDocumentsFromDirectory(dir_path);
  documentsHandler.scanWords();
  documentsHandler.debugPrintScannedWords();

  NonPosInvIdx invertedIndex(documentsHandler);

  return 0;
}