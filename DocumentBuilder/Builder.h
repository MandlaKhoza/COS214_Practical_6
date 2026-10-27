#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <vector>

class Builder {
public:
  void populateTUI(std::vector<std::string> commonFields,
                   std::vector<Comparable *> comparableFields);

  void createMapComponents(std::vector<std::string> commonFields,
                           std::vector<Comparable *> comparableFields);
  void setMapValues();
  Document *getDocument();
};

#endif // BUILDER_H
