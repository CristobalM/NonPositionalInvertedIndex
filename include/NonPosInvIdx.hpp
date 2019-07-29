//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_NONPOSINVIDX_HPP
#define NONPOSINVIDX_NONPOSINVIDX_HPP

#include "InvertedIndex.hpp"
#include "DocumentsHandler.hpp"
#include "WTHGog.hpp"
#include "BVHGog.hpp"

using NonPosInvIdx = InvertedIndex<DocumentsHandler, WTHGog<>, BVHGog<>>;


#endif //NONPOSINVIDX_NONPOSINVIDX_HPP
