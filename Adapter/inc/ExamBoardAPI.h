/**
 * @file ExamBoardAPI.h
 * @brief Adaptee: stub of the external exam board results service.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef EXAMBOARDAPI_H
#define EXAMBOARDAPI_H

#include <string>
#include <map>
#include "ExamRecord.h"
using namespace std;

/**
 * @class ExamBoardAPI
 * @brief Adaptee: a stand-in for the exam board's results service.
 *
 * Its interface (a candidate number in, a subject → mark map out) does not match
 * TIVIDY's ExternalCheck, which is why QualificationAdapter exists.
 *
 * @note This is a documented stub; no real service is contacted.
 * @see QualificationAdapter, ExamRecord
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
class ExamBoardAPI {
public:
    /**
     * @brief The service's own interface: returns a candidate's marks.
     * @param[in] candidateNo The exam candidate number.
     * @return subject → mark; empty if the candidate is unknown.
     */
    std::map<std::string, int> getResults(const std::string& candidateNo) const;

    /**
     * @brief Stores a candidate's record, replacing any existing record with the same number.
     * @param[in] record The record to store.
     * @post findRecord(record.candidateNo) returns this record.
     */
    void addRecord(const ExamRecord& record);

    /**
     * @brief Looks up a candidate's full record.
     * @param[in] candidateNo The exam candidate number.
     * @return A pointer to the record (owned by the service).
     * @retval nullptr No record for this candidate.
     */
    const ExamRecord* findRecord(const std::string& candidateNo) const;
    
private:
    std::map<std::string, ExamRecord> records;
};

#endif
