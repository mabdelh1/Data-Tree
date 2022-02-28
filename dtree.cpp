/**
 * CMSC 341 - Spring 2021
 * Project 2 - Binary Trees
 * DiscriminatorTree.cpp
 * Implementation for the DTree class.
 */

#include "dtree.h"

/**
 * Destructor, deletes all dynamic memory.
 */
DTree::~DTree() {

  clear(); //Deallocate memory

  _root = nullptr;  
}

/**
 * Overloaded assignment operator, makes a deep copy of a DTree.
 * @param rhs Source DTree to copy
 * @return Deep copy of rhs
 */
DTree& DTree::operator=(const DTree& rhs) {

  if(this != &rhs) { //Guard against self asaignament
    clear(); //deallocate memory before proceeding

    if(rhs._root != nullptr)
      createCopy(_root, rhs._root); //call copy creator function
    else
      _root = nullptr;
    
  }

  return *this;
}

/**
 * Dynamically allocates a new DNode in the tree. 
 * Should also update heights and detect imbalances in the traversal path
 * an insertion.
 * @param newAcct Account object to be contained within the new DNode
 * @return true if the account was inserted, false otherwise
 */
bool DTree::insert(Account newAcct) {
  if(retrieve(newAcct.getDiscriminator()) != nullptr) //Make sure the data does not already exist
    return false;
  
  _root = insert(newAcct, _root); //Insert the data into the tree
  
  return true;
}

/**
 * Removes the specified DNode from the tree.
 * @param disc discriminator to match
 * @param removed DNode object to hold removed account
 * @return true if an account was removed, false otherwise
 */
bool DTree::remove(int disc, DNode*& removed) {
  if(retrieve(disc) == nullptr)  //make sure the data exists in the tree before attempting to remove it
    return false;
  
  DNode* temp = retrieve(disc); //Retrieve the data to be removed
  
  removed = remover(disc, _root); //call remover function to remove data
  removed = temp; //set removed to the data removed

  return true;
}

/**
 * Retrieves the specified Account within a DNode.
 * @param disc discriminator int to search for
 * @return DNode with a matching discriminator, nullptr otherwise
 */
DNode* DTree::retrieve(int disc) {
  return retrieve(disc, _root); //call retrieve function 
}

/**
 * Helper for the destructor to clear dynamic memory.
 */
void DTree::clear() {  
  clear(_root); 
}

/**
 * Prints all accounts' details within the DTree.
 */
void DTree::printAccounts() const {
  printAccounts(_root);
}

/**
 * Dump the DTree in the '()' notation.
 */
void DTree::dump(DNode* node) const {
    if(node == nullptr) return;
    cout << "(";
    dump(node->_left);
    cout << node->getAccount().getDiscriminator() << ":" << node->getSize() << ":" << node->getNumVacant();
    dump(node->_right);
    cout << ")";
}

/**
 * Returns the number of valid users in the tree.
 * @return number of non-vacant nodes
 */
int DTree::getNumUsers() const {
  return (_root->_size - _root->_numVacant); //return size of root minus vacant for number of users    
}

/**
 * Updates the size of a node based on the imedaite children's sizes
 * @param node DNode object in which the size will be updated
 */
void DTree::updateSize(DNode* node) {
  if(node == nullptr) //not size updating is needed for empty nodes
    return;
  
  int i = 1; //set i to 1 for the node itself since it has a value
  //add sizes of children if any
  if(node->_left != nullptr)
    i = i + (node->_left->_size); 
  if(node->_right != nullptr)
    i = i + (node->_right->_size);

  node->_size = i; //set size
}


/**
 * Updates the number of vacant nodes in a node's subtree based on the immediate children
 * @param node DNode object in which the number of vacant nodes in the subtree will be updated
 */
void DTree::updateNumVacant(DNode* node) {
  if(node == nullptr)
    return;

  int i = 0;
  if(node->_vacant == true) //add 1 if the node itself is vacant
    i = i + 1;

  //get num of vacant nodes of children (if any) anf add it to the parent's
  if(node->_left != nullptr)
    i = i + node->_left->_numVacant;
  if(node->_right != nullptr)
    i = i + node->_right->_numVacant;

  node->_numVacant = i;
}

