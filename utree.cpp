/**
 * CMSC 341 - Spring 2021
 * Project 2 - Binary Trees
 * UserTree.h
 * Implementation for the UTree class.
 */

#include "utree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
UTree::~UTree() {
  clear();
}

/**
 * Sources a .csv file to populate Account objects and insert them into the UTree.
 * @param infile path to .csv file containing database of accounts
 * @param append true to append to an existing tree structure or false to clear before importing
 */ 
void UTree::loadData(string infile, bool append) {
    std::ifstream instream(infile);
    string line;
    char delim = ',';
    const int numFields = 5;
    string fields[numFields];

    /* Check to make sure the file was opened */
    if(!instream.is_open()) {
        std::cerr << __FUNCTION__ << ": File " << infile << " could not be opened or located" << endl;
        exit(-1);
    }

    /* Should we append or clear? */
    if(!append) this->clear();

    /* Read in the data from the .csv file and insert into the UTree */
    while(std::getline(instream, line)) {
        std::stringstream buffer(line);

        /* Quick check to make sure each line is formatted correctly */
        int delimCount = 0;
        for(unsigned int c = 0; c < buffer.str().length(); c++) if(buffer.str()[c] == delim) delimCount++;
        if(delimCount != numFields - 1) {
            throw std::invalid_argument("Malformed input file detected - ensure each line contains 5 fields deliminated by a ','");
        }

        /* Populate the account attributes - 
         * Each line always has 5 sections of data */
        for(int i = 0; i < numFields; i++) {
            std::getline(buffer, line, delim);
            fields[i] = line;
        }
        Account newAcct = Account(fields[0], std::stoi(fields[1]), std::stoi(fields[2]), fields[3], fields[4]);
        this->insert(newAcct);
    }
}

/**
 * Dynamically allocates a new UNode in the tree and passes insertion into DTree. 
 * Should also update heights and detect imbalances in the traversal path after
 * an insertion.
 * @param newAcct Account object to be inserted into the corresponding DTree
 * @return true if the account was inserted, false otherwise
 */
bool UTree::insert(Account newAcct) {
  UNode* temp = retrieve(newAcct.getUsername());
  
  if(temp != nullptr){
    if(temp->_dtree->insert(newAcct))
      return true;
    else
      return false;
  }
  
    _root = insert(newAcct, _root);

    return true;
}

/**
 * Removes a user with a matching username and discriminator.
 * @param username username to match
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool UTree::removeUser(string username, int disc, DNode*& removed) {

}

/**
 * Retrieves a set of users within a UNode.
 * @param username username to match
 * @return UNode with a matching username, nullptr otherwise
 */
UNode* UTree::retrieve(string username) {
  return retrieve(username, _root);
}

/**
 * Retrieves the specified Account within a DNode.
 * @param username username to match
 * @param disc discriminator to match
 * @return DNode with a matching username and discriminator, nullptr otherwise
 */
DNode* UTree::retrieveUser(string username, int disc) {
 UNode* temp =  retrieve(username);
 if(temp == nullptr)
   return nullptr;
 
 DNode* tempDisc = temp->_dtree->retrieve(disc);
 return tempDisc;
}

/**
 * Returns the number of users with a specific username.
 * @param username username to match
 * @return number of users with the specified username
 */
int UTree::numUsers(string username) {
  UNode* temp = retrieve (username);
  if(temp == nullptr)
    return 0;

  return temp->getDTree()->getNumUsers();
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void UTree::clear() {
  clear(_root);
}

/**
 * Prints all accounts' details within every DTree.
 */
void UTree::printUsers() const {
  printUsers(_root);
}

/**
 * Dumps the UTree in the '()' notation.
 */
void UTree::dump(UNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getUsername() << ":" << node->getHeight() << ":" << node->getDTree()->getNumUsers();
    dump(node->_right);
    cout << ")";
}

/**
 * Updates the height of the specified node.
 * @param node UNode object in which the height will be updated
 */
void UTree::updateHeight(UNode* node) {
  if(node == nullptr)
    return;

  int lHeight = (node->_left == nullptr ? -1 : node->_left->_height);
  int rHeight = (node->_right == nullptr ? -1 : node->_right->_height);
  node->_height = 1 + (lHeight > rHeight ? lHeight : rHeight);
}

/**
 * Checks for an imbalance, defined by AVL rules, at the specified node.
 * @param node UNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
int UTree::checkImbalance(UNode* node) {
  if(node == nullptr)
    return 0;

  if(node->_left->_height > node->_right->_height)
    return (node->_left->_height - node->_right->_height);
  else
    return (node->_right->_height - node->_left->_height);
}

//----------------
/**
 * Begins and manages the rebalance procedure for an AVL tree (pass by reference).
 * @param node UNode object where an imbalance occurred
 */
void UTree::rebalance(UNode*& node) {

// -- OR --
}

/**
 * Begins and manages the rebalance procedure for an AVL tree (returns a pointer).
 * @param node UNode object where an imbalance occurred
 * @return UNode object replacing the unbalanced node's position in the tree
 */
//UTree* UTree::rebalance(UNode* node) {

//}
//----------------

UNode* UTree::retrieve(string username, UNode*& node){
  if(node == nullptr)
    return nullptr;

  if(node->getUsername() == username)
      return node;
  else{
    if(node->getUsername() > username)
      return retrieve(username, node->_left);
    else{
      if(node->getUsername() < username)
        return retrieve(username, node->_right);
    }
  }
}

void UTree::clear(UNode* node){
  if(node == nullptr)
    return;
  else{
    clear(node->_left);
    clear(node->_right);
    delete node;
  }
}

void UTree::printUsers(UNode* node) const {
  if(node == nullptr)
    return;

  printUsers(node->_left);
  node->_dtree->printAccounts();
  printUsers(node->_right);
}

UNode* UTree::insert(Account newAcct, UNode*& node){
  if(node == nullptr){
    UNode* node = new UNode();
    if(node->getDTree()->insert(newAcct))
      return node;
    else
      return nullptr;
  }
  else{
    if(newAcct.getUsername() < node->getUsername()){
        node->_left = insert(newAcct, node->_left);
        updateHeight(node);
        //if(checkImbalance(node) > 1)
	//rebalance(node);
         return node;
      }
    else{
      if(newAcct.getUsername() > node->getUsername()){
          node->_right = insert(newAcct, node->_right);
          updateHeight(node);
          //if(checkImbalance(node) > 1)
	  //rebalance(node);
          return node;
      }
    }
  }
  
  return nullptr;
}
