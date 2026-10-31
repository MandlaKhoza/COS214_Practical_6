/**
 * @file CancelledState.cpp
 * @brief Implementation of CancelledState.
 * @details Final state: it overrides no events, so every event
 *          (including reset and cancel) throws std::logic_error.
 */
#include "CancelledState.h"

/**
 * @brief Gives the name of this state.
 * @return "Cancelled".
 * */
std::string CancelledState::getName() const { return "Cancelled"; }
