#include "AccountFactory.h"


/**
 * @brief Constructor of  AccountFactories.
 *
 * Initialises the account number prefix and sets the counter to zero. 
 * It stores the field name used to retrieve the account owner's ID.
 *
 * @param prefix The prefix used when generating account numbers.
 * @param ownerField The document field name used for retrieving the account owners ID.
 */
AccountFactory::AccountFactory(std::string prefix, std::string ownerField)
: prefix(prefix), counter(0), ownerField(ownerField){}


/**
 * @brief Deallocates the AccountFactory.
 */
AccountFactory::~AccountFactory(){}


/** 
* @brief Creates an account by delegating to the CreateAccount() function that is declared
*        in concrete Factories
*        
* @param w the workflowInstance which we are creating an account for
 * @return A unique pointer to the created Account.
*/
std::unique_ptr<Account> AccountFactory::create(WorkflowInstance& w){
    return createAccount(w);
}


/**
 * @brief Generates the next account number.
 *
 * Increments the account counter and converts the new counter value into a string.
 * @return The next account number as a string.
 */
std::string AccountFactory::nextAccountNumber(){
    counter++;
    std::string nextAccountNumber = std::to_string(counter);
    return nextAccountNumber;

}


/**
 * @brief Gets the account owner's ID from a workflow's document.
 * Uses ownerField to identify which document field contains the owner's ID.
 *
 * @param w The workflow instance whose document contains the owner's ID.
 * @return The owner's ID if the field is found or an empty string if it is missing or not filled      
 */
std::string AccountFactory::getOwnerId(const WorkflowInstance& w) const{
    std::string id;

    const Document& d = w.getDocument();
    bool found = d.getField(ownerField, id);
    if(found == false){
        return "";
    }
    return id;
}
