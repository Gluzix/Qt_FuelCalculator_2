#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    if(!w.return_if_db_connected())
    {
        w.show_message("Niestety, nie udalo sie polaczyc z baza danych!");
    }
    else
    {
        w.show_message("Aplikacja zawiera bazę danych która niekoniecznie musi zgadzać"
                       " się z faktycznym spalaniem Twojego auta, oraz pomimo naszej ciężkiej pracy, nie wszystkie"
                       " samochody są umieszone w naszej bazie!");
    }

    return a.exec();
}
