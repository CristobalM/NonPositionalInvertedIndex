//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_INVERTEDINDEX_HPP
#define NONPOSINVIDX_INVERTEDINDEX_HPP

#include <memory>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <cmath>

#include <stack>

#include <stdexcept>

#include <vector>

#include "TermOrdering.hpp"
#include "TermGrouping.hpp"


template<class WordToDocFreqMap,
        class WTHandler,
        class BVHandler,
        class RelevanceDocOrder = TFDocOrder<WordToDocFreqMap>,
        class TermGrouping = AnyTermGrouping<BVHandler>>
class InvertedIndex {
public:
  explicit InvertedIndex(WordToDocFreqMap &wordToDocFreqMap) : alphabet_sz(
          (uint) wordToDocFreqMap.getUniqueDocsCount()) {
    buildInvertedIndex(wordToDocFreqMap);
  }


  /** Get the number of documents term_idx appears in **/
  inline uint documentFrequency(WordToDocFreqMap &wordToDocFreqMap, const std::string &term_s) {
    auto word_idx = wordToDocFreqMap.getWordIdxByName(term_s);
    return documentFrequency(word_idx);
  }

  /** Get the ith document in which term_idx appears in, ordered by RelevanceOrder **/
  inline uint ithDoc(WordToDocFreqMap &wordToDocFreqMap, const std::string &term_s, int i) {
    auto word_idx = wordToDocFreqMap.getWordIdxByName(term_s);
    return ithDoc(word_idx, i);
  }

  inline std::string ithDocName(WordToDocFreqMap &wordToDocFreqMap, const std::string &term_s, int i) {
    auto ith_doc = ithDoc(wordToDocFreqMap, term_s, i);
    return wordToDocFreqMap.getDocNameByIdx(ith_doc);
  }

  /** Get the list of documents paths where both term_a and term_b appear **/
  std::vector<std::string> termListIntersectionByDocNames(WordToDocFreqMap &wordToDocFreqMap,
                                                          const std::string &term_a, const std::string &term_b) {
    auto indexes_result = termListIntersection(wordToDocFreqMap, term_a, term_b);
    std::vector<std::string> result;

    for (auto &doc_idx : indexes_result) {
      result.push_back(wordToDocFreqMap.getDocNameByIdx(doc_idx));
    }
    return result;
  }

  /** Get the list of documents paths where any of term_a or term_b appear **/
  std::vector<std::string> termListUnionByDocNames(WordToDocFreqMap &wordToDocFreqMap,
                                                   const std::string &term_a, const std::string &term_b) {
    auto indexes_result = termListUnion(wordToDocFreqMap, term_a, term_b);
    std::vector<std::string> result;

    for (auto &doc_idx : indexes_result) {
      result.push_back(wordToDocFreqMap.getDocNameByIdx(doc_idx));
    }
    return result;
  }

  /** Get the list of documents indexes where both term_a and term_b appear **/
  std::vector<int> termListIntersection(WordToDocFreqMap &wordToDocFreqMap,
                                        const std::string &term_a, const std::string &term_b) {
    auto term_a_idx = wordToDocFreqMap.getWordIdxByName(term_a);
    auto term_b_idx = wordToDocFreqMap.getWordIdxByName(term_b);
    auto inner_idx_a = word_idx_mapping[term_a_idx];
    auto inner_idx_b = word_idx_mapping[term_b_idx];

    return termListIntersection(inner_idx_a, inner_idx_b);
  }

  /** Get the list of documents indexes where any of term_a or term_b appear **/
  std::vector<int> termListUnion(WordToDocFreqMap &wordToDocFreqMap,
                                 const std::string &term_a, const std::string &term_b) {
    auto term_a_idx = wordToDocFreqMap.getWordIdxByName(term_a);
    auto term_b_idx = wordToDocFreqMap.getWordIdxByName(term_b);
    auto inner_idx_a = word_idx_mapping[term_a_idx];
    auto inner_idx_b = word_idx_mapping[term_b_idx];

    return termListUnion(inner_idx_a, inner_idx_b);
  }


  WTHandler &getWTHandler() {
    return *wtHandler;
  }



private:
  std::unique_ptr<WTHandler> wtHandler;
  std::unique_ptr<BVHandler> terms_separator;
  std::vector<int> word_idx_mapping;

  uint alphabet_sz;


  void buildInvertedIndex(WordToDocFreqMap &wordToDocFreqMap) {
    auto orderedByRelevance = RelevanceDocOrder::order(wordToDocFreqMap);
    auto grouping = TermGrouping::group(orderedByRelevance);
    auto &raw_result_sequence = grouping.raw_result_sequence;
    word_idx_mapping = std::move(grouping.word_idx_mapping);
    terms_separator = std::move(grouping.terms_separator);
    wtHandler = std::make_unique<WTHandler>(raw_result_sequence);

  }

  std::pair<uint, uint> getTermInterval(int term_idx) {
    auto inner_idx = word_idx_mapping[term_idx];
    auto it = terms_separator->select_1(inner_idx);
    auto ft = terms_separator->succ(it + 1) - 1;
    return {it, ft};
  }

  uint documentFrequency(int term_idx) {
    if (term_idx == -1) {
      return 0;
    }
    if (!(term_idx >= 1 && term_idx < (int)word_idx_mapping.size())) {
      throw std::runtime_error("term idx out of range: " +
                               std::to_string(term_idx) +
                               ". Range: [1, " + std::to_string(word_idx_mapping.size()) + ")");
    }
    auto[it, ft] = getTermInterval(term_idx);
    return ft - it + 1;
  }

  uint ithDoc(int term_idx, int i) {
    if (term_idx == -1) {
      return 0;
    }
    if (!(term_idx >= 1 && term_idx < (int)word_idx_mapping.size())) {
      throw std::runtime_error("term idx out of range: " +
                               std::to_string(term_idx) +
                               ". Range: [1, " + std::to_string(word_idx_mapping.size()) + ")");
    }

    auto[it, ft] = getTermInterval(term_idx);

    if(it + i - 1 > ft){
      throw std::runtime_error("#"+std::to_string(i) +
      " document does not exist for term with index " + std::to_string(term_idx));
    }

    return wtHandler->access(it + i - 1);
  }

  using WTNode = typename WTHandler::WTNode;
  struct TraversalNode {
    uint left_symbol;
    uint right_symbol;

    uint first_term_left_idx;
    uint first_term_right_idx;

    uint second_term_left_idx;
    uint second_term_right_idx;

    WTNode wt_node;
  };

  static inline bool reachedSymbol(TraversalNode &currentTNode) {
    return currentTNode.left_symbol == currentTNode.right_symbol;
  }

  static inline bool anyOutOfBounds(TraversalNode &currentTNode) {
    return firstOutOfBounds(currentTNode) || secondOutOfBounds(currentTNode);
  }

  static inline bool bothOutOfBounds(TraversalNode &currentTNode) {
    return firstOutOfBounds(currentTNode) && secondOutOfBounds(currentTNode);
  }

  static inline bool firstOutOfBounds(TraversalNode &currentTNode) {
    return currentTNode.first_term_left_idx > currentTNode.first_term_right_idx;
  }

  static inline bool secondOutOfBounds(TraversalNode &currentTNode) {
    return currentTNode.second_term_left_idx > currentTNode.second_term_right_idx;
  }


  struct AndOperation {
    static inline bool failCondition(TraversalNode &currentTNode) {
      return anyOutOfBounds(currentTNode);
    }

    static inline void setTermVariables(WTHandler &wt_handler, TraversalNode &currentTNode,
                                        uint *first_term_left_idx, uint *first_term_right_idx,
                                        uint *second_term_left_idx, uint *second_term_right_idx) {
      *first_term_left_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                      currentTNode.first_term_left_idx - 1) + 1;
      *first_term_right_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                       currentTNode.first_term_right_idx);

