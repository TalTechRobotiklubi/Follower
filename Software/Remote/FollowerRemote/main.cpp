#include "followerui.h"
#include "Follower.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Follower follower;
    FollowerUi w(&follower);
    w.show();
    return a.exec();
}
