//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_DOCUMENTINFO_HPP
#define NONPOSINVIDX_DOCUMENTINFO_HPP


#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <unordered_map>
#include <regex>
#include <iostream>

/** Class that is used to read a single document and extract word occurrences count from it **/
class DocumentInfo {
  std::vector<std::pair<std::string, int>> words_freq_pairs;

  std::string filename;

  static inline bool hasKey(std::unordered_map<std::string, int> &map_, const std::string &key);

public:
  explicit DocumentInfo(const std::string &filename);

  void readDocument(const std::regex &word_regex);

  std::vector<std::pair<std::string, int>> &getWordsFreqPairs();

  int getUniqueWC();

  void clean();
};


#endif //NONPOSINVIDX_DOCUMENTINFO_HPP
