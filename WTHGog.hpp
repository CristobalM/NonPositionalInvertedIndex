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

template<uint8_t ivSZ = _DEFAULT_IV_SZ, typename AlphabetType = uint32_t>
class WTHGog {
  using WTType = sdsl::wt_int<>;
  WTType wt;

  static sdsl::int_vector<> viToIv(std::vector<AlphabetType> &vi) {
    sdsl::int_vector<> iv(vi.size());
    for (AlphabetType i = 0; i < vi.size(); i++) {
      iv[i] = vi[i];
    }
    return iv;
  }

public:
  explicit WTHGog(std::vector<AlphabetType> &inputWT) {
    auto iv = viToIv(inputWT);
    sdsl::construct_im(wt, iv);

    debugWTPrint(inputWT);
  }

  inline uint access(uint i) {
    return wt[i - 1];
  }

  inline uint rank(uint i, AlphabetType term_idx) {
    if(i == 0){
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


};


#endif //NONPOSINVIDX_WTHGOG_HPP
