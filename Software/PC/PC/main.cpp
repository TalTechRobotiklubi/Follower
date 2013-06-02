#include "followerui.h"
#include "Follower.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Follower *follower = new Follower();
    FollowerUi w(follower);
    w.show();
    return a.exec();
}