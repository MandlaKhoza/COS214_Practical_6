#ifndef BUILDER_H
#define BUILDER_H

#include <string>
#include <vector>
#include <memory>

#include "Document.h"
#include "Comparable.h"

class Builder {
public:
  virtual void populateTUI(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields) = 0;
  virtual void createMapComponents(std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields) = 0;
  virtual void setMapValues() = 0;
  virtual Document *getDocument() = 0;
};

#endif // BUILDER_H
