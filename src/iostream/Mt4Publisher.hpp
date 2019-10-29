#ifndef MT4PUBLISHER_H
#define MT4PUBLISHER_H

#include "common/Observer.hpp"
#include "common/Types.hpp"

class Mt4Publisher : public AsyncThreadProcess<request_command>
{
private:
public:
    virtual ~Mt4Publisher() = default;
    virtual void AsyncUpdate(RequestCommandPtr reqCmdPtr) = 0;
};

typedef std::shared_ptr<Mt4Publisher> Mt4PublisherPtr;

#endif // MT4PUBLISHER_H
