//
// Created by Cristobal Miranda, 2019
//

#include <memory>
#include <regex>
#include <experimental/filesystem>
#include <sstream>
#include <chrono>

#include "cxxopts/cxxopts.hpp"

#include "NonPosInvIdx.hpp"
#include "DocumentsHandler.hpp"

using v_str = std::vector<std::string>;

class QueryOperation{
protected:
  v_str *query_terms;
  bool with_indexes;
public:
  explicit QueryOperation(v_str *query_terms, bool with_indexes) :
  query_terms(query_terms), with_indexes(with_indexes){}
  virtual void runOperation(DocumentsHandler &d_handler, NonPosInvIdx &i_idx) = 0;
  virtual std::string getResult() = 0;
  template <class T>
  static std::string joinListResult(T &list_result){
    std::stringstream ss;
    for(const auto &r : list_result){
      ss << r << "\n";
    }
    return ss.str();
  }
};

class AndQueryOperation : public QueryOperation{
  using QueryOperation::query_terms;
  v_str result;
  std::vector<int> result_idxs;
public:
  explicit AndQueryOperation(v_str *query_terms, bool with_indexes) :
  QueryOperation(query_terms, with_indexes){}

  void runOperation(DocumentsHandler &d_handler, NonPosInvIdx &i_idx) override {
    if(with_indexes){
      result_idxs = i_idx.termListIntersection(d_handler, *query_terms);
    }
    else{
      result = i_idx.termListIntersectionByDocNames(d_handler, *query_terms);
    }
  }

  std::string getResult() override {
    return with_indexes ? joinListResult(result_idxs) : joinListResult(result);
  }
};

class OrQueryOperation : public QueryOperation{
  using QueryOperation::query_terms;
  v_str result;
  std::vector<int> result_idxs;

public:
  explicit OrQueryOperation(v_str *query_terms, bool with_indexes) :
  QueryOperation(query_terms, with_indexes){}

  void runOperation(DocumentsHandler &d_handler, NonPosInvIdx &i_idx) override {
    if(with_indexes){
      result_idxs = i_idx.termListUnion(d_handler, *query_terms);
    }
    else{
      result = i_idx.termListUnionByDocNames(d_handler, *query_terms);
    }
  }

  std::string getResult() override {
    return with_indexes ? joinListResult(result_idxs) : joinListResult(result);
  }
};

class DFQueryOperation : public QueryOperation{
  using QueryOperation::query_terms;
  uint result;
public:
  explicit DFQueryOperation(v_str *query_terms) : QueryOperation(query_terms, false), result(0){}

  void runOperation(DocumentsHandler &d_handler, NonPosInvIdx &i_idx) override {
    assert(query_terms->size() == 1);
    result = i_idx.documentFrequency(d_handler, (*query_terms)[0]);
  }

  std::string getResult() override {
    return std::to_string(result);
  }

};

class IthDocQueryOperation : public QueryOperation{
  using QueryOperation::query_terms;
  std::string result;
public:
  explicit IthDocQueryOperation(v_str *query_terms) : QueryOperation(query_terms, false){}

  void runOperation(DocumentsHandler &d_handler, NonPosInvIdx &i_idx) override {
    assert(query_terms->size() == 2);
    result = i_idx.ithDocName(d_handler, (*query_terms)[0], std::stoi((*query_terms)[1]));
  }

  std::string getResult() override {
    return result;
  }

};

