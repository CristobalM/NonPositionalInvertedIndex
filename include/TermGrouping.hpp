//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_TERMGROUPING_H
#define NONPOSINVIDX_TERMGROUPING_H

/** Structure to store the result of term grouping in a WT **/
template<class BVHandler, typename AlphabetType = uint32_t>
struct GroupedTerms {
  std::vector<AlphabetType> raw_result_sequence;
  std::unique_ptr<BVHandler> terms_separator;
  std::vector<int> word_idx_mapping;
};

/** Concrete implementation for no particular grouping of terms **/
template<class BVHandler, typename AlphabetType = uint32_t>
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

#endif //NONPOSINVIDX_TERMGROUPING_H
