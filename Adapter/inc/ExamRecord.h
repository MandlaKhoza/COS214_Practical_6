/**
 * @file ExamRecord.h
 * @brief One candidate's entry in the simulated exam board.
 * @author Hafani Ramagoma
 * @date 2026-10-30
 */
#ifndef EXAMRECORD_H
#define EXAMRECORD_H

#include <map>
#include <string>

/**
 * @brief One candidate's results in the simulated exam board (ExamBoardAPI).
 * @see ExamBoardAPI
 */
struct ExamRecord {
    std::string candidateNo;
    std::string fullName;
    std::map<std::string, int> results;  ///< subject → mark
};

#endif
