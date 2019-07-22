//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_WTHGOG_H
#define NONPOSINVIDX_WTHGOG_H

#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "BVHGog.h"


class WTHGog {
  BVHGog<> terms_separator;
  sdsl::wt_blcd<> wt;
public:
  explicit WTHGog(const std::string& sequenceInput) : terms_separator(sequenceInput.size()){
    
  }
};


#endif //NONPOSINVIDX_WTHGOG_H
