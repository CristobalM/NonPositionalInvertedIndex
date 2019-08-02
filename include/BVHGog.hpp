//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_BVHGOG_HPP
#define NONPOSINVIDX_BVHGOG_HPP

#include <memory>

#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support_v5.hpp>
#include <sdsl/select_support_mcl.hpp>

using uint = uint32_t ;

template<class BVGog>
class RankSupportConstantGog {
  sdsl::rank_support_v5<> rank_support;
public:
  explicit RankSupportConstantGog(BVGog *bvGog) : rank_support(bvGog) {}

  inline uint rank(uint i) {
    return rank_support.rank(i);
  }
};

template<class BVGog, uint8_t WhichVal>
class SelectSupportConstantGog {
  sdsl::select_support_mcl<WhichVal, 1> select_support;
public:
  explicit SelectSupportConstantGog(BVGog *bvGog) : select_support(bvGog) {
  }

  inline uint select(uint i) {
    return select_support.select(i);
  }
};

template<class any>
struct always_false {
  static constexpr bool value = false;
};


template<class BVGog, uint8_t WhichVal>
class NullSelect {
public:
  explicit NullSelect(BVGog *bvGog) {}

  inline uint select(uint i) {
    static_assert(always_false<BVGog>::value, "Calling select on NullSelect");
    return 0;
  }
};

template<class BVGog, uint8_t WhichVal>
class NullRank {
public:
  explicit NullRank(BVGog *bvGog) {}

  inline uint rank(uint i) {
    static_assert(always_false<BVGog>::value, "Calling rank on NullRank");
    return 0;
  }
};

/** Wrapper class for SDSL's bit vector with rank and select (0, 1) support **/
template<template<class> class RankSupport = RankSupportConstantGog,
        template<class, uint8_t> class Select0Support = SelectSupportConstantGog,
        template<class, uint8_t> class Select1Support = SelectSupportConstantGog>
class BVHGog {
  using GBV = sdsl::bit_vector;
  using Rk = RankSupport<GBV>;
  using Sel0 = Select0Support<GBV, 0>;
  using Sel1 = Select1Support<GBV, 1>;

  std::unique_ptr<Rk> rankSupport;
  std::unique_ptr<Sel0> select0Support;
  std::unique_ptr<Sel1> select1Support;

  uint bv_size;

  std::unique_ptr<GBV> bv;

  bool structures_built;
  uint nofzeros;
  uint nofones;

public:
  explicit BVHGog(unsigned long bv_size) :
          bv_size(bv_size), bv(std::make_unique<GBV>(bv_size)), structures_built(false), nofzeros(0), nofones(0) {}

  explicit BVHGog(std::unique_ptr<GBV> &&bv_) : bv_size(bv_->size()), bv(std::move(bv_)), structures_built(false), nofzeros(0), nofones(0) {
    buildStructures();
  }


  void bitset(uint i) {
    assert(i >= 1 && i <= bv_size);
    (*bv)[i - 1] = 1;
  }

  void bitclear(uint i) {
    assert(i >= 1 && i <= bv_size);
    (*bv)[i - 1] = 0;
  }

  void buildStructures() {
    rankSupport = std::make_unique<Rk>(bv.get());
    select0Support = std::make_unique<Sel0>(bv.get());
    select1Support = std::make_unique<Sel1>(bv.get());
    structures_built = true;
    nofones = rank(bv_size);
    nofzeros = bv_size - nofones;
    //std::cout << "Built bv:\n" << bv << std::endl;
  }

  inline uint rank(uint i) {
    if (!structures_built) {
      throw std::runtime_error("Rank structure was not built");
    }
    if(i == 0){
      return 0;
    }
    assert(i >= 1 && i <= bv_size);

    return rankSupport->rank(i);
  }

  inline uint rank_0(uint i) {
    return i - rank(i);
  }

  inline uint rank_1(uint i) {
    return rank(i);
  }

  inline uint select_0(uint i) {
    if (!structures_built) {
      throw std::runtime_error("Select 0 structure was not built");
    }
    assert(i >= 1 && i <= bv_size);
    if (i > nofzeros) {
      return bv_size + 1;
    }
    return select0Support->select(i) + 1;
  }

  inline uint select_1(uint i) {
    if (!structures_built) {
      throw std::runtime_error("Select 1 structure was not built");
    }
    assert(i >= 1 && i <= bv_size);
    if (i > nofones) {
      return bv_size + 1;
    }
    return select1Support->select(i) + 1;
  }

  inline uint pred(uint i){
    return select_1(rank(i));
  }
  inline uint succ(uint i){
    auto rk_v = rank(i-1) + 1;
    auto result =  select_1(rk_v);
    return result;
  }

  void debugDisplay() {
    std::cout << "Debug display of BVHGog: \n"
              << *bv << std::endl;
  }

  void save(const std::string &index_path, const std::string &index_name, const std::string &name){
    auto full_path = index_path + "/" + index_name + "_" + name;
    std::ofstream out(full_path, std::ios::out | std::ios::binary);
    bv->serialize(out);
  }

  static std::unique_ptr<BVHGog> load(const std::string &index_path, const std::string &index_name, const std::string &name){
    auto full_path = index_path + "/" + index_name + "_" + name;
    std::ifstream in(full_path, std::ios::in | std::ios::binary);
    auto stored_wt = std::make_unique<GBV>();
    stored_wt->load(in);
    return std::make_unique<BVHGog>(std::move(stored_wt));
  }

};


#endif //NONPOSINVIDX_BVHGOG_HPP