/**
 * Checks for an imbalance, defined by 'Discord' rules, at the specified node.
 * @param checkImbalance DNode object to inspect for an imbalance
 * @return (can change) returns true if an imbalance occured, false otherwise
 */
bool DTree::checkImbalance(DNode* node) {
  if(node == nullptr) //if noes itself is empty no need for cheching for imbalance
    return false;

  bool imbalance = false;     
  double difference = 0.0;
  double differenceRate = 0.0;
  double leftSize = 0.0;
  double rightSize = 0.0;

  //get sizes of left and right if any
  if(node->_left != nullptr)
    leftSize = static_cast<double>(node->_left->_size);
  if(node->_right != nullptr)
    rightSize = static_cast<double>(node->_right->_size);

  if(leftSize == rightSize) //if both size are equal there is no imbalance
    return false;

  //if and else statement to get the difference in sizes and calculating the difference rate
  if(leftSize > rightSize){
    difference = leftSize - rightSize;
    if(difference > 0.0 && leftSize > 0.0)
      differenceRate = difference / leftSize;
    else
      differenceRate = 0.0;
  }
  else{
    difference = rightSize - leftSize;
    if(difference > 0.0 && rightSize > 0.0)
      differenceRate = difference / rightSize;
    else
      differenceRate = 0.0;
}

  //evaluatwe criarteria of imbalance and set impalance to true if crateria is met
  if((rightSize >= 4.0 || leftSize >= 4.0) && differenceRate >= 0.5){
    imbalance = true;
    resetArrayMigration();  
  }

  return imbalance;
}

//----------------
/**
 * Begins and manages the rebalancing process for a 'Discrd' tree (pass by reference).
 * @param node DNode root of the subtree to balance
 */
void DTree::rebalance(DNode*& node) {
  if(node == nullptr) //no nooed for rebalancing if node is empty
    return;

  return;

  //create an array from data inside the bst tree
  int arraySize = (node->getSize()-node->_numVacant);
  Account allAccounts[arraySize];
  convertToArray(node, allAccounts);

  //dismantle the bst tree after extracting data from it
  clear(node);
  node = nullptr;

  //sort array from smallet to largest
  Account temp;
  for(int i = 0; i < arraySize; i++){
     for(int c = 0; c < arraySize; c++){
      if(allAccounts[i].getDiscriminator() < allAccounts[c].getDiscriminator()){
	temp = allAccounts[i];
	allAccounts[i] = allAccounts[c];
	allAccounts[c] = temp;
      }
    }
  }

  //add data back to bst
  node = arrayToBalancedBST(allAccounts, 0, arraySize, node, arraySize);
 
  return;
}

// -- OR --

/**
 * Begins and manages the rebalancing process for a 'Discrd' tree (returns a pointer).
 * @param node DNode root of the subtree to balance
 * @return DNode root of the balanced subtree
 */
//DNode* DTree::rebalance(DNode*& node) {

//}
//----------------

/**
 * Overloaded << operator for an Account to print out the account details
 * @param sout ostream object
 * @param acct Account objec to print
 * @return ostream object containing stream of account details
 */
ostream& operator<<(ostream& sout, const Account& acct) {
    sout << "Account name: " << acct.getUsername() << 
            "\n\tDiscriminator: " << acct.getDiscriminator() <<
            "\n\tNitro: " << acct.hasNitro() << 
            "\n\tBadge: " << acct.getBadge() << 
            "\n\tStatus: " << acct.getStatus();
    return sout;
}

void DTree::createCopy(DNode* node, DNode* copyNode){
    if(copyNode == nullptr) return;

    //go through the bst tree and create copies
    createCopy(node->_left, copyNode->_left);
    node = new DNode(copyNode->_account);
    node->_size = copyNode->_size;
    node->_numVacant = copyNode->_numVacant;
    node->_vacant = copyNode->_vacant;
    createCopy(node->_right, copyNode->_right);

}

