#include "AccountRegistry.h"

/**
 * @brief Adds an account to the registry.
 *
 * Identifies Accounts by account number
 * @param account A Pointer to the account being added.
 */
void AccountRegistry::add(Account* account){

    if(account == NULL){
        return;
    }
    std::string accountNumber = account->getAccountNumber();
    accounts[accountNumber] = std::unique_ptr<Account>(account);
}


/**
 * @brief Finds an account using its account number.
 *
 * Searches the registry for an account with the specified account number.
 *A contant iterator is used to loop through the map becasue the function is constant 
 *
 * @param accountNumber The account number of the account being searched for.
 * @return A pointer to the account if found, or NULL if it does not exist.
 */
Account* AccountRegistry::find(const std::string& accountNumber) const{

    std::map<std::string, std::unique_ptr<Account>>::const_iterator it;

    it = accounts.find(accountNumber); // FINDS AN ACCOUNTNUMBER(returns an iterator)

    if(it == accounts.end()){
        return NULL;
    }
    return it->second.get();
}


/**
 * @brief Finds all accounts belonging to a particular owner.
 *
 * Iterates through the registry and collects pointers to accounts
 * whose owner ID matches the passed in owner ID.
 *
 * @param ownerId The ID of the account owner.
 * @return A vector containing pointers to all matching accounts.
 */
std::vector<Account*> AccountRegistry::findByOwner(const std::string& ownerId) const{

    std::vector<Account*> ownerAccounts;

    std::map<std::string, std::unique_ptr<Account>>::const_iterator it;

    for(it = accounts.begin(); it != accounts.end(); it++){
        if(it->second->getOwnerId() == ownerId){
            ownerAccounts.push_back(it->second.get());
        }
    }

    return ownerAccounts;
}