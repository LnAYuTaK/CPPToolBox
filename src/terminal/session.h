#pragma once 
#include <string>
//服务器会话
class Session {
  public:
  
    Session(){

    }
    bool send(char ch) const{
      return false ;
    }
    bool send(const std::string &str) const
    {
     return false ;
    }
    bool endSession() const
    {
       return false ;
    }
    bool isValid() const{
        return false ;
    }
  private:
};