DNode* DTree::insert(Account newAcct, DNode*& node){
  if(node != nullptr){ //if node is not empty, add data
    //check if data is less than the root
    if(newAcct.getDiscriminator() < node->_account.getDiscriminator()){
      //if data is node is vacant and if able to take data, insert data
      if(node->_vacant == true && vacantInsertEligibility(node, newAcct.getDiscriminator())){
	node->_account = newAcct;
	node->_vacant = false;
	updateNumVacant(node);
	return node;
       }
      else{
	//move left if data is less than the root
        node->_left = insert(newAcct, node->_left);
	updateSize(node);
	updateNumVacant(node);
	if(checkImbalance(node))
	  rebalance(node);
	 return node;
      }
    }
    else{
      //check if data is more than the root
      if(newAcct.getDiscriminator() > node->_account.getDiscriminator()){
	 //if data is node is vacant and if able to take data, insert data 
	if(node->_vacant == true && vacantInsertEligibility(node, newAcct.getDiscriminator())){
	  node->_account = newAcct;
	  node->_vacant = false;
	  updateNumVacant(node);
	  return node;
	}
	else{
	  //move right if data is more than the root
	  node->_right = insert(newAcct, node->_right);
	  updateSize(node);
	  updateNumVacant(node);
	  if(checkImbalance(node))
	    rebalance(node);
	  return node;
	}
      }
    }
  }
  else{
    //allocate memory and add data to it if an emoty node is found
    DNode* node = new DNode(newAcct);
    updateSize(node);
    return node;
  }
  return nullptr;
}

DNode* DTree::remover(int disc, DNode*& node){
  if(node == nullptr)
    return nullptr;

  //recursivly cal; function to get to the data to be removed
  node->_left = remover(disc, node->_left);
  node->_right = remover(disc, node->_right);

  if(node->_account.getDiscriminator() == disc &&
     node->_left == nullptr && node->_right == nullptr){
    node->_vacant = true;
    updateNumVacant(node);
    return node;
  }

  updateNumVacant(node);
  return node;
}

bool DTree::vacantInsertEligibility(DNode*& node, int AcctNumber){
  //Evaluate node against data to see if it is elibale to be added in place of a vacant node
  if(node->_account.getDiscriminator() == AcctNumber)
    return false;
  
  if(node->_right != nullptr && node->_left != nullptr){
    if((node->_right->_account.getDiscriminator() > AcctNumber) && (AcctNumber > node->_left->_account.getDiscriminator()))
      return true;
  }
  else{
    if(node->_right == nullptr && node->_left != nullptr){
      if(node->_left->_account.getDiscriminator() < AcctNumber)
	return true;
    }
    else{
      if(node->_left == nullptr && node->_right != nullptr){
      if(node->_right->_account.getDiscriminator() > AcctNumber)
        return true;
      }
      else
	return true;
    }
  }

  return false;

}
  
DNode* DTree::retrieve(int disc, DNode*& node){
  //call function recursivly until the data that is being looked for is found
  if(node == nullptr)
    return nullptr;

  if(node->_account.getDiscriminator() == disc){
    if(node->_vacant == true)
      return nullptr;
    else
      return node;
  }
  else{
    if(node->_account.getDiscriminator() > disc)
      return retrieve(disc, node->_left);
    else{
      if(node->_account.getDiscriminator() < disc)
	return retrieve(disc, node->_right);
    }
  }
  return nullptr;
}

void DTree::clear(DNode* node){
  if(node == nullptr)
    return;
  else{
    clear(node->_left);
    clear(node->_right);
    delete node;
  }
}

void DTree::printAccounts(DNode* node) const{
  if(node == nullptr || node->_vacant == true)
    return;
  printAccounts(node->_left);
  cout << node;
  printAccounts(node->_right);
}

void DTree::convertToArray(DNode*& node, Account allAccounts[]){
  if(node == nullptr)
    return;

  //only add data to array if it is not vacant
  if(node->_vacant == false){
    allAccounts[arrayMigration] = node->_account;
    arrayMigration = arrayMigration + 1;
  }
  
  if(node->_left != nullptr)
    convertToArray(node->_left, allAccounts);
  if(node->_right != nullptr)
    convertToArray(node->_right, allAccounts);

  return;
}

DNode* DTree::arrayToBalancedBST(Account allAccounts[], int start, int end, DNode*& node, int stagnent){
  //add data to bst tree
  if(start > end)
    return nullptr;
  
  if(end == stagnent && start == stagnent)
    return nullptr;
  
  int mid = (start + end)/2;

  node = insert(allAccounts[mid], node); 
  
  node->_left = arrayToBalancedBST(allAccounts, start, mid-1, node->_left, stagnent);
  node->_right = arrayToBalancedBST(allAccounts, mid+1, end, node->_right, stagnent);
  
  return node;

}
