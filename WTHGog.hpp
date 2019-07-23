//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_WTHGOG_HPP
#define NONPOSINVIDX_WTHGOG_HPP

#include <vector>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>

#include <iostream> // debug

#include "BVHGog.hpp"


static const uint8_t _DEFAULT_IV_SZ = 32;

class WtIntGogWTreeRank : public sdsl::wt_int<> {
public:
  using sdsl::wt_int<>::wt_int;

  uint32_t tree_rank(uint i) {
    return m_tree_rank.rank(i);
  }

  //WtIntGogWTreeRank(): sdsl::wt_int<>(){}
};

template<uint8_t ivSZ = _DEFAULT_IV_SZ, typename AlphabetType = uint32_t>
class WTHGog {
  using WTType = WtIntGogWTreeRank;
  WTType wt;

  static sdsl::int_vector<> viToIv(std::vector<AlphabetType> &vi) {
    sdsl::int_vector<> iv(vi.size());
    for (AlphabetType i = 0; i < vi.size(); i++) {
      iv[i] = vi[i];
    }
    return iv;
  }

public:
  using WTNode = WTType::node_type;

  explicit WTHGog(std::vector<AlphabetType> &inputWT) {
    auto iv = viToIv(inputWT);
    sdsl::construct_im(wt, iv);

    debugWTPrint(inputWT);
  }

  inline uint access(uint i) {
    return wt[i - 1];
  }

  inline uint rank(uint i, AlphabetType term_idx) {
    if (i == 0) {
      return 0;
    }
    return wt.rank(i - 1, term_idx);
  }

  inline uint select(uint i, AlphabetType term_idx) {
    return wt.select(i, term_idx) + 1;
  }

  void debugWTPrint(std::vector<AlphabetType> &inputWT) {
    std::cout << " WT INPUT SEQUENCE" << std::endl;
    for (AlphabetType i = 0; i < inputWT.size(); i++) {
      std::cout << (uint) inputWT[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "DEBUG WT \n"
              << wt << std::endl;

  }

  WTType &getInternalStructure() {
    return wt;
  }

  inline uint innerBVRank(WTNode &wtNode, uint idx) {
    if (idx < 1 || idx > wtNode.size) {
      throw std::runtime_error("innerBVRank idx out of bounds: 1 <= " +
                               std::to_string(idx) + " <= " + std::to_string(wtNode.size));
    }
    auto base_rank = wtNode.offset > 0 ? wt.tree_rank(wtNode.offset) : 0;
    auto real_idx = idx + wtNode.offset;
    auto extra_rank = real_idx > 0 ? wt.tree_rank(real_idx) : 0;
    return extra_rank - base_rank;
  }

  inline uint innerBVRank_0(WTNode &wtNode, uint idx) {
    if(idx == 0){
      return 0;
    }
    if(idx > wtNode.size){
      idx = wtNode.size;
    }
    if (idx < 1 || idx > wtNode.size) {
      throw std::runtime_error("innerBVRank_0 idx out of bounds: 1 <= " +
                               std::to_string(idx) + " <= " + std::to_string(wtNode.size));
    }
    //auto real_idx = idx - 1 + wtNode.offset;
    return idx - innerBVRank(wtNode, idx);
  }

  WTNode getRoot() {
    return wt.root();
  }

  inline bool isLeaf(WTNode &wt_node) {
    return wt.is_leaf(wt_node);
  }

  std::pair<WTNode, WTNode> getChildren(WTNode &wt_node) {
    auto tmp_exp = wt.expand(wt_node);
    return {std::move(tmp_exp[0]), std::move(tmp_exp[1])};
  }


};


#endif //NONPOSINVIDX_WTHGOG_HPP
