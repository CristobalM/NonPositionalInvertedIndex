//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_TERMORDERING_HPP
#define NONPOSINVIDX_TERMORDERING_HPP

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
#endif //NONPOSINVIDX_TERMORDERING_HPP