int main(int argc, char **argv) {
  cxxopts::Options options("query_index", "Queries an index");

  struct SummaryOptions {
    std::string option, description;
    std::shared_ptr<cxxopts::Value> value;
  };

  static const std::string INDEX_PATH = "index_path";
  static const std::string QUERY_INPUT_TYPE = "query_input_type";
  static const std::string FILE_PATH = "queries_file_path";
  static const std::string SILENCE_RESULTS = "silence_results";
  static const std::string INDEX_RESULTS = "index_results";

  std::vector<SummaryOptions> opts = {
          {INDEX_PATH,       "Path where the index is stored", cxxopts::value<std::string>()},
          {QUERY_INPUT_TYPE, "Input can be passed by command line or file (default=command_line, file)",
                                                               cxxopts::value<std::string>()->default_value("file")},
          {FILE_PATH,        "File path with queries",         cxxopts::value<std::string>()}
  };

  auto acc_opts = options.add_options();
  for (auto &opt : opts) {
    acc_opts(opt.option, opt.description, opt.value);
  }

  acc_opts(SILENCE_RESULTS, "If this is active, the results will not be shown");
  acc_opts(INDEX_RESULTS, "If this is active, the results will be shown as indexes of documents");

  std::unique_ptr<cxxopts::ParseResult> result_opt_ptr;

  try {
    result_opt_ptr = std::make_unique<cxxopts::ParseResult>(options.parse(argc, argv));
  }
  catch (const cxxopts::argument_incorrect_type &e) {
    throw std::runtime_error("Invalid input: type error\nReason: " + std::string(e.what()));
  }

  auto &result_opt = *result_opt_ptr;


  if (result_opt[INDEX_PATH].count() < 1) {
    throw std::runtime_error(INDEX_PATH + " option is missing. Please specify the file path where the index is stored");
  }

  auto index_path = result_opt[INDEX_PATH].as<std::string>();


  auto query_input_type = result_opt[QUERY_INPUT_TYPE].as<std::string>();

  if (result_opt[FILE_PATH].count() < 1) {
    throw std::runtime_error(FILE_PATH + " option is missing.");
  }

  auto query_input = result_opt[FILE_PATH].as<std::string>();


  auto silence_results = result_opt[SILENCE_RESULTS].as<bool>();
  auto index_results = result_opt[INDEX_RESULTS].as<bool>();

  std::regex word_regex("([^\\s]+)");

  std::vector<std::unique_ptr<v_str>> queries;
  std::vector<std::unique_ptr<QueryOperation>> queries_ops;

  if (query_input_type == "file") {
    std::ifstream input_file(query_input);

    std::string line;

    while (std::getline(input_file, line)) {
      std::smatch sm;
      bool first = true;

      auto query_terms = std::make_unique<v_str>();

      std::string query_op_s;
      for (auto r_it = std::sregex_iterator(line.begin(), line.end(), word_regex);
           r_it != std::sregex_iterator(); r_it++) {
        auto match = *r_it;
        auto word = match.str(0);

        if (first) {
          query_op_s = word;
          first = false;
        } else {
          query_terms->push_back(word);
        }
      }

      std::unique_ptr<QueryOperation> qop;
      if(query_op_s == "AND"){
        qop = std::make_unique<AndQueryOperation>(query_terms.get(), index_results);
      }
      else if(query_op_s == "OR"){
        qop = std::make_unique<OrQueryOperation>(query_terms.get(), index_results);
      }
      else if(query_op_s == "DOCUMENT_FREQUENCY"){
        qop = std::make_unique<DFQueryOperation>(query_terms.get());
      }
      else if(query_op_s == "ITH_DOCUMENT"){
        qop = std::make_unique<IthDocQueryOperation>(query_terms.get());
      }
      else{
        throw std::runtime_error("Unrecognized operation: '" + query_op_s + "'");
      }

      queries.push_back(std::move(query_terms));
      queries_ops.push_back(std::move(qop));
    }

    input_file.close();

  } else {
    throw std::runtime_error("Query input type " + query_input_type + " invalid");
  }

  fs::path path_index(index_path);
  auto index_base_path = path_index.parent_path();
  auto index_filename = path_index.filename();

  auto loadedDocHandler_ptr = DocumentsHandler::load(index_path + "_documentsHandler");
  auto &loadedDocHandler = *loadedDocHandler_ptr;

  auto loadedIdx = NonPosInvIdx::load(index_base_path, index_filename);
  if(silence_results){
    loadedIdx.silenceResults();
  }

  v_str result;

  auto start_time = std::chrono::steady_clock::now();
  for(const auto &qop : queries_ops){
    qop->runOperation(loadedDocHandler, loadedIdx);
    if(!silence_results){
        result.push_back(qop->getResult());
    }
  }
  auto end_time = std::chrono::steady_clock::now();
  auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

  std::cout << "Finished running queries. Elapsed time: " << elapsed_time << "ms" << std::endl;

  if(!silence_results){
    auto counter = 0;
    for(const auto &r : result){
      std::cout << "#" << ++counter << " Result:\n" << r << "\n" << std::endl;
    }

  }

  return 0;
}