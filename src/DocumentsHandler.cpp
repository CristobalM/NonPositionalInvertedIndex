//
// Created by Cristobal Miranda, 2019
//

#include <cassert>
#include "DocumentsHandler.h"

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
  for (auto &document :  documents) {
    document->clean();
  }
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
