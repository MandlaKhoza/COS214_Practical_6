#ifndef ACCOUNTCREATIONOBSERVER_H
#define ACCOUNTCREATIONOBSERVER_H

#include <memory>
#include "WorkflowObserver.h"

class AccountFactory;
class AccountRegistry;

/**
 * @brief Concrete Observer that creates an account when a workflow is approved.
 *
 * Must be attached before NotificationObserver so the account exists
 * before the applicant is notified.
 */
class AccountCreationObserver : public WorkflowObserver {
public:
    /**
     * @brief Constructor.
     * @param factory The factory used to create the account.
     * @param registry The registry the new account is stored in.
     */
    AccountCreationObserver(std::shared_ptr<AccountFactory> factory, std::shared_ptr<AccountRegistry> registry);

    /**
     * @brief Creates and stores an account if the workflow was approved.
     * @param wi The workflow instance that finished.
     * @param out How the workflow finished.
     */
    void onWorkflowFinished(WorkflowInstance& wi, Outcome out) override;

private:
    std::shared_ptr<AccountFactory> factory;   ///< Creates the account.
    std::shared_ptr<AccountRegistry> registry; ///< Stores the account.
};

#endif
