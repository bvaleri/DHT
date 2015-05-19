/**********************************
 * FILE NAME: MP2Node.cpp
 *
 * DESCRIPTION: MP2Node class definition
 **********************************/
#include "MP2Node.h"

/**
 * constructor
 */
MP2Node::MP2Node(Member *memberNode, Params *par, EmulNet * emulNet, Log * log, Address * address) {
	this->memberNode = memberNode;
	this->par = par;
	this->emulNet = emulNet;
	this->log = log;
	ht = new HashTable();
	this->memberNode->addr = *address;
}

/**
 * Destructor
 */
MP2Node::~MP2Node() {
	delete ht;
	delete memberNode;
}

/**
 * FUNCTION NAME: updateRing
 *
 * DESCRIPTION: This function does the following:
 * 				1) Gets the current membership list from the Membership Protocol (MP1Node)
 * 				   The membership list is returned as a vector of Nodes. See Node class in Node.h
 * 				2) Constructs the ring based on the membership list
 * 				3) Calls the Stabilization Protocol
 */
void MP2Node::updateRing() {
	/*
	 * Implement this. Parts of it are already implemented
	 */
	vector<Node> curMemList;
	bool change = false;

	/*
	 *  Step 1. Get the current membership list from Membership Protocol / MP1
	 */
	curMemList = getMembershipList();

	/*
	 * Step 2: Construct the ring
	 */
	// Sort the list based on the hashCode
	sort(curMemList.begin(), curMemList.end());
    ring = curMemList;

	/*
	 * Step 3: Run the stabilization protocol IF REQUIRED
	 */
	// Run stabilization protocol if the hash table size is greater than zero and if there has been a changed in the ring
}

/**
 * FUNCTION NAME: getMemberhipList
 *
 * DESCRIPTION: This function goes through the membership list from the Membership protocol/MP1 and
 * 				i) generates the hash code for each member
 * 				ii) populates the ring member in MP2Node class
 * 				It returns a vector of Nodes. Each element in the vector contain the following fields:
 * 				a) Address of the node
 * 				b) Hash code obtained by consistent hashing of the Address
 */
vector<Node> MP2Node::getMembershipList() {
	unsigned int i;
	vector<Node> curMemList;
	for ( i = 0 ; i < this->memberNode->memberList.size(); i++ ) {
		Address addressOfThisMember;
		int id = this->memberNode->memberList.at(i).getid();
		short port = this->memberNode->memberList.at(i).getport();
		memcpy(&addressOfThisMember.addr[0], &id, sizeof(int));
		memcpy(&addressOfThisMember.addr[4], &port, sizeof(short));
		curMemList.emplace_back(Node(addressOfThisMember));
	}
	return curMemList;
}

/**
 * FUNCTION NAME: hashFunction
 *
 * DESCRIPTION: This functions hashes the key and returns the position on the ring
 * 				HASH FUNCTION USED FOR CONSISTENT HASHING
 *
 * RETURNS:
 * size_t position on the ring
 */
size_t MP2Node::hashFunction(string key) {
	std::hash<string> hashFunc;
	size_t ret = hashFunc(key);
	return ret%RING_SIZE;
}

/**
 * FUNCTION NAME: clientCreate
 *
 * DESCRIPTION: client side CREATE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientCreate(string key, string value) {
    g_transID++;
	vector<Node> replicas = findNodes(key);
	Message msg(g_transID,memberNode->addr,CREATE,key,value,PRIMARY);
    quorum[g_transID] = vector<string>();
    outgoingMsgTimestamp[g_transID] = par->getcurrtime(); 
   	outgoingMsg.emplace(g_transID,msg); // attention!!! opertor[] would result in compile-error 
       
    Message msg_sec(g_transID,memberNode->addr,CREATE,key,value,SECONDARY);
	Message msg_ter(g_transID,memberNode->addr,CREATE,key,value,TERTIARY);
    sendMsg(msg,&replicas[0].nodeAddress);
    sendMsg(msg_sec,&replicas[1].nodeAddress);
    sendMsg(msg_ter,&replicas[2].nodeAddress);
}

/**
 * FUNCTION NAME: clientRead
 *
 * DESCRIPTION: client side READ API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientRead(string key){
	
     
     
}

/**
 * FUNCTION NAME: clientUpdate
 *
 * DESCRIPTION: client side UPDATE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientUpdate(string key, string value){
	/*
	 * Implement this
	 */
    g_transID++;
	vector<Node> replicas = findNodes(key);
	Message msg(g_transID,memberNode->addr,UPDATE,key,value,PRIMARY);
    quorum[g_transID] = vector<string>();
    outgoingMsgTimestamp[g_transID] = par->getcurrtime(); 
   	outgoingMsg.emplace(g_transID,msg); // attention!!! opertor[] would result in compile-error 
       
    Message msg_sec(g_transID,memberNode->addr,UPDATE,key,value,SECONDARY);
	Message msg_ter(g_transID,memberNode->addr,UPDATE,key,value,TERTIARY);
    sendMsg(msg,&replicas[0].nodeAddress);
    sendMsg(msg_sec,&replicas[1].nodeAddress);
    sendMsg(msg_ter,&replicas[2].nodeAddress);
}

