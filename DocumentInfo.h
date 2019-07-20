//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_DOCUMENTINFO_H
#define NONPOSINVIDX_DOCUMENTINFO_H


#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <unordered_map>
#include <regex>
#include <iostream>

class DocumentInfo {
  std::vector<std::pair<std::string, int>> words_freq_pairs;
  int words_count;
  int unique_words_count;
  std::string filename;

  static inline bool hasKey(std::unordered_map<std::string, int> &map_, const std::string &key);

public:
  explicit DocumentInfo(const std::string &filename);

  void readDocument();

  std::vector<std::pair<std::string, int>> &getWordsFreqPairs();

  void clean();
};


#endif //NONPOSINVIDX_DOCUMENTINFO_H
