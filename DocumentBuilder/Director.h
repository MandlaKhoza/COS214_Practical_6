#ifndef DIRECTOR_H
#define DIRECTOR_H

#include <string>
#include <vector>

class Director {
private:
  DocumentBuilder *builder;
  vector<string> commonFields;
  vector<Comparable *> comparableFields;

public:
  Director(DocumentBuilder *builder, vector<string> commonFields,
           vector<Comparable *> comparableFields);
  Document *construct();
};
#endif // DIRECTOR_H
