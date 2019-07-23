//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_INVERTEDINDEX_H
#define NONPOSINVIDX_INVERTEDINDEX_H

#include <memory>
#include <algorithm>
#include <iostream>

#include <stdexcept>

#include "BVHGog.h"


// Term-frequency order
template<class WordToDocFreqMap>
struct TFDocOrder {

  using pi = std::pair<int, int>;

  static void printVector(std::vector<int> &v) { // debug
    for (auto &e : v) {
      std::cout << e << " ";
    }
    std::cout << std::endl;
  }

  static bool compareBySecond(const pi &p_a, const pi &p_b) {
    return p_a.second > p_b.second;
  }


  static std::vector<std::vector<int>> order(WordToDocFreqMap &wordToDocFreqMap) {
    std::vector<std::vector<int>> result;

    auto uwc = wordToDocFreqMap.getUniqueWordsCount();
    for (unsigned long i = 1; i <= uwc; i++) {
      auto &iDocList = wordToDocFreqMap.getWordDocs(i);
      std::vector<int> indexes(iDocList.size());
      std::vector<int> frequencies(iDocList.size());

      std::vector<pi> sortedVec(iDocList);
      std::sort(sortedVec.begin(), sortedVec.end(), compareBySecond);

      for (unsigned long j = 0; j < sortedVec.size(); j++) {
        indexes[j] = sortedVec[j].first;
        frequencies[j] = sortedVec[j].second;
      }

      result.push_back(indexes);
    }
    return result;
  };
};

template<class BVHandler, typename AlphabetType = uint32_t >
struct GroupedTerms {
  std::vector<AlphabetType> raw_result_sequence;
  std::unique_ptr<BVHandler> terms_separator;
  std::vector<int> word_idx_mapping;
};

template<class BVHandler, typename AlphabetType = uint32_t >
struct AnyTermGrouping {
  static void printVector(std::vector<AlphabetType> &v) {
    for (auto &e : v) {
      std::cout << e << " ";
    }
    std::cout << std::endl;
  }

  static GroupedTerms<BVHandler> group(std::vector<std::vector<int>> &input_vs) {
    std::vector<AlphabetType> raw_result_sequence;
    std::vector<int> word_idx_mapping(input_vs.size() + 1);
    int separator_sz = 0;
    for (auto &input_v : input_vs) {
      separator_sz += input_v.size();
    }

    auto separator = std::make_unique<BVHandler>(separator_sz);

    unsigned long offset = 1;
    separator->bitset(offset);
    unsigned long count = 1;
    for (auto &input_v : input_vs) {
      word_idx_mapping[count] = count;
      count++;
      separator->bitset(offset);
      offset += input_v.size();
      for (auto &docIdx : input_v) {
        raw_result_sequence.push_back(docIdx);
      }
    }

    separator->buildStructures();


    return {std::move(raw_result_sequence), std::move(separator), word_idx_mapping};
  }
};

template<class WordToDocFreqMap,
        class WTHandler,
        class BVHandler,
        class RelevanceDocOrder = TFDocOrder<WordToDocFreqMap>,
        class TermGrouping = AnyTermGrouping<BVHandler>>
class InvertedIndex {
  std::unique_ptr<WTHandler> wtHandler;
  std::unique_ptr<BVHandler> terms_separator;
  std::vector<int> word_idx_mapping;


public:
  explicit InvertedIndex(WordToDocFreqMap &wordToDocFreqMap) {
    buildInvertedIndex(wordToDocFreqMap);
  }

  void buildInvertedIndex(WordToDocFreqMap &wordToDocFreqMap) {
    auto orderedByRelevance = RelevanceDocOrder::order(wordToDocFreqMap);
    auto grouping = TermGrouping::group(orderedByRelevance);
    auto &raw_result_sequence = grouping.raw_result_sequence;
    word_idx_mapping = std::move(grouping.word_idx_mapping);
    terms_separator = std::move(grouping.terms_separator);
    wtHandler = std::make_unique<WTHandler>(raw_result_sequence);

    terms_separator->debugDisplay();
  }

  inline uint documentFrequency(int term_idx) {
    if (term_idx == -1) {
      return 0;
    }
    if (!(term_idx >= 1 && term_idx < word_idx_mapping.size())) {
      throw std::runtime_error("term idx out of range: " +
                               std::to_string(term_idx) +
                               ". Range: [1, " + std::to_string(word_idx_mapping.size()) + ")");
    }
    auto inner_idx = word_idx_mapping[term_idx];
    auto it = terms_separator->select_1(inner_idx);
    auto ft = terms_separator->succ(it + 1) - 1;
    return ft - it + 1;
  }

  inline uint ithDoc(int term_idx, int i) {
    if (term_idx == -1) {
      return 0;
    }
    if (!(term_idx >= 1 && term_idx < word_idx_mapping.size())) {
      throw std::runtime_error("term idx out of range: " +
                               std::to_string(term_idx) +
                               ". Range: [1, " + std::to_string(word_idx_mapping.size()) + ")");
    }

    auto inner_idx = word_idx_mapping[term_idx];
    auto it = terms_separator->select_1(inner_idx);
    wtHandler->access(it+i-1);

  }

  inline uint documentFrequency(WordToDocFreqMap &wordToDocFreqMap, const std::string &term_s) {
    auto word_idx = wordToDocFreqMap.getWordIdxByName(term_s);
    return documentFrequency(word_idx);
  }

  inline uint ithDoc(WordToDocFreqMap &wordToDocFreqMap, const std::string &term_s, int i){
    auto word_idx = wordToDocFreqMap.getWordIdxByName(term_s);
    return ithDoc(word_idx, i);
  }

  WTHandler &getWTHandler(){
    return *wtHandler;
  }

};


#endif //NONPOSINVIDX_INVERTEDINDEX_H
