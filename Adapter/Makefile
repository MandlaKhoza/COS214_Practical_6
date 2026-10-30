# Build and run the Adapter unit tests:   make test_adapter
CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Isrc -Iinclude -Ithird_party

ADAPTER_SRC = src/LegacyCreditBureau.cpp src/PoliceRecordsDB.cpp src/HomeAffairsService.cpp \
              src/ExamBoardAPI.cpp src/FraudScreeningEngine.cpp \
              src/CreditCheckAdapter.cpp src/CriminalRecordAdapter.cpp \
              src/IdentityVerificationAdapter.cpp src/QualificationAdapter.cpp src/FraudCheckAdapter.cpp

# TEMPORARY: replace with the real Document.cpp once the Builder owner pushes it
DOCUMENT_SRC = tests/support/DocumentStub.cpp

test_adapter: $(ADAPTER_SRC) $(DOCUMENT_SRC) tests/test_adapter.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@
	./$@

clean:
	rm -f test_adapter test_adapter.exe
