//#ifndef globals
//#define globals

#include "server.hpp"
#include "client.hpp"
#include <unordered_map>
#include <string>

using namespace std;

class Server;
//
extern Server serv;
extern unordered_map<string,Client*> storageClients;
extern bool infinity;
//

//#endif
