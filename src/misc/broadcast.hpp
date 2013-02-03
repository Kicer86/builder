#ifndef BROADCAST_HPP
#define BROADCAST_HPP

#include <QObject>

class ReleaseInfo;

class Broadcast : public QObject
{
        Q_OBJECT

        explicit Broadcast(QObject *parent = 0);

    public:
        static Broadcast* instance();

    public slots:
        void releaseSelected(ReleaseInfo *);

    signals:
        void releaseSelectedSignal(ReleaseInfo *);

};

#endif // BROADCAST_HPP
