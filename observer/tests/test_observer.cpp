#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "WorkflowSubject.h"
#include <string>
#include <vector>

using namespace std;
//5 Tests
class WorkflowInstance {};

vector<string> calls;

class TestSubject : public WorkflowSubject {
public:
    void finish(WorkflowInstance* wi, Outcome out) {
          notify(wi, out);
    }
};

class TestObserver : public WorkflowObserver {
public:
      string name;
    Outcome received;

    TestObserver(string n){
        name = n;
         received = Outcome::Cancelled;
    }

    void onWorkflowFinished(WorkflowInstance& wi, Outcome out) {
        received = out;
         calls.push_back(name);
    }
};

TEST_CASE("Observers are called in the order in which they were attached"){
    calls.clear();
      TestSubject subject;
    WorkflowInstance wi;

       subject.attach(make_shared<TestObserver>("first"));
    subject.attach(make_shared<TestObserver>("second"));
    subject.finish(&wi, Outcome::Approved);
    CHECK(calls.size() == 2);
    CHECK(calls[0] == "first");
    CHECK(calls[1] == "second");
}

TEST_CASE("Observer produce the right output") {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;
     shared_ptr<TestObserver> observer = make_shared<TestObserver>("a");

     subject.attach(observer);
    subject.finish(&wi, Outcome::Rejected);
    CHECK(observer->received == Outcome::Rejected);
}

TEST_CASE("Detached observer is not called") {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;
    shared_ptr<TestObserver> observer = make_shared<TestObserver>("a");

    subject.attach(observer);
    subject.detach(observer);
    subject.finish(&wi, Outcome::Approved);

    CHECK(calls.size() == 0);
}

TEST_CASE("Null observer is not attached") {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;

    subject.attach(nullptr);
    subject.attach(make_shared<TestObserver>("a"));
    subject.finish(&wi, Outcome::Approved);

    CHECK(calls.size() == 1);
}

TEST_CASE("Nothing happens if the instance is null"){
    calls.clear();
    TestSubject subject;
    subject.attach(make_shared<TestObserver>("a"));
    subject.finish(nullptr, Outcome::Approved);

    CHECK(calls.size() == 0);
}