/**
 * FUNCTION NAME: clientDelete
 *
 * DESCRIPTION: client side DELETE API
 * 				The function does the following:
 * 				1) Constructs the message
 * 				2) Finds the replicas of this key
 * 				3) Sends a message to the replica
 */
void MP2Node::clientDelete(string key){
	/*
	 * Implement this
	 */
}

/**
 * FUNCTION NAME: createKeyValue
 *
 * DESCRIPTION: Server side CREATE API
 * 			   	The function does the following:
 * 			   	1) Inserts key value into the local hash table
 * 			   	2) Return true or false based on success or failure
 */
bool MP2Node::createKeyValue(string key, string value, ReplicaType replica) {
	Entry entry(value,par->getcurrtime(),replica); 
    return ht->create(key,entry.convertToString());
}

/**
 * FUNCTION NAME: readKey
 *
 * DESCRIPTION: Server side READ API
 * 			    This function does the following:
 * 			    1) Read key from local hash table
 * 			    2) Return value
 */
string MP2Node::readKey(string key) {
	/*
	 * Implement this
	 */
	// Read key from local hash table and return value
}

/**
 * FUNCTION NAME: updateKeyValue
 *
 * DESCRIPTION: Server side UPDATE API
 * 				This function does the following:
 * 				1) Update the key to the new value in the local hash table
 * 				2) Return true or false based on success or failure
 */
bool MP2Node::updateKeyValue(string key, string value, ReplicaType replica) {
    Entry entry(value,par->getcurrtime(),replica); 
    return ht->update(key,entry.convertToString());
}

/**
 * FUNCTION NAME: deleteKey
 *
 * DESCRIPTION: Server side DELETE API
 * 				This function does the following:
 * 				1) Delete the key from the local hash table
 * 				2) Return true or false based on success or failure
 */
bool MP2Node::deletekey(string key) {
	/*
	 * Implement this
	 */
	// Delete the key from the local hash table
}

/**
 * FUNCTION NAME: checkMessages
 *
 * DESCRIPTION: This function is the message handler of this node.
 * 				This function does the following:
 * 				1) Pops messages from the queue
 * 				2) Handles the messages according to message types
 */
void MP2Node::checkMessages() {
	/*
	 * Implement this. Parts of it are already implemented
	 */
	char * data;
	int size;

	/*
	 * Declare your local variables here
	 */

	// dequeue all messages and handle them
	while ( !memberNode->mp2q.empty() ) {
		/*
		 * Pop a message from the queue
		 */
		data = (char *)memberNode->mp2q.front().elt;
		size = memberNode->mp2q.front().size;
		memberNode->mp2q.pop();

		string message(data, data + size);
		/*
		 * Handle the message types here
		 */
        handleMsg(message);
	}

    // loop through all keys in quorum and check for time-outs    
    for (auto it : outgoingMsgTimestamp) {
        if (par->getcurrtime() - it.second > 512) {
			// get type of mesg
            Message msg = outgoingMsg.at(it.first);
			if (static_cast<MessageType>(msg.type) == CREATE) {
				log->logCreateFail(&memberNode->addr,true,it.first,msg.key,msg.value);
            }else if (static_cast<MessageType>(msg.type) == READ) {
                
            }else if (static_cast<MessageType>(msg.type) == UPDATE) {
                
            }
            
        }
    }
    
}

// wrapper for all message types handling
void MP2Node::handleMsg(string message) {
    string originalMsg = message;
    int found = message.find("::");
    int transID = stoi(message.substr(0,found));
    message = message.substr(found + 2);
    
    found = message.find("::");
    string fromAddr = message.substr(0,found);
    message = message.substr(found + 2);
    
    found = message.find("::");
    MessageType msgType = static_cast<MessageType>(stoi(message.substr(0,found))); 
    message = message.substr(found + 2);

    if (msgType == CREATE || msgType == UPDATE) {
        dispatchCreateUpdateMsg(message,transID,fromAddr,msgType);
    }else if (msgType == DELETE) {
        
    }else if (msgType == READ) {
        
    }else if (msgType == REPLY) {
        handlesReply(originalMsg,message,transID);
    }else if (msgType == READREPLY) {
        
    }
}

