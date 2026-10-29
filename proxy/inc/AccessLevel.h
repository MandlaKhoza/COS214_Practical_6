#ifndef ACCESSLEVEL_H
#define ACCESSLEVEL_H

/**
 * @brief Defines a user's access level to the account
 */
enum class AccessLevel
{
    /// @brief User does not have access
    Denied,

    /// @brief user is allowed to view only a summary of the account (staff)
    Summary,

    /// @brief user is allowed to view the full account (client who owns the account)
    Full,

    /// @brief User is allowed to view the full account, with additional notes (Auditor)
    FullWithNotes
};

#endif