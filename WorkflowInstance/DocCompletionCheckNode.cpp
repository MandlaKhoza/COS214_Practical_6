#include "DocCompletionCheckNode.h"

DocCompletionCheckNode::DocCompletionCheckNode(std::string name) : WorkflowNode(name){}

NodeResult DocCompletionCheckNode::process(WorkflowInstance& wi){
  std::map<std::string,std::string> commonFields = wi.getDocument()->getCommon();
  std::map<std::string, double> comparableFields = wi.getDocument()->getComparable();

  for (const auto& [key, value] : commonFields){
    if(value.empty()){
      NodeResult node;
      node.name = "Document not completed ";
      node.result = HandleResult::FAILED; // I don't know what the difference between failed and rejected is
      return node;
    }

  }

  for (const auto& [key, value] : comparableFields){
    if(value.empty()){
      NodeResult node;
      node.name = "Document not completed";
      node.result = HandleResult::FAILED;

      return node;
    }

  }
  NodeResult node;
  node.name = "Document is properly filled";
  node.result = HandleResult::PASSED;
  return node;
}

std::unique_ptr<WorkflowNode> DocCompletionCheckNode::clone(){
    std::string name = this->WorkflowNode::getName();

    return std::unique_ptr<WorkflowNode>(new DocCompletionCheckNode(name));
}
