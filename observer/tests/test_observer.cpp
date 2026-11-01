#include <gtest/gtest.h>
#include "WorkflowSubject.h"
#include <string>
#include <vector>

using namespace std;

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

    TestObserver(string n) {
        name = n;
        received = Outcome::Cancelled;
    }

    void onWorkflowFinished(WorkflowInstance& wi, Outcome out) {
        received = out;
        calls.push_back(name);
    }
};

TEST(ObserverTest, ObserversAreCalledInAttachOrder) {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;

    subject.attach(make_shared<TestObserver>("first"));
    subject.attach(make_shared<TestObserver>("second"));
    subject.finish(&wi, Outcome::Approved);

      ASSERT_EQ(calls.size(), 2u);
    EXPECT_EQ(calls[0], "first");
    EXPECT_EQ(calls[1], "second");
}

TEST(ObserverTest, ObserverGetsTheRightOutcome) {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;
      shared_ptr<TestObserver> observer = make_shared<TestObserver>("a");

    subject.attach(observer);
    subject.finish(&wi, Outcome::Rejected);

    EXPECT_EQ(observer->received, Outcome::Rejected);
}

TEST(ObserverTest, DetachedObserverIsNotCalled) {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;
    shared_ptr<TestObserver> observer = make_shared<TestObserver>("a");

    subject.attach(observer);
       subject.detach(observer);
    subject.finish(&wi, Outcome::Approved);

    EXPECT_TRUE(calls.empty());
}

TEST(ObserverTest, NullObserverIsNotAttached) {
    calls.clear();
    TestSubject subject;
    WorkflowInstance wi;
      subject.attach(nullptr);
     subject.attach(make_shared<TestObserver>("a"));
    subject.finish(&wi, Outcome::Approved);

    EXPECT_EQ(calls.size(), 1u);
}

TEST(ObserverTest, NothingHappensIfInstanceIsNull) {
    calls.clear();
    TestSubject subject;

       subject.attach(make_shared<TestObserver>("a"));
    subject.finish(nullptr, Outcome::Approved);

 EXPECT_TRUE(calls.empty());
}
