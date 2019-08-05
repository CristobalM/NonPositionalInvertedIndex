//
// Created by Cristobal Miranda, 2019
//

#include <memory>

#include "cxxopts/cxxopts.hpp"

#include "NonPosInvIdx.hpp"
#include "DocumentsHandler.hpp"

int main(int argc, char **argv){
  cxxopts::Options options("build_inverted_index", "Builds an inverted index and saves it to a file");

  struct SummaryOptions{
    std::string option, description;
    std::shared_ptr<cxxopts::Value> value;
  };

  static const std::string READ_FROM_DIR_OPT = "read_from_dir";
  static const std::string SAVE_LOC_OPT = "save_location";
  static const std::string INDEX_NAME_OPT = "index_name";

  std::vector<SummaryOptions> opts = {
          {READ_FROM_DIR_OPT, "Directory where all documents are located as files", cxxopts::value<std::string>()},
          {SAVE_LOC_OPT, "Save location of the index", cxxopts::value<std::string>()},
          {INDEX_NAME_OPT, "Index Name", cxxopts::value<std::string>()}
  };

  auto acc_opts = options.add_options();
  for(auto &opt : opts){
    acc_opts(opt.option, opt.description, opt.value);
  }

  std::unique_ptr<cxxopts::ParseResult> result_opt_ptr;

  try{
    result_opt_ptr = std::make_unique<cxxopts::ParseResult>(options.parse(argc, argv));
  }
  catch (const cxxopts::argument_incorrect_type &e){
    throw std::runtime_error("Invalid input: type error\nReason: " + std::string(e.what()));
  }

  auto &result_opt = *result_opt_ptr;


  if(result_opt[READ_FROM_DIR_OPT].count() < 1){
    throw std::runtime_error("read_from_dir option is missing. Please specify a directory from where to read the documents");
  }

  auto save_location =  result_opt[SAVE_LOC_OPT].count() >= 1 ? result_opt[SAVE_LOC_OPT].as<std::string>() : ".";

  for(auto &opt : opts){
    std::cout << opt.option << ": " << result_opt[opt.option].as<std::string>()  << std::endl;
  }

  auto documents_path = result_opt[READ_FROM_DIR_OPT].as<std::string>();
  auto index_name = result_opt[INDEX_NAME_OPT].as<std::string>();

  DocumentsHandler documentsHandler;

  documentsHandler.initDocumentsFromDirectory(documents_path);
  documentsHandler.scanWords();
  NonPosInvIdx invertedIndex(documentsHandler, index_name);
  invertedIndex.save(save_location);
  documentsHandler.save(save_location + index_name + "_documentsHandler");
}