#include "Session.h"
#include "Connection.h"

Session::Session(Connection *conn, const SessionToken &st) :
    conn_(conn),
    st_(st)
{ }

bool Session::send(char ch) const
{
    return conn_->send(st_, ch);
}

bool Session::send(const std::string &str) const
{
    return conn_->send(st_, str);
}

bool Session::endSession() const
{
    return conn_->endSession(st_);
}

bool Session::isValid() const
{
    return conn_->isValid(st_);
}
