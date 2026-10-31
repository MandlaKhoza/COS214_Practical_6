#ifndef DIRECTOR_H
#define DIRECTOR_H

#include <string>
#include <vector>
#include <memory>

#include "Builder.h"
#include "Document.h"

class Director {
private:
  Builder *builder;
  std::vector<std::string> commonFields;
  std::vector<std::shared_ptr<Comparable>> comparableFields;

public:
  Director(Builder *builder, std::vector<std::string> commonFields, std::vector<std::shared_ptr<Comparable>> comparableFields);
  Document *construct();
};
#endif // DIRECTOR_H
