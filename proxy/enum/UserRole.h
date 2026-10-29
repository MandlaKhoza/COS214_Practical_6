#ifndef USERROLE_H
#define USERROLE_H

/**
 * @brief The different types of user roles
 */
enum class UserRole
{
    /// @brief Personnel, can view account summary
    Staff,

    /// @brief user that uses the workflow, full access to own account
    Client,

    /// @brief can view full accounts with notes
    Auditor
};

#endif