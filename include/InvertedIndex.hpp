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
#include <array>
#include <stdexcept>
#include <vector>
#include <cassert>

#include <chrono>
#include <list>

#include "TermOrdering.hpp"
#include "TermGrouping.hpp"
#include "SimpleStorage.hpp"
#include "CircularQueue.hpp"

/** Inverted Index structure, built from WordToDocFreqMap.
 * Has AND, OR operations **/
template<class WordToDocFreqMap,
        class WTHandler,
        class BVHandler,
        class RelevanceDocOrder = TFDocOrder<WordToDocFreqMap>,
        class TermGrouping = AnyTermGrouping<BVHandler>,
        class Storage = SimpleStorage>
class InvertedIndex {
public:
  InvertedIndex(const InvertedIndex&) = delete;
  
  explicit InvertedIndex(WordToDocFreqMap &wordToDocFreqMap, const std::string &index_name_) :
  alphabet_sz((uint) wordToDocFreqMap.getUniqueDocsCount()), index_name(index_name_) {
    buildInvertedIndex(wordToDocFreqMap);
    silence_results = false;
  }

  explicit InvertedIndex(std::unique_ptr<WTHandler> &&wt_handler_,
          std::unique_ptr<BVHandler> &&terms_separator_,
          std::vector<int> &word_idx_mapping_,
          int alphabet_sz_,
          const std::string &index_name_) :
          wtHandler(std::move(wt_handler_)),
          terms_separator(std::move(terms_separator_)),
          word_idx_mapping(word_idx_mapping_),
          alphabet_sz(alphabet_sz_),
          index_name(index_name_){
    silence_results = false;
  }

  void save(const std::string &index_path){
    wtHandler->save(index_path, index_name, "wtHandler");
    terms_separator->save(index_path, index_name, "terms_separator");
    Storage::save_vi(index_path, index_name, "word_idx_mapping", word_idx_mapping);
    Storage::save_int(index_path, index_name, "alphabet_sz", alphabet_sz);
  }

  static InvertedIndex load(const std::string &index_path, const std::string &index_name){
    auto wt_handler_ = WTHandler::load(index_path, index_name, "wtHandler");
    auto terms_separator_ = BVHandler::load(index_path, index_name, "terms_separator");
    auto word_idx_mapping_ = Storage::load_vi(index_path, index_name, "word_idx_mapping");
    auto alphabet_sz_ = Storage::load_int(index_path, index_name, "alphabet_sz");

    return InvertedIndex(std::move(wt_handler_),
                         std::move(terms_separator_),
                         word_idx_mapping_,
                         alphabet_sz_,
                         index_name);
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

  /** Get the list of documents indexes where all of query_terms appear **/
  std::vector<int> termListIntersection(WordToDocFreqMap &wordToDocFreqMap, std::vector<std::string> &query_terms) {
    auto query_terms_uint = getIndexesFromTerms(wordToDocFreqMap, query_terms);
    return termListIntersection(query_terms_uint);
  }

  /** Get the list of documents indexes where any of query_terms appear **/
  std::vector<int> termListUnion(WordToDocFreqMap &wordToDocFreqMap, std::vector<std::string> &query_terms) {
    auto query_terms_uint = getIndexesFromTerms(wordToDocFreqMap, query_terms);
    return termListUnion(query_terms_uint);
  }

  /** Get the list of documents paths where both term_a and term_b appear **/
  std::vector<std::string> termListIntersectionByDocNames(WordToDocFreqMap &wordToDocFreqMap, std::vector<std::string> &query_terms) {
    auto indexes_result = termListIntersection(wordToDocFreqMap, query_terms);
    std::vector<std::string> result;

    for (auto &doc_idx : indexes_result) {
      result.push_back(wordToDocFreqMap.getDocNameByIdx(doc_idx));
    }
    return result;
  }

  /** Get the list of documents paths where any of term_a or term_b appear **/
  std::vector<std::string> termListUnionByDocNames(WordToDocFreqMap &wordToDocFreqMap, std::vector<std::string> &query_terms) {
    auto indexes_result = termListUnion(wordToDocFreqMap, query_terms);
    std::vector<std::string> result;

    for (auto &doc_idx : indexes_result) {
      result.push_back(wordToDocFreqMap.getDocNameByIdx(doc_idx));
    }
    return result;
  }



  /** For debugging **/
  WTHandler &getWTHandler() {
    return *wtHandler;
  }

  void silenceResults(){
    silence_results = true;
  }



private:
  std::unique_ptr<WTHandler> wtHandler;
  std::unique_ptr<BVHandler> terms_separator;
  std::vector<int> word_idx_mapping;

  uint alphabet_sz;

  std::string index_name;

  bool silence_results;

  std::vector<uint> getIndexesFromTerms(WordToDocFreqMap &wordToDocFreqMap, std::vector<std::string> &terms_list){
    std::vector<uint> indexes_list;
    for(const auto &s : terms_list){
      indexes_list.push_back(word_idx_mapping[wordToDocFreqMap.getWordIdxByName(s)]);
    }
    return indexes_list;
  }


  void buildInvertedIndex(WordToDocFreqMap &wordToDocFreqMap) {
    auto orderedByRelevance = RelevanceDocOrder::order(wordToDocFreqMap);
    wordToDocFreqMap.clearWordsToDocMap(); // memory optimisation
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


  struct TraversalNode{
    uint left_symbol;
    uint right_symbol;

    //std::unique_ptr<std::vector<uint>> left_indexes;
    //std::unique_ptr<std::vector<uint>> right_indexes;

    std::vector<uint> left_indexes;
    std::vector<uint> right_indexes;

    WTNode wt_node;

    uint idx;

  };


  static inline bool reachedSymbol(TraversalNode &currentTNode) {
    return currentTNode.left_symbol == currentTNode.right_symbol;
  }

  struct AndOperation {

    static inline bool failCondition(TraversalNode &currentTNode) {
      for (auto i = 0ul; i < currentTNode.left_indexes.size(); i++) {
        if (currentTNode.left_indexes[i] > currentTNode.right_indexes[i]) {
          return true;
        }
      }
      return false;
    }

    static inline void
    setTermVariables(WTHandler &wt_handler, TraversalNode &currentTNode, std::vector<uint> &left_indexes_lc,
                     std::vector<uint> &right_indexes_lc, std::vector<uint> &left_indexes_rc,
                     std::vector<uint> &right_indexes_rc) {

      for (auto i = 0ul; i < currentTNode.left_indexes.size(); i++) {
        auto current_li = currentTNode.left_indexes[i];
        auto current_ri = currentTNode.right_indexes[i];

        auto left_val_lc = wt_handler.innerBVRank_0(currentTNode.wt_node, current_li - 1) + 1;
        auto right_val_lc = wt_handler.innerBVRank_0(currentTNode.wt_node, current_ri);

        left_indexes_lc[i] = left_val_lc;
        right_indexes_lc[i] = right_val_lc;
        left_indexes_rc[i] = current_li - left_val_lc + 1;
        right_indexes_rc[i] = current_ri - right_val_lc;

      }
    }

    double getElapsedST(){return 0;}
    double getElapsedNF(){return 0;}

    static inline void reachedSymbolAction(std::vector<int> &intersection_result, TraversalNode &currentTNode, InvertedIndex &invi) {
      if(!invi.silence_results){
        intersection_result.push_back(currentTNode.left_symbol);
      }

    }
  };

  struct OrOperation {
    std::list<uint> not_failed;
    std::unordered_map<uint, bool> foundTerms;
    std::chrono::duration<double> duration_loop_st;
    std::chrono::duration<double> duration_loop_nf;

    OrOperation() : duration_loop_st(0), duration_loop_nf(0) {}

    double getElapsedST(){return std::chrono::duration_cast<std::chrono::milliseconds>(duration_loop_st).count();}
    double getElapsedNF(){return std::chrono::duration_cast<std::chrono::milliseconds>(duration_loop_nf).count();}


    bool failCondition(TraversalNode &currentTNode) {
      if(not_failed.size() == 0){
        for(auto i = 0ul; i < currentTNode.left_indexes.size(); i++){
          not_failed.push_back(i);
        }
      }
      auto start_time = std::chrono::steady_clock::now();

      for(auto it = not_failed.begin(); it != not_failed.end(); ){
        auto i = *it;
        auto current_li = currentTNode.left_indexes[i];
        auto current_ri = currentTNode.right_indexes[i];
        if(current_li > current_ri){
          it = not_failed.erase(it);
        }else{
          it++;
        }
      }

      auto end_time = std::chrono::steady_clock::now();
      auto elapsed_time = end_time - start_time;
      duration_loop_nf = duration_loop_nf + elapsed_time;

      return not_failed.size() == 0;
    }

    void setTermVariables(WTHandler &wt_handler, TraversalNode &currentTNode, std::vector<uint> &left_indexes_lc,
                                 std::vector<uint> &right_indexes_lc, std::vector<uint> &left_indexes_rc,
                                 std::vector<uint> &right_indexes_rc) {

      auto start_time = std::chrono::steady_clock::now();


      for(auto it = not_failed.begin(); it != not_failed.end(); it++){
        auto i = *it;
        auto current_li = currentTNode.left_indexes[i];
        auto current_ri = currentTNode.right_indexes[i];

        auto left_val_lc = wt_handler.innerBVRank_0(currentTNode.wt_node, current_li - 1) + 1;
        auto right_val_lc = wt_handler.innerBVRank_0(currentTNode.wt_node, current_ri);

        left_indexes_lc[i] = left_val_lc;
        right_indexes_lc[i] = right_val_lc;
        left_indexes_rc[i] = current_li - left_val_lc + 1;
        right_indexes_rc[i] = current_ri - right_val_lc;
      }
      auto end_time = std::chrono::steady_clock::now();
      auto elapsed_time = (end_time - start_time);
      duration_loop_st = duration_loop_st + elapsed_time;
    }

    void reachedSymbolAction(std::vector<int> &intersection_result, TraversalNode &currentTNode, InvertedIndex &invi) {
      auto term = currentTNode.left_symbol;
      if (foundTerms.find(term) == foundTerms.end()) {
        foundTerms[term] = true;
        if(!invi.silence_results){
          intersection_result.push_back(currentTNode.left_symbol);
        }

      }
    }

  };

  inline std::vector<int> termListIntersection(uint termAIdx, uint termBIdx) {
    return treeTraversal<AndOperation>(termAIdx, termBIdx);
  }

  inline std::vector<int> termListUnion(uint termAIdx, uint termBIdx) {
    return treeTraversal<OrOperation>(termAIdx, termBIdx);
  }

  inline std::vector<int> termListIntersection(std::vector<uint> &query_terms) {
    return treeTraversal<AndOperation>(query_terms);
  }

  inline std::vector<int> termListUnion(std::vector<uint> &query_terms) {
    return treeTraversal<OrOperation>(query_terms);
  }

  template<class TraversalOperation>
  inline std::vector<int> treeTraversal(uint termAIdx, uint termBIdx) {
    std::vector<uint> query_terms{termAIdx, termBIdx};
    return treeTraversal<TraversalOperation>(query_terms);
  }



  template<class TraversalOperation, uint32_t bufsz=100>
  std::vector<int> treeTraversal(std::vector<uint> &query_terms) {
    auto pre_st = std::chrono::steady_clock::now();

    std::vector<int> output;


    auto root = wtHandler->getRoot();
    const uint left_symbol_root = 0u;

    const auto greaterPowerOf2 = (uint)std::ceil(std::log2(alphabet_sz));

    const uint right_symbol_root = (1u << (greaterPowerOf2)) - 1;

    const auto max_depth = greaterPowerOf2 + 2;

    std::array<TraversalNode, bufsz> prealloc_nodes;

    for(auto &tn_ptr : prealloc_nodes){
      tn_ptr.left_indexes.resize(query_terms.size());
      tn_ptr.right_indexes.resize(query_terms.size());
    }


    auto current_idx_tn = 0;
    auto &to_use_tn = prealloc_nodes[current_idx_tn];
    to_use_tn.idx = current_idx_tn;


    auto counter = 0;
    for(const auto& term : query_terms){
      auto[it, ft] = getTermInterval(term);
      to_use_tn.left_indexes[counter] = it;
      to_use_tn.right_indexes[counter] = ft;
      counter++;
    }


    CircularQueue<uint, bufsz> traversalStack;
    CircularQueue<uint, bufsz> available_indexes;
    for(auto i = 1ul; i < max_depth; i++){
      available_indexes.push(i);
    }

    to_use_tn.left_symbol = left_symbol_root;
    to_use_tn.right_symbol = right_symbol_root;
    to_use_tn.wt_node = root;

    traversalStack.push(current_idx_tn);


    TraversalOperation traversalOperation;


    while (!traversalStack.empty()) {
      auto currentTNode_ptr = traversalStack.top();
      auto &currentTNode = prealloc_nodes[currentTNode_ptr];
      traversalStack.pop();
      available_indexes.push(currentTNode.idx);


      if (traversalOperation.failCondition(currentTNode)) {
        continue;
      }

      if (reachedSymbol(currentTNode)) {
        traversalOperation.reachedSymbolAction(output, currentTNode, *this);
        continue;
      }

      if (wtHandler->isLeaf(currentTNode.wt_node)) {
        continue;
      }

      auto lc_idx = available_indexes.front(); available_indexes.pop_front();
      auto rc_idx = available_indexes.front(); available_indexes.pop_front();


      uint m = (currentTNode.left_symbol + currentTNode.right_symbol) >> 1u;

      auto &lc = prealloc_nodes[lc_idx];
      auto &rc = prealloc_nodes[rc_idx];


      auto &left_indexes_lc = lc.left_indexes;
      auto &right_indexes_lc = lc.right_indexes;
      auto &left_indexes_rc = rc.left_indexes;
      auto &right_indexes_rc = rc.right_indexes;

      traversalOperation.setTermVariables(*wtHandler, currentTNode, left_indexes_lc, right_indexes_lc,
              left_indexes_rc, right_indexes_rc);

      auto[left_child, right_child] = wtHandler->getChildren(currentTNode.wt_node);


      lc.left_symbol = currentTNode.left_symbol;
      lc.right_symbol = m;
      lc.wt_node = std::move(left_child);
      lc.idx = lc_idx;

      rc.left_symbol = m + 1;
      rc.right_symbol = currentTNode.right_symbol;
      rc.wt_node = std::move(right_child);
      rc.idx = rc_idx;


      traversalStack.push(rc_idx);
      traversalStack.push(lc_idx);

    }

    return output;
  }
};


#endif //NONPOSINVIDX_INVERTEDINDEX_HPP
