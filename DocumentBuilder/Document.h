#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>
#include <vector>

class Document {
private:
  std::map<std::string, std::string> common;
  std::map<std::string, double> comparable;

public:
  Document();
  void setDocument(std::map<std::string, std::string> common,
                   std::map<std::string, double> comparable);
  std::map<std::string, std::string> getCommon();
  std::map<std::string, double> getComparable();
};

#endif // DOCUMENT_H
