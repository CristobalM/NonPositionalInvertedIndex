//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_IWORDTODOCFREQMAP_H
#define NONPOSINVIDX_IWORDTODOCFREQMAP_H

#include <vector>
#include <utility>

class IWordToDocFreqMap{
public:
  virtual std::vector<std::pair<int, int>> &getWordDocs(int word_idx) = 0;
  virtual int getUniqueWordsCount() = 0;
};
#endif //NONPOSINVIDX_IWORDTODOCFREQMAP_H
