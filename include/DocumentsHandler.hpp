//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_DOCUMENTSHANDLER_HPP
#define NONPOSINVIDX_DOCUMENTSHANDLER_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include <experimental/filesystem>

#include <iostream>
#include <sstream>

#include "DocumentInfo.hpp"

namespace fs = std::experimental::filesystem;
class DocumentsHandler {
  std::vector<std::unique_ptr<DocumentInfo>> documents;
  std::unordered_map<std::string, int> words_map;
  std::unordered_map<int, std::string> words_map_inv;
  std::unordered_map<int, std::vector<std::pair<int, int>>> words_to_docs_map; // word_idx -> [(doc_idx, freq_in_doc)...]

  std::vector<std::string> documentsNames;

  bool clean_on_scan;
  int last_assigned_int;
  std::regex word_regex;


  bool wordExists(const std::string &word);

public:
  explicit DocumentsHandler(bool clean_on_scan = true, const std::string &word_regex_str = "([^\\s]+)");

  void initDocumentsFromDirectory(const std::string &directory_path);

  void scanWords();

  void debugPrintScannedWords();

  void cleanData();

  std::vector<std::pair<int, int>> &getWordDocs(int word_idx);

  int getWordIdxByName(const std::string &word_name);
  std::string getDocNameByIdx(int doc_idx);

  unsigned long getUniqueWordsCount();

  uint getUniqueDocsCount();

  const std::vector<std::string> &getDocumentNames() const{
    return documentsNames;
  }
};


#endif //NONPOSINVIDX_DOCUMENTSHANDLER_HPP
