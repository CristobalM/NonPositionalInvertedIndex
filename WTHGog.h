//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_WTHGOG_H
#define NONPOSINVIDX_WTHGOG_H

#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>

#include "BVHGog.h"


class WTHGog {
  sdsl::wt_blcd<> wt;
public:
  explicit WTHGog(const std::string& sequenceInput){

  }
};


#endif //NONPOSINVIDX_WTHGOG_H
