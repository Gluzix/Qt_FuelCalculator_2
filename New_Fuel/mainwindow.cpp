#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVector>
#include <string>
#include <string.h>

using namespace std;

/*
 *W konstruktorze ustawiam polaczenie w bazie danych oraz ustawiam zmienna pomocnicza selectin counter na zero
 *Tworze sobie dynamicznie Obiekt klasy QMessageBox
 *Which_petrol - defaultowo ustawiona na "none"
 *setAttribute dla messageBoxa, oznacza ze obiekt ma zostac zniszczony i wyczyszczony z pamieci gdy klikniemy Krzyzyk lub Ok
 *Warunek, jezeli baza danuych zostala poprawnie otwarta do zmiennej if_db_connected przypoisujemy true
*/
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    my_db = QSqlDatabase::addDatabase("QSQLITE");
    my_db.setDatabaseName("C:/Users/kamil/Desktop/Samochody/cars.db");
    selecting_counter=0;
    Which_Petrol="None";
    mgbox = new QMessageBox(parent);
    mgbox->setAttribute(Qt::WA_DeleteOnClose, true);
    if(my_db.open())
    {
        if_db_connected=true;
    }
    else if_db_connected=false;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/*
 *Ponizej funkcja odpowiedzialna jest za wpisywanie danych z bazy danych do ComboBoxów
 *zrobilem switch case poniewaz funkcja jest wywolywana rekurencyjnie tyle razy ile trzeba, by umiescic potrzebne dane do COmboBoxow
 * QSqlQuery tworze obiekt by moc manipulowac poleceniami SQL oraz wskaznik zapisuje w wektorze by moc go potem usunac
 * QSQLQueryModel wpisywanie wybranych wartosci do comboboxow(w uproszczeniu)
 * Warunek, jezeli counter osiągnie 10 zaczynamy czyscic pojemnik wektor, oraz przesuwac wartosci
*/
void MainWindow::select_from_db(QString slct)
{
    if(counter==10)
    {
        delete_old_objects();
    }else counter++;

    QSqlQuery *qry = new QSqlQuery(my_db);
    QSqlQueryModel *modal = new QSqlQueryModel();
    vctrQRY.push_back(qry);
    vctrModal.push_back(modal);

    qry->exec(slct);
    modal->setQuery(*qry);
    switch(selecting_counter)
    {
    case 0:
        ui->comboBox->setModel(modal);
        selecting_counter++;
        select_from_db("select distinct engine from "+ui->comboBox_4->currentText()+" where model='"+ui->comboBox->currentText()+"'and fuel='"+Which_Petrol+"'");
        break;
    case 1:
        ui->comboBox_2->setModel(modal);
        selecting_counter++;
        select_from_db("select distinct power from "+ui->comboBox_4->currentText()+" where engine='"+ui->comboBox_2->currentText()+"'and model='"+ui->comboBox->currentText()+"' and fuel='"+Which_Petrol+"'");
        break;
    case 2:
        ui->comboBox_3->setModel(modal);
        break;
    }
}

//Funkcja ktora usuwa najstarszy wskaznik w wektorze, nastepnie przesuwa reszte na pozycje jedna nizej
void MainWindow::delete_old_objects()
{
    vctrModal.remove(0);
    delete vctrModal[0];
    vctrQRY.remove(0);
    delete vctrQRY[0];
    for(int i=0; i<vctrQRY.size(); i++)
    {
        if(i>0)
        {
            vctrQRY.move(i, i-1);
            vctrModal.move(i, i-1);
        }
    }
}


//Druga funkcja ktora sluzy do manipulowania comboboxami w momencie klikniecia w ComboBoxy.
//Poprzednia funkcja jest uzywana w momencie pierwszym tj. wybrania paliwa(diesel or benzyna)
void MainWindow::select_from_db_next(const QString x)
{
    if(counter==10)
    {
        delete_old_objects();
    }else counter++;
    QSqlQuery *qry = new QSqlQuery(my_db);
    QSqlQueryModel *modal = new QSqlQueryModel();
    vctrQRY.push_back(qry);
    vctrModal.push_back(modal);
    counter++;
    switch(selecting_counter)
    {
    case 0:
        qry->exec(x);
        modal->setQuery(*qry);
        ui->comboBox->setModel(modal);
        selecting_counter++;
        select_from_db_next("select distinct engine from "+ui->comboBox_4->currentText()+" where model='"+ui->comboBox->currentText()+"' and fuel='"+Which_Petrol+"'");
        break;
    case 1:
        qry->exec(x);
        modal->setQuery(*qry);
        ui->comboBox_2->setModel(modal);
        selecting_counter++;
        select_from_db_next("select distinct power from "+ui->comboBox_4->currentText()+" where engine='"+ui->comboBox_2->currentText()+"'and model='"+ui->comboBox->currentText()+"' and fuel='"+Which_Petrol+"'");
        break;
    case 2:
        qry->exec(x);
        modal->setQuery(*qry);
        ui->comboBox_3->setModel(modal);
        break;
    }
}