      *second_term_left_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                       currentTNode.second_term_left_idx - 1) + 1;
      *second_term_right_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                        currentTNode.second_term_right_idx);
    }

    static inline void reachedSymbolAction(std::vector<int> &intersection_result, TraversalNode &currentTNode) {
      intersection_result.push_back(currentTNode.left_symbol);
    }
  };

  struct OrOperation {
    bool first_fail = false;
    bool second_fail = false;
    std::unordered_map<uint, bool> foundTerms;

    inline bool failCondition(TraversalNode &currentTNode) {
      first_fail = firstOutOfBounds(currentTNode);
      second_fail = secondOutOfBounds(currentTNode);

      return first_fail && second_fail; // bothOutOfBounds

    }

    inline void setTermVariables(WTHandler &wt_handler, TraversalNode &currentTNode,
                                 uint *first_term_left_idx, uint *first_term_right_idx,
                                 uint *second_term_left_idx, uint *second_term_right_idx) {

      if (!first_fail) {
        *first_term_left_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                        currentTNode.first_term_left_idx - 1) + 1;
        *first_term_right_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                         currentTNode.first_term_right_idx);
      } else {
        *first_term_left_idx = currentTNode.wt_node.size + 1;
        *first_term_right_idx = 0;
      }
      if (!second_fail) {
        *second_term_left_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                         currentTNode.second_term_left_idx - 1) + 1;
        *second_term_right_idx = wt_handler.innerBVRank_0(currentTNode.wt_node,
                                                          currentTNode.second_term_right_idx);
      } else {
        *second_term_left_idx = currentTNode.wt_node.size + 1;
        *second_term_right_idx = 0;
      }
    }

    inline void reachedSymbolAction(std::vector<int> &intersection_result, TraversalNode &currentTNode) {
      auto term = currentTNode.left_symbol;
      if (foundTerms.find(term) == foundTerms.end()) {
        foundTerms[term] = true;
        intersection_result.push_back(currentTNode.left_symbol);
      }

    }

  };

  inline std::vector<int> termListIntersection(uint termAIdx, uint termBIdx) {
    return treeTraversal<AndOperation>(termAIdx, termBIdx);
  }


  inline std::vector<int> termListUnion(uint termAIdx, uint termBIdx) {
    return treeTraversal<OrOperation>(termAIdx, termBIdx);
  }


  template<class TraversalOperation>
  std::vector<int> treeTraversal(uint termAIdx, uint termBIdx) {
    std::vector<int> intersection_result;

    auto[itA, ftA] = getTermInterval(termAIdx);
    auto[itB, ftB] = getTermInterval(termBIdx);

    std::stack<TraversalNode> traversalStack;

    auto root = wtHandler->getRoot();
    uint left_symbol_root = 0u;

    auto greaterPowerOf2 = (uint)std::ceil(std::log2(alphabet_sz));  //lg2u + (lg2 > lg2u ? 1u : 0u);

    uint right_symbol_root = (1u << (greaterPowerOf2)) - 1;

    traversalStack.push({left_symbol_root, right_symbol_root,
                         itA, ftA,
                         itB, ftB,
                         root});

    TraversalOperation traversalOperation;

    while (!traversalStack.empty()) {
      auto currentTNode = std::move(traversalStack.top());
      traversalStack.pop();


      if (traversalOperation.failCondition(currentTNode)) {
        continue;
      }

      if (reachedSymbol(currentTNode)) {
        traversalOperation.reachedSymbolAction(intersection_result, currentTNode);
        continue;
      }

      if (wtHandler->isLeaf(currentTNode.wt_node)) {
        continue;
      }

      uint m = (currentTNode.left_symbol + currentTNode.right_symbol) >> 1u;

      uint first_term_left_idx, first_term_right_idx, second_term_left_idx, second_term_right_idx;

      traversalOperation.setTermVariables(*wtHandler, currentTNode, &first_term_left_idx, &first_term_right_idx,
                                          &second_term_left_idx, &second_term_right_idx);

      auto[left_child, right_child] = wtHandler->getChildren(currentTNode.wt_node);


      TraversalNode traversal_node_left = {
              currentTNode.left_symbol, m,
              first_term_left_idx, first_term_right_idx,
              second_term_left_idx, second_term_right_idx,
              left_child
      };

      TraversalNode traversal_node_right = {
              m + 1, currentTNode.right_symbol,
              currentTNode.first_term_left_idx - first_term_left_idx + 1,
              currentTNode.first_term_right_idx - first_term_right_idx,
              currentTNode.second_term_left_idx - second_term_left_idx + 1,
              currentTNode.second_term_right_idx - second_term_right_idx,
              right_child
      };

      traversalStack.push(std::move(traversal_node_right));
      traversalStack.push(std::move(traversal_node_left));
    }

    return intersection_result;
  }
};


#endif //NONPOSINVIDX_INVERTEDINDEX_HPP
