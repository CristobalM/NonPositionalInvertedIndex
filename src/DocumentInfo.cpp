//
// Created by Cristobal Miranda, 2019
//

#include "DocumentInfo.hpp"

void DocumentInfo::readDocument(const std::regex &word_regex) {
  std::unordered_map<std::string, int> word_count;
  std::vector<std::string> unique_words;

  std::ifstream file;
  file.open(filename);

  std::stringstream sbuffer;
  sbuffer << file.rdbuf();

  auto str_buffer = sbuffer.str();


  std::smatch sm;

  for (auto r_it = std::sregex_iterator(str_buffer.begin(), str_buffer.end(), word_regex); r_it != std::sregex_iterator(); r_it++) {
    auto match = *r_it;
    auto word = match.str(0);
    if (!hasKey(word_count, word)) {
      word_count[word] = 0;
      unique_words.push_back(word);
    }
    word_count[word]++;
  }

  /* This is probably less efficient in CPU but more efficient in simultaneous memory loaded
   * If very large documents were needed that cannot be kept in memory, this way should be preferred
   *
  std::string line;
  while (std::getline(file, line)) {
    std::smatch sm;

    for (auto r_it = std::sregex_iterator(line.begin(), line.end(), word_regex); r_it != std::sregex_iterator(); r_it++) {
      auto match = *r_it;
      auto word = match.str(0);
      if (!hasKey(word_count, word)) {
        word_count[word] = 0;
        unique_words.push_back(word);
      }
      word_count[word]++;
    }
  }
   */

  for (auto &uw : unique_words) {
    words_freq_pairs.emplace_back(uw, word_count[uw]);
  }

  file.close();
}

DocumentInfo::DocumentInfo(const std::string &filename) : filename(filename){
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

std::string &DocumentInfo::getFilename() {
  return filename;
}