//Select distinct -> eliminuje powielanie przy wyszukiwaniu elementow w bazie danych
//Muszę pobrać jakoś z comboboxa widoczny(zaznaczony) napis i potem zrobic funkcje, ktora bedzie
//do reszty comboboxow pobierala dane z bazy wzgledem tego zaznaczonego napisu.

void MainWindow::show_message(const QString str)
{
    mgbox->setText(str);
    mgbox->show();
}

//Funkcja ktora jest odpowiedzialna za pobranie danych i obliczenie sredniego ksoztu przejechania podanych kilometrow
void MainWindow::on_pushButton_clicked()
{

    double fuelconsump=0;
    if(from_db_or_manual)
    {
        QSqlQuery query;
        query.prepare("SELECT DISTINCT fuelconsump FROM "+ui->comboBox_4->currentText()+" WHERE engine='"+ui->comboBox_2->currentText()+"' AND model='"+ui->comboBox->currentText()+"' AND fuel='"+Which_Petrol+"' AND power ='"+ui->comboBox_3->currentText()+"'");
        query.exec();
        while(query.next())
        {
        fuelconsump = query.value(0).toDouble();
        }
    }else fuelconsump = ui->textEdit_2->toPlainText().toDouble();
    double range = ui->textEdit_3->toPlainText().toDouble();
    double current_fuel_cost = ui->textEdit->toPlainText().toDouble();
    double equal=(range*fuelconsump/100)*current_fuel_cost;
    QString text = QString::number(equal);
    ui->label_7->setText("Zaplacisz: "+text+" zł");
}

void MainWindow::change_statements()
{
    ui->checkBox->setEnabled(true);
    ui->checkBox_2->setEnabled(true);
    ui->textEdit->setEnabled(true);
    ui->textEdit->setText("");
    ui->textEdit_3->setEnabled(true);
    ui->textEdit_3->setText("");
    if_petrl_chs=true;
}

//RadioButton -> Petrol
//RadioButton_2 -> Diesel


void MainWindow::on_radioButton_clicked()
{
    if(!if_petrl_chs)
    {
        change_statements();//wywloanie funkcji ktora zmieni na enabled obiekty w ui
    }
    Which_Petrol="Petrol";
    selecting_counter=0;
    select_from_db("select distinct model from "+ui->comboBox_4->currentText()+" where fuel='"+Which_Petrol+"'"); //jak wyzej, wywolanie funkcji bazy danych.
}
void MainWindow::on_radioButton_2_clicked()
{
    if(!if_petrl_chs)
    {
        change_statements();
    }
    Which_Petrol="Diesel";
    selecting_counter=0;
    select_from_db("select distinct model from "+ui->comboBox_4->currentText()+" where fuel='"+Which_Petrol+"'");//^^^^^^^^^^^^^^
}

//Checkbox_2 - ustawia wybieranie spalania z bazy danych
//Checkbox - uzytkownik wpisuje spalanie manualnie
void MainWindow::on_checkBox_2_clicked()
{
    if(!if_db_connected)
    {
        ui->checkBox_2->setChecked(false);
        QMessageBox bx; bx.setText("Nastaplil blad polaczenia z baza danych!"); bx.exec();
    }
    else
    {
        from_db_or_manual=true;
        ui->checkBox_2->setChecked(true);
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_3->setEnabled(true);
        ui->comboBox_4->setEnabled(true);
        ui->checkBox->setChecked(false);
        ui->textEdit_2->setEnabled(false);
        ui->textEdit_2->setText("Choose petrol");
        //wlaczenie potrzebnych obiektow oraz wylaczenie niepotrzebnych
    }
}
void MainWindow::on_checkBox_clicked()
{
    from_db_or_manual=false;
    ui->checkBox->setChecked(true);
    ui->comboBox->setEnabled(false);
    ui->comboBox_2->setEnabled(false);
    ui->comboBox_3->setEnabled(false);
    ui->comboBox_4->setEnabled(false);
    ui->checkBox_2->setChecked(false);
    ui->textEdit_2->setEnabled(true);
    ui->textEdit_2->setText("");
    //wlaczenie obiektow ui
}

void MainWindow::on_comboBox_4_currentIndexChanged(const QString &arg1)
{
    selecting_counter=0;
    select_from_db_next("select distinct model from "+arg1+" where fuel='"+Which_Petrol+"'");
    //po zmianie w comboboxie wywolywanaj est funkcja ktora uaktualnia resszte boxow
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    selecting_counter=1;
    select_from_db_next("select distinct engine from "+ui->comboBox_4->currentText()+" where model='"+arg1+"' and fuel='"+Which_Petrol+"'");
    //^^^^^^^^^^^^^^^^^
}

void MainWindow::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    selecting_counter=2;
    select_from_db_next("select distinct power from "+ui->comboBox_4->currentText()+" where model = '"+ui->comboBox->currentText()+"' and engine='"+arg1+"' and fuel='"+Which_Petrol+"'");
    //^^^^^^^^^^^^^^
}


void MainWindow::on_comboBox_3_currentIndexChanged(const QString &arg1)
{

}


