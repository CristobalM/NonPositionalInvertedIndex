//
// Created by Cristobal Miranda, 2019
//


#include <memory>
#include <regex>
#include <experimental/filesystem>

#include "cxxopts/cxxopts.hpp"

#include "NonPosInvIdx.hpp"
#include "DocumentsHandler.hpp"

int main(int argc, char **argv){
  cxxopts::Options options("query_index", "Queries an index");

  struct SummaryOptions{
    std::string option, description;
    std::shared_ptr<cxxopts::Value> value;
  };

  static const std::string INDEX_PATH = "index_path";
  static const std::string QUERY_TYPE = "query_type";
  static const std::string QUERY_INPUT_TYPE = "query_input_type";
  static const std::string QUERY_INPUT = "query_input";

  std::vector<SummaryOptions> opts = {
          {INDEX_PATH, "Path where the index is stored", cxxopts::value<std::string>()},
          {QUERY_TYPE, "What query is being called (AND, OR)", cxxopts::value<std::string>()},
          {QUERY_INPUT_TYPE, "Input can be passed by command line or file (default=command_line, file)",
           cxxopts::value<std::string>()->default_value("command_line")},
          {QUERY_INPUT, "Query input", cxxopts::value<std::string>()}
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


  if(result_opt[INDEX_PATH].count() < 1){
    throw std::runtime_error(INDEX_PATH + " option is missing. Please specify the file path where the index is stored");
  }

  auto index_path = result_opt[INDEX_PATH].as<std::string>();


  if(result_opt[QUERY_TYPE].count() < 1){
    throw std::runtime_error(QUERY_TYPE + " option is missing. Should be AND or OR");
  }

  auto query_type = result_opt[QUERY_TYPE].as<std::string>();

  auto query_input_type = result_opt[QUERY_INPUT_TYPE].as<std::string>();

  if(result_opt[QUERY_INPUT].count() < 1){
    throw std::runtime_error(QUERY_INPUT + " option is missing.");
  }

  auto query_input = result_opt[QUERY_INPUT].as<std::string>();

  std::vector<std::string> query_terms;

  if(query_input_type == "command_line"){
    std::regex word_regex("([^\\s]+)");
    std::smatch sm;
    for (auto r_it = std::sregex_iterator(query_input.begin(), query_input.end(), word_regex);
    r_it != std::sregex_iterator();r_it++) {
      auto match = *r_it;
      auto word = match.str(0);
      query_terms.push_back(word);
    }
  }
  else if(query_input_type == "file"){
    throw std::runtime_error("Query input type " + query_input_type + " is not currently supported");
  }
  else{
    throw std::runtime_error("Query input type " + query_input_type + " invalid");
  }

  std::cout << "query_input: \n";
  for(const auto &q : query_terms){
    std::cout << q << std::endl;
  }


  fs::path path_index(index_path);
  auto index_base_path = path_index.parent_path();
  auto index_filename = path_index.filename();

  auto loadedDocHandlerP = DocumentsHandler::load(index_path + "_documentsHandler");
  auto &loadedDocHandler = *loadedDocHandlerP;

  auto loadedIdx = NonPosInvIdx::load(index_base_path, index_filename);

  std::vector<std::string> result;

  if(query_type == "AND"){
    result = loadedIdx.termListIntersectionByDocNames(loadedDocHandler, query_terms);
  }
  else if(query_type ==  "OR"){
    result = loadedIdx.termListUnionByDocNames(loadedDocHandler, query_terms);
  }
  else{
    throw std::runtime_error("Invalid operation " + query_type);
  }

  std::cout << "Result (" << result.size()  << ") :\n";

  for(const auto &r : result){
    std::cout << r << std::endl;
  }

  std::cout << std::endl;
}