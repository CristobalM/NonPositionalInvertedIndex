//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_INVERTEDINDEX_H
#define NONPOSINVIDX_INVERTEDINDEX_H

#include <memory>


template<class WordToDocFreqMap, class WTHandler>
class InvertedIndex {
  WTHandler wtHandler;

public:
  InvertedIndex(WordToDocFreqMap &wordToDocFreqMap){
    buildInvertedIndex(wordToDocFreqMap);
  }

  void buildInvertedIndex(WordToDocFreqMap &wordToDocFreqMap){
    auto uwc = wordToDocFreqMap.getUniqueWordsCount();
    for(auto wordIdx = 1; wordIdx <= uwc; wordIdx++){
      auto &docList = wordToDocFreqMap.getWordDocs(wordIdx);

    }
  }

};


#endif //NONPOSINVIDX_INVERTEDINDEX_H
