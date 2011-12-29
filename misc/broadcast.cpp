
#include "broadcast.hpp"

Broadcast::Broadcast(QObject *p) :
    QObject(p)
{
}


Broadcast* Broadcast::instance()
{
    static Broadcast _instance;

    return &_instance;
}

void Broadcast::releaseSelected(ReleaseInfo *releaseInfo)
{
    //broadcast it

    emit releaseSelectedSignal(releaseInfo);
}
