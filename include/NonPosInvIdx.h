//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_NONPOSINVIDX_H
#define NONPOSINVIDX_NONPOSINVIDX_H

#include "InvertedIndex.hpp"
#include "DocumentsHandler.h"
#include "WTHGog.hpp"
#include "BVHGog.hpp"

using NonPosInvIdx = InvertedIndex<DocumentsHandler, WTHGog<>, BVHGog<>>;


#endif //NONPOSINVIDX_NONPOSINVIDX_H
