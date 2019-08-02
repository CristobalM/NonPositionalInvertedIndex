//
// Created by Cristobal Miranda, 2019
//

#include <cassert>
#include <fstream>
#include <regex>
#include <string>
#include "DocumentsHandler.hpp"

DocumentsHandler::DocumentsHandler(bool clean_on_scan, const std::string &word_regex_str) :
clean_on_scan(clean_on_scan), last_assigned_int(0), word_regex(word_regex_str) {}

void DocumentsHandler::initDocumentsFromDirectory(const std::string &directory_path) {
  auto dir_iterator = fs::directory_iterator(directory_path);
  for (auto &doc_fname : dir_iterator) {
    documents.push_back(std::make_unique<DocumentInfo>(doc_fname.path()));
    std::cout << "Creating DocumentInfo with path: " << doc_fname.path() << std::endl;
    documentsNames.push_back(doc_fname.path());
  }
}

void DocumentsHandler::scanWords() {
  for (unsigned long i = 0; i < documents.size(); i++) {
    auto &document = *documents[i];
    document.readDocument(word_regex);
    for (auto &word_freqs_pair : document.getWordsFreqPairs()) {
      auto &word = word_freqs_pair.first;
      auto freq = word_freqs_pair.second;
      if (!wordExists(word)) {
        words_map[word] = ++last_assigned_int;
        words_map_inv[last_assigned_int] = word;
      }

      auto word_idx = words_map[word];
      words_to_docs_map[word_idx].emplace_back(i, freq);
    }
    if (clean_on_scan) {
      document.clean();
    }
  }
}

void DocumentsHandler::debugPrintScannedWords() {
  std::cout << "DEBUG: Printing words_to_doc_map" << std::endl;
  std::stringstream ss;
  for (auto &map_element : words_to_docs_map) {
    std::stringstream subss;
    for (auto &doc_info : map_element.second) {
      subss << "(" << doc_info.first << ", " << doc_info.second << ") | ";
    }
    ss << "Word #" << map_element.first << " (" << words_map_inv[map_element.first] << "):\t" << subss.str() << "\n";
  }
  std::cout << ss.str() << std::endl;
}

void DocumentsHandler::cleanData() {
  /*
  for (auto &document :  documents) {
    document->clean();
  }
   */
  documents.clear();

}

std::vector<std::pair<int, int>> &DocumentsHandler::getWordDocs(int word_idx) {
  return words_to_docs_map[word_idx];
}

unsigned long DocumentsHandler::getUniqueWordsCount() {
  return last_assigned_int;
}

bool DocumentsHandler::wordExists(const std::string &word) {
  return words_map.find(word) != words_map.end();
}

int DocumentsHandler::getWordIdxByName(const std::string &word_name) {
  if (!wordExists(word_name)) {
    return -1;
  }
  return words_map[word_name];
}

uint DocumentsHandler::getUniqueDocsCount() {
  return documents.size();
}

std::string DocumentsHandler::getDocNameByIdx(int doc_idx) {
  assert(doc_idx >= 0 && (unsigned long)doc_idx < documentsNames.size());
  return documentsNames[doc_idx];
}

void DocumentsHandler::save(const std::string &fpath) {
  std::ofstream file_words(fpath + "_words");

  for(auto &word : words_map){
    file_words << word.first << " " << word.second << "\n";
  }

  file_words.close();

  std::ofstream file_docs(fpath + "_docs");

  for(auto doc_idx = 0; doc_idx < documentsNames.size(); doc_idx++){
    file_docs << documentsNames[doc_idx] << "\n";
  }
}

DocumentsHandler DocumentsHandler::load(const std::string &fpath) {
  DocumentsHandler out;

  std::ifstream file_words(fpath + "_words");

  std::string input_line;

  std::regex word_regex_load("([^\\s]+)");



  while(std::getline(file_words, input_line)){
    std::vector<std::string> input_matches(std::sregex_token_iterator(input_line.begin(), input_line.end(), word_regex_load),
                                   std::sregex_token_iterator());
    assert(input_matches.size() == 2);

    auto word_idx = (int)std::atoi(input_matches[1].c_str());
    auto word = input_matches[0];
    out.words_map[word] = word_idx;
    out.words_map_inv[word_idx] = word;
  }
  file_words.close();

  std::ifstream file_docs(fpath + "_docs");
  while(std::getline(file_docs, input_line)){
    std::vector<std::string> input_matches(std::sregex_token_iterator(input_line.begin(), input_line.end(), word_regex_load),
                                           std::sregex_token_iterator());
    assert(input_matches.size() == 1);

    //auto doc_idx = (int)std::atoi(input_matches[0].c_str());
    auto doc = input_matches[0];
    out.documentsNames.push_back(doc);
  }


  return out;
}
