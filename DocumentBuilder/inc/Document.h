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
  Document() = default;
  void setDocument(std::map<std::string, std::string> common,
                   std::map<std::string, double> comparable);
  std::map<std::string, std::string> getCommon();
  std::map<std::string, double> getComparable();
  bool getField(std::string fieldName, std::string& value) const;
  bool getNumber(std::string fieldName, double& value) const;
};

#endif // DOCUMENT_H
