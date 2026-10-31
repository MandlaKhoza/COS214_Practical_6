# TIVIDY Factory GoogleTest Makefile
# GoogleTest 1.17 requires C++17, so test executables use C++17.
# The separate check-cxx11 target checks production source compatibility
# using the project's required C++11 standard.

CXX = g++
TEST_CXXFLAGS = -std=c++17 -Wall -Werror -g
PROD_CXXFLAGS = -std=c++11 -Wall -Werror -g
CPPFLAGS = -Itests/stubs -Iinc
GTEST_LIBS = -lgtest -lgtest_main -pthread

GENERAL_TEST_SOURCES = \
	tests/FactoryTests.cpp \
	src/Account.cpp \
	src/AccountFactory.cpp \
	src/GeneralAccountFactory.cpp \
	src/StudentAccount.cpp \
	src/EmployeeAccount.cpp \
	src/SurveyReceipt.cpp \
	src/TimeStamp.cpp

FINANCE_TEST_SOURCES = \
	tests/FinanceFactoryTests.cpp \
	src/Account.cpp \
	src/AccountFactory.cpp \
	src/FinanceAccountFactory.cpp \
	src/LoanAccount.cpp \
	src/RefundRecord.cpp \
	src/TimeStamp.cpp

.PHONY: all test test-general test-finance check-cxx11 clean

# Run both Factory test suites.
all: test

test: test-general test-finance
	./tests/factory_tests
	./tests/finance_tests

# Build and run GeneralAccountFactory tests.
test-general: tests/factory_tests
	./tests/factory_tests

tests/factory_tests: $(GENERAL_TEST_SOURCES)
	$(CXX) $(TEST_CXXFLAGS) $(CPPFLAGS) $(GENERAL_TEST_SOURCES) $(GTEST_LIBS) -o $@

# Build and run FinanceAccountFactory tests.
test-finance: tests/finance_tests
	./tests/finance_tests

tests/finance_tests: $(FINANCE_TEST_SOURCES)
	$(CXX) $(TEST_CXXFLAGS) $(CPPFLAGS) $(FINANCE_TEST_SOURCES) $(GTEST_LIBS) -o $@

# Check production source files against the required C++11 standard.
# Test stubs provide the Document and WorkflowInstance interfaces.
check-cxx11:
	$(CXX) $(PROD_CXXFLAGS) $(CPPFLAGS) -fsyntax-only \
		src/Account.cpp \
		src/AccountFactory.cpp \
		src/GeneralAccountFactory.cpp \
		src/StudentAccount.cpp \
		src/EmployeeAccount.cpp \
		src/SurveyReceipt.cpp \
		src/FinanceAccountFactory.cpp \
		src/LoanAccount.cpp \
		src/RefundRecord.cpp \
		src/TimeStamp.cpp

# Remove only the two generated test executables.
clean:
	rm -f tests/factory_tests tests/finance_tests
