//
// Created by Cristobal Miranda, 2019
//

#include "DocumentInfo.h"

void DocumentInfo::readDocument() {
  std::unordered_map<std::string, int> word_count;
  std::vector<std::string> unique_words;

  std::ifstream file;
  file.open(filename);

  std::string line;
  while (std::getline(file, line)) {
    std::smatch sm;

    for (auto r_it = std::sregex_iterator(line.begin(), line.end(), WORD_REGEX); r_it != std::sregex_iterator(); r_it++) {
      auto match = *r_it;
      auto word = match.str(0);
      if (!hasKey(word_count, word)) {
        word_count[word] = 0;
        unique_words.push_back(word);
      }
      word_count[word]++;
    }
  }

  for (auto &uw : unique_words) {
    words_freq_pairs.emplace_back(uw, word_count[uw]);
  }

  file.close();
}

DocumentInfo::DocumentInfo(const std::string &filename) : filename(filename), words_count(0){
}

std::vector<std::pair<std::string, int>> &DocumentInfo::getWordsFreqPairs() {
  return words_freq_pairs;
}

void DocumentInfo::clean() {
  words_freq_pairs.clear();
}

bool DocumentInfo::hasKey(std::unordered_map<std::string, int> &map_, const std::string &key){
  return map_.find(key) != map_.end();
}

int DocumentInfo::getUniqueWC() {
  return  words_freq_pairs.size();
}

