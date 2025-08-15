#pragma once
#include <iostream>

class Client {
  private:
    std::string _nickname;

  public:
    Client() = delete;
    Client& operator=(Client const& client) = delete;
    Client(std::string name);
    ~Client();
    bool operator<(Client const& client) const;

    std::string getName() const;
    void setName(std::string newName);
};