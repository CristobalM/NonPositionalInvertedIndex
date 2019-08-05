//
// Created by Cristobal Miranda, 2019
//

#include <cassert>
#include <fstream>
#include <regex>
#include <string>


#include <algorithm>

#include <functional>

#include "SafeQueue.hpp"

#include "DocumentsHandler.hpp"

DocumentsHandler::DocumentsHandler(bool clean_on_scan, const std::string &word_regex_str) :
clean_on_scan(clean_on_scan), last_assigned_int(0), word_regex(word_regex_str) {}

void DocumentsHandler::initDocumentsFromDirectory(const std::string &directory_path) {
  auto dir_iterator = fs::directory_iterator(directory_path);
  for (auto &doc_fname : dir_iterator) {
    documents.push_back(std::make_unique<DocumentInfo>(doc_fname.path()));
    documentsNames.push_back(doc_fname.path());
  }
}


const unsigned long MAX_THREADS = 10;


void DocumentsHandler::processDocument(DocumentInfo *document_ptr, int document_index) {
  auto &document = *document_ptr;
  document.readDocument(word_regex);
  auto lg = std::lock_guard(wtd_mutex);
  for (auto &word_freqs_pair : document.getWordsFreqPairs()) {
    auto &word = word_freqs_pair.first;
    auto freq = word_freqs_pair.second;

    if (!wordExists(word)) {
      words_map[word] = ++last_assigned_int;
    }
    auto word_idx = words_map[word];
    words_to_docs_map[word_idx].emplace_back(document_index, freq);
  }
  if (clean_on_scan) {
    document.clean();
  }
}


using JobsQueue = SafeQueue<std::function<void()>>;

std::mutex consumer_mt;

void consumerJob(JobsQueue *jobs_queue_ptr){
  auto &jobs_queue = *jobs_queue_ptr;
  while(!jobs_queue.empty()){
    auto ul = std::unique_lock(consumer_mt);
    auto current_job = std::move(jobs_queue.front());
    jobs_queue.pop_front();
    ul.unlock();
    current_job();
  }
}

void DocumentsHandler::scanWords() {
  auto thread_count = std::min<unsigned long>(documents.size(), MAX_THREADS);
  std::vector<std::thread> docs_threads;
  JobsQueue processing_queue;

  for (unsigned long i = 0; i < documents.size(); i++) {
    auto document_ptr = documents[i].get();

    processing_queue.emplace_back(std::bind(&DocumentsHandler::processDocument, this, document_ptr, (int)i));
  }

  std::cout << "Starting " << thread_count << " threads to read the files\n";

  for(auto i = 0ul; i < thread_count; i++){
    docs_threads.emplace_back(consumerJob, &processing_queue);
  }

  for(auto &doc_thread: docs_threads){
    doc_thread.join();
  }

  std::cout << "Finished reading files\nCleaning unnecessary data" << std::endl;

  if(clean_on_scan){
    cleanData();
  }
  std::cout << "Finished cleaning unnecessary data" << std::endl;
}


void DocumentsHandler::cleanData() {
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

  for(auto doc_idx = 0ul; doc_idx < documentsNames.size(); doc_idx++){
    file_docs << documentsNames[doc_idx] << "\n";
  }
}

std::unique_ptr<DocumentsHandler> DocumentsHandler::load(const std::string &fpath) {
  auto out_uptr = std::make_unique<DocumentsHandler>();
  auto &out = *out_uptr;

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
  }
  file_words.close();

  std::ifstream file_docs(fpath + "_docs");
  while(std::getline(file_docs, input_line)){
    std::vector<std::string> input_matches(std::sregex_token_iterator(input_line.begin(), input_line.end(), word_regex_load),
                                           std::sregex_token_iterator());
    assert(input_matches.size() == 1);

    auto doc = input_matches[0];
    out.documentsNames.push_back(doc);
  }


  return out_uptr;
}

void DocumentsHandler::clearWordsToDocMap() {
  words_to_docs_map.clear();
}
