#include <iostream>
#include <boost/asio.hpp>
#include <vector>
#include "server.hpp"
#include "client.hpp"
#include "globals.hpp"
//#include "mysql.hpp"
//#include "mysql.hpp"
using namespace std;
using namespace boost::asio;

bool infinity = true;
Server serv;

int main() {
    //
    storageClients["Marin"] = new Client(serv.io);
    storageClients["Marin"]->password = "admin";
    //
    infinity = true;


    serv.startUpdateDataToAuthClients();
    while (infinity)  {
        serv.checkConnectedWithNewClient();
        serv.checkAndSetAuthClients();

        
        
        
        
        
        
        
        
        
        cout << "size unAuthClients: " << serv.unAuthClients.size() << "       size authClients: " << serv.authClients.size() << endl;
        cout <<"poll" <<endl;

        serv.io.poll();
        serv.io.restart();

        sleep(5);
    }
}



void checkConnection(boost::system::error_code ec) {
    if(ec) {
        cout << ec.message() << "-\n";
    }
    else {
        
        Client* connectedClient = serv.newClient;
        cout << connectedClient->rozetka->remote_endpoint().address().to_string() << ':' << connectedClient->rozetka->remote_endpoint().port() << endl;
    }
    
}

void updateNewDataToClients(const boost::system::error_code& ec) {
    if (ec){
        if (ec) {
            std::cout << "Ошибка отмены: " << ec.message() << "\n";
        }
        else {
            std::cout << "Таймер завершился.\n";
        }
    }
    else {
        for (int i = 0; i < serv.authClients.size(); ++i) {
            Client* ptrClient = serv.authClients[i];
            if (!(ptrClient->isLWing)) {
                async_write(*(ptrClient->rozetka),buffer(ptrClient->dataBuffer),[ptrClient](const boost::system::error_code& ec, size_t bytes_transferred) {
                    if (ec) {
                        cout << ec.message();
                        closeSocket(ptrClient);
                    }
                    else {
                        ptrClient->isLWing = false;
                        cout << "TheData will send to: " + ptrClient->getNamePass() << endl;
                    }
                    
                });
            }
        }
        serv.timer.expires_after(std::chrono::seconds(5)); // Новый интервал
        serv.timer.async_wait(&updateNewDataToClients);
    }
}

void closeSocket(Client* ptrClient) {
    ptrClient->rozetka->close();
    if (ptrClient->name == "unAuth") {
        auto it = find(serv.unAuthClients.begin(),serv.unAuthClients.end(),ptrClient);
        if (serv.unAuthClients.end() != it)
        {
            serv.unAuthClients.erase(it);
        }
        else {
            serv.authClients.erase(find(serv.authClients.begin(),serv.authClients.end(),ptrClient));
        }
    }
    
    cout << "Socket is Closed"<<endl;

}
