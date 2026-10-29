#include "MessageSender.h"
#include <iostream>

void MessageSender::send(const std::string& recipient, const std::string& subject, const std::string& body){
  std::cout << "To: " << recipient << std::endl;
    std::cout << "Subject: " << subject << std::endl;
    std::cout << body << std::endl;
}