// handles reply messages
void MP2Node::handlesReply(string originalMsg, string leftMsg,int transID) {
    quorum[transID].push_back(originalMsg);
    // if all replies have been received
    if (quorum[transID].size() == 3) {
        int vote = 0;
        
        for (int i = 0; i < quorum[transID].size(); i++) {
            if (stoi(leftMsg) == 1) {
                vote++;
            }
        }
        
        Message outgoingMessage = outgoingMsg.at(transID);
        if (vote > 2) {
            if (outgoingMessage.type == CREATE) {
                log->logCreateSuccess(&memberNode->addr,true,transID,outgoingMessage.key,outgoingMessage.value);
            }else if (outgoingMessage.type == UPDATE) {
                log->logUpdateSuccess(&memberNode->addr,true,transID,outgoingMessage.key,outgoingMessage.value);
            }else if (outgoingMessage.type == READ) {
                
            }else if (outgoingMessage.type == DELETE) {
                
            }
        }else {
            if (outgoingMessage.type == CREATE) {
                log->logCreateFail(&memberNode->addr,true,transID,outgoingMessage.key,outgoingMessage.value);
            }else if (outgoingMessage.type == UPDATE) {
                log->logUpdateFail(&memberNode->addr,true,transID,outgoingMessage.key,outgoingMessage.value);
            }else if (outgoingMessage.type == READ) {
                
            }else if (outgoingMessage.type == DELETE) {
                
            }
        }
        
        // close this transaction
        quorum.erase(transID);
        outgoingMsg.erase(transID);
        outgoingMsgTimestamp.erase(transID);
    }
}

// wrapper for message sending
void MP2Node::sendMsg(Message msg, Address *toAddr) {
    string msgStr = msg.toString();
    char* msgChar = (char*)malloc(msgStr.size() + 1);

    strcpy(msgChar,msgStr.c_str());
    emulNet->ENsend(&memberNode->addr,toAddr,msgChar,msgStr.size() + 1);
    
    free(msgChar);
}

// handles CREATE and UPDATE msg
void MP2Node::dispatchCreateUpdateMsg(string message, int transID, string masterAddrStr, MessageType msgType) {
    int found = message.find("::");
    string key = message.substr(0,found);
    message = message.substr(found + 2);
            
    found = message.find("::");
    string value = message.substr(0,found);
            
    ReplicaType replicaType = static_cast<ReplicaType>(stoi(message.substr(found + 2)));
    Address masterAddr = Address(masterAddrStr);
    
    // create/update the K/V pair on local hash table and send back a reply to master
    bool success;
    if (msgType == CREATE) {
        success = createKeyValue(key,value, replicaType);
        
        // logging
        if (success) {
            log->logCreateSuccess(&memberNode->addr,false,transID,key,value);
        }else {
            log->logCreateFail(&memberNode->addr,false,transID,key,value);
        }
    }else if (msgType == UPDATE) {
        success = updateKeyValue(key,value, replicaType);
        
        // logging
        if (success) {
            log->logUpdateSuccess(&memberNode->addr,false,transID,key,value);
        }else {
            log->logUpdateFail(&memberNode->addr,false,transID,key,value);
        }
    }
    Message reply(transID,memberNode->addr,REPLY,success);
    sendMsg(reply,&masterAddr);
}


/**
 * FUNCTION NAME: findNodes
 *
 * DESCRIPTION: Find the replicas of the given keyfunction
 * 				This function is responsible for finding the replicas of a key
 */
vector<Node> MP2Node::findNodes(string key) {
	size_t pos = hashFunction(key);
	vector<Node> addr_vec;
	if (ring.size() >= 3) {
		// if pos <= min || pos > max, the leader is the min
		if (pos <= ring.at(0).getHashCode() || pos > ring.at(ring.size()-1).getHashCode()) {
			addr_vec.emplace_back(ring.at(0));
			addr_vec.emplace_back(ring.at(1));
			addr_vec.emplace_back(ring.at(2));
		}
		else {
			// go through the ring until pos <= node
			for (int i=1; i<ring.size(); i++){
				Node addr = ring.at(i);
				if (pos <= addr.getHashCode()) {
					addr_vec.emplace_back(addr);
					addr_vec.emplace_back(ring.at((i+1)%ring.size()));
					addr_vec.emplace_back(ring.at((i+2)%ring.size()));
					break;
				}
			}
		}
	}
	return addr_vec;
}

/**
 * FUNCTION NAME: recvLoop
 *
 * DESCRIPTION: Receive messages from EmulNet and push into the queue (mp2q)
 */
bool MP2Node::recvLoop() {
    if ( memberNode->bFailed ) {
    	return false;
    }
    else {
    	return emulNet->ENrecv(&(memberNode->addr), this->enqueueWrapper, NULL, 1, &(memberNode->mp2q));
    }
}

/**
 * FUNCTION NAME: enqueueWrapper
 *
 * DESCRIPTION: Enqueue the message from Emulnet into the queue of MP2Node
 */
int MP2Node::enqueueWrapper(void *env, char *buff, int size) {
	Queue q;
	return q.enqueue((queue<q_elt> *)env, (void *)buff, size);
}
/**
 * FUNCTION NAME: stabilizationProtocol
 *
 * DESCRIPTION: This runs the stabilization protocol in case of Node joins and leaves
 * 				It ensures that there always 3 copies of all keys in the DHT at all times
 * 				The function does the following:
 *				1) Ensures that there are three "CORRECT" replicas of all the keys in spite of failures and joins
 *				Note:- "CORRECT" replicas implies that every key is replicated in its two neighboring nodes in the ring
 */
void MP2Node::stabilizationProtocol() {
	/*
	 * Implement this
	 */
}
