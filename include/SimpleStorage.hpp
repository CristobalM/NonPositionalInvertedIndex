//
// Created by Cristobal Miranda, 2019
//

#ifndef NONPOSINVIDX_STORAGE_HPP
#define NONPOSINVIDX_STORAGE_HPP

#include <fstream>
#include <vector>

class SimpleStorage{
public:
  static inline std::string join_path(const std::string &s_a, const std::string &s_b){
    return s_a + "/" + s_b;
  }

  template <typename VecType>
  static void save_vi(const std::string &index_path, const std::string &index_name, const std::string &vi_name,
          std::vector<VecType> &vec){
    if(vec.empty()){
      return;
    }

    auto full_path = join_path(index_path, index_name) + "_" + vi_name;
    std::ofstream out(full_path, std::ios::out | std::ios::binary);

    out.write(reinterpret_cast<char *>(&vec[0]), vec.size() * sizeof(VecType));
  }

  template <class VecType = int>
  static std::vector<VecType> load_vi(const std::string &index_path, const std::string &index_name, const std::string &vi_name){
    auto full_path = join_path(index_path, index_name) + "_" + vi_name;
    std::ifstream in(full_path, std::ios::in | std::ios::binary);
    in.seekg(0, std::ios::end);
    auto fsize = in.tellg();
    in.seekg(0,std::ios::beg);

    std::vector<VecType> output(fsize / sizeof(VecType));
    in.read(reinterpret_cast<char *>(output.data()), output.size() * sizeof(VecType));
    return output;
  }

  static void save_int(const std::string &index_path, const std::string &index_name, const std::string &int_name, int data){
    auto full_path = join_path(index_path, index_name) + "_" + int_name;
    std::ofstream out(full_path, std::ios::out | std::ios::binary);
    out.write(reinterpret_cast<char *>(&data), sizeof(int));
  }

  static int load_int(const std::string &index_path, const std::string &index_name, const std::string &int_name){
    auto full_path = join_path(index_path, index_name) + "_" + int_name;
    std::ifstream in(full_path, std::ios::in | std::ios::binary);
    int data;
    in.read(reinterpret_cast<char *>(&data), sizeof(int));
    return data;
  }


};

#endif //NONPOSINVIDX_STORAGE_HPP
