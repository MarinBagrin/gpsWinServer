#ifndef server
#define server
#include <iostream>
#include <boost/asio.hpp>
#include <map>
#include <vector>
#include <string>

#include "globals.hpp"
#include "client.hpp"
using namespace std;
using namespace boost::asio;
void updateClients();
void sendDataToClients();
void checkConnection(boost::system::error_code ec);
void updateNewDataToClients(const boost::system::error_code& ec);
void closeSocket(Client* ptrClient);


class Server {
public:
    ///data
    io_context io;
    steady_timer timer;
    ip::tcp::acceptor acceptor;
    
    vector<Client*> authClients;
    vector<Client*> unAuthClients;
    Client* newClient;
    ///constructor initialize
    Server():io(),
    acceptor(io,ip::tcp::endpoint(boost::asio::ip::tcp::v4(),49500)),
    newClient(new Client(io)),timer(io) {
        acceptor.async_accept(*(newClient->rozetka),&checkConnection);
        
    }
    ///class functions
    void checkConnectedWithNewClient() {
        if (newClient->rozetka->is_open()) {
            unAuthClients.push_back(newClient);
            newClient = new Client(io);
            acceptor.async_accept(*(newClient->rozetka),&checkConnection);
            
            
        }
    }
    
    void checkAndSetAuthClients() {
        for (int i = 0; i < unAuthClients.size(); ++i) {
            Client* ptrClient = unAuthClients[i];
            if (!(ptrClient->isLWing)) {
                ptrClient->isLWing = true;
                
                //
                ptrClient->rozetka->async_read_some(buffer(ptrClient->authBuffer),[ptrClient,i](const boost::system::error_code& ec, size_t bytes_transferred){
                    ptrClient->isLWing = false;
                    if (ec) {
                        if (ec == boost::asio::error::eof) {
                            // Соединение закрыто удаленной стороной
                            std::cerr << "Connection closed gracefully by the peer." << std::endl;
                        } else if (ec == boost::asio::error::operation_aborted) {
                            // Операция была отменена
                            std::cerr << "Read operation was aborted." << std::endl;
                        } else {
                            // Обработка других типов ошибок
                            std::cerr << "Error during read: " << ec.message() << std::endl;
                        }
                        //serv.unAuthClients.erase(serv.unAuthClients.begin()+i);
                        closeSocket(ptrClient);
                        delete ptrClient;

                    }
                    else {
                        // Если ошибка нет, то выводим количество прочитанных байт и данные
                        std::cout << "Successfully read " << bytes_transferred << " bytes." << std::endl;
                        std::cout << "Received data: " << std::string(ptrClient->authBuffer, bytes_transferred) << std::endl;
                        vector<string> namePass(2,""); //name and password values, 0 for name nad 1 for pass
                        char* recieveBuffer = ptrClient->authBuffer;
                        int j = 0;
                        for (int i = 0; i < 2; i++) {
                            for (; j < 32 ; ++j ) {
                                if (recieveBuffer[j] == '/') {
                                    ++j;
                                    break;
                                }
                                namePass[i] += recieveBuffer[j];
                            }
                        }
                        if (storageClients.find(namePass[0]) != storageClients.end() && namePass[1] == storageClients[namePass[0]]->password )  {
                            storageClients[namePass[0]]->rozetka = ptrClient->rozetka;
                            serv.unAuthClients.erase(serv.unAuthClients.begin()+i); // утечка памяти
                            serv.authClients.push_back(storageClients[namePass[0]]);
                            cout << "setAuthClient: " << serv.authClients[serv.authClients.size()-1]->getNamePass(); //getStograge
                            
                        }
                        
                    }
                });
                
            }
        }
    }
    void startUpdateDataToAuthClients() {
        timer.expires_after(std::chrono::seconds(5));
        timer.async_wait(&updateNewDataToClients);
        
    }
    
};

extern Server serv;
unordered_map<string,Client*> storageClients;



#endif








