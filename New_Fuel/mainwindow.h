#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include <QMessageBox>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool return_if_db_connected(){return if_db_connected;}
    void show_message(QString);


private slots:
    void on_pushButton_clicked();
    void on_radioButton_clicked();
    void on_radioButton_2_clicked();
    void on_checkBox_2_clicked();
    void on_checkBox_clicked();
    void change_statements();
    void select_from_db(QString);
    void select_from_db_next(QString);
    void delete_old_objects();


    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

    void on_comboBox_4_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    bool if_db_connected;
    QMessageBox *mgbox;
    bool if_petrl_chs=false;
    bool from_db_or_manual=false;
    QSqlDatabase my_db;
    int selecting_counter;
    bool Petrol;
    QString Which_Petrol;
    QVector<QSqlQuery*> vctrQRY;
    QVector<QSqlQueryModel*> vctrModal;
    int counter=0;
};

#endif // MAINWINDOW_H
