#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QLocale>
#include <QMessageBox>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtGui/QPainter>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string>
#include <map>
#include <fstream>
#include <filesystem>
#include <signal.h>
#include <iostream>
#include <ctime>

#include <boost/algorithm/string/trim.hpp>

#include "headers/mainwindow.h"
#include "ui_mainwindow.h"
#include "headers/additional_functions.h"
#include "headers/parse_proc_dir.h"
#include "headers/structures.h"
#include "headers/reading_from_file.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),

    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        
        // creation of the database
        db = QSqlDatabase::addDatabase("QSQLITE");
        std::string curr_path = std::filesystem::temp_directory_path();
        std::string path_to_db = curr_path + "/db.sqlite";
        if(std::filesystem::exists(path_to_db)){
            std::filesystem::remove(path_to_db); // delete old database
        }

        db.setDatabaseName(path_to_db.c_str());
        if(!db.open()){
            std::cout << "Cannot create database!!!" << std::endl;
            QMessageBox::warning(this,
                                         "Database creation",
                                         "Cannot create database",
                                         QMessageBox::Ok);
            qApp->quit();
        }
        QString query = "CREATE TABLE all_finfos ("
                "time_checkp Integer,"
                "name varchar,"
                "pid integer,"
                "state varchar,"
                "cpu_usage real,"
                "virt Real,"
                "pss Real,"
                "mem_usage real"
              ");"
                "";
        QSqlQuery qry;
        if(!qry.exec(query)){
            QMessageBox::warning(this,
                                         "Database creation",
                                         "Cannot create table",
                                         QMessageBox::Ok);
            qApp->quit();
        }
        
        //get the constant information 
        std::map<std::string, std::string> consti = get_constant_info();
        constinf.cpu_cores = stoi(consti["cpu cores"]);
        constinf.cpu_name = consti["model name"];

        // create timer that will call update_database() with the delay
        wind_timer = new QTimer(this);
        connect(wind_timer, &QTimer::timeout, this, &MainWindow::update_database);
        wind_timer->start(1000);

        // edit table of processes, connect titles to function
        ui->tableWidget->verticalHeader()->setVisible(false);
        QObject::connect( ui->tableWidget->horizontalHeader(), SIGNAL( sectionClicked( int ) ), this, SLOT( on_myTable_sectionClicked( int ) ) );

        // add processes icon on the processes table
        QPixmap pixmap(":/photos/processes.png");
        QIcon ButtonIcon(pixmap);
        ui->Processes_Button->setIcon(ButtonIcon);
        ui->Processes_Button->setIconSize(QSize(20, 20));

        // add cpu icon on the processes table
        QPixmap pixmap1(":/photos/cpu.png");
        QIcon ButtonIcon1(pixmap1);
        ui->CPU_Button->setIcon(ButtonIcon1);
        ui->CPU_Button->setIconSize(QSize(20, 20));
        
        // add memory icon on the processes table
        QPixmap pixmap2(":/photos/memory.png");
        QIcon ButtonIcon2(pixmap2);
        ui->Memory_Button->setIcon(ButtonIcon2);
        ui->Memory_Button->setIconSize(QSize(20, 20));

        // add about us icon on the processes table
        QPixmap pixmap3(":/photos/aboutus.png");
        QIcon ButtonIcon3(pixmap3);
        ui->AboutUs_Button->setIcon(ButtonIcon3);
        ui->AboutUs_Button->setIconSize(QSize(20, 20));

        // add help icon on the processes table
        QPixmap pixmap4(":/photos/help.png");
        QIcon ButtonIcon4(pixmap4);
        ui->Help_Button->setIcon(ButtonIcon4);
        ui->Help_Button->setIconSize(QSize(25, 25));

        // add dark_theme icon on the processes table
        QPixmap pixmap5(":/photos/dark_theme.png");
        QIcon ButtonIcon5(pixmap5);
        ui->DarkTheme->setIcon(ButtonIcon5);
        ui->DarkTheme->setIconSize(QSize(25, 25));

        // create buttons list
        btns.push_back(ui->Processes_Button);
        btns.push_back(ui->CPU_Button);
        btns.push_back(ui->Memory_Button);
        btns.push_back(ui->AboutUs_Button);
        btns.push_back(ui->Help_Button);
        btns.push_back(ui->DarkTheme);

        // create all the graphs
        ui->plot->addGraph();
        ui->plot_2->addGraph();
        ui->plot_3->addGraph();
        ui->plot_4->addGraph();
        
        // create graphs list
        plts.push_back(ui->plot);
        plts.push_back(ui->plot_2);
        plts.push_back(ui->plot_3);
        plts.push_back(ui->plot_4);
        
        // get first /proc information
        update_database();
        
        //style text fields
        style_text_fields();
        
        //set default colors
        theme_highlight_color = "#90e0ef";
        theme_default_color = "#efefef";
        theme_text_color = "#000000";
        std::string background_color = "background-color:" + theme_default_color + ";";
        this->setStyleSheet(QString::fromStdString(background_color));
        
        //render first page
        on_Processes_Button_clicked();
    }


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::style_text_fields(){
    /**
    * @brief This function sets styleSheet of all the text fields in the application.
    */
    std::string text_c = "color:'" + theme_text_color + "';";
    std::string cpu_text_c = "background-color:'#289EE37D'; " + text_c;
    std::string mem_text_c = "background-color:'#229D4EDD'; " + text_c;

    ui->NameText->setStyleSheet(QString("background-color:'#8863C132'; font-weight: 900; "
                                        " font-size: 17px;  margin-left: 250px; ;  margin-right: 250px;"));

    ui->CP_usage_text->setStyleSheet(QString(cpu_text_c.c_str()));
    ui->NumbOfCorsText->setStyleSheet(QString(cpu_text_c.c_str()));
    ui->NumbOfProcsText->setStyleSheet(QString(cpu_text_c.c_str()));
    ui->WorkingTimeText->setStyleSheet(QString(cpu_text_c.c_str()));

    ui->CachedMemText->setStyleSheet(QString(mem_text_c.c_str()));
    ui->FreeMemText->setStyleSheet(QString(mem_text_c.c_str()));
    ui->TotalMemText->setStyleSheet(QString(mem_text_c.c_str()));
    ui->UsedMemText->setStyleSheet(QString(mem_text_c.c_str()));


    ui->UsedCpuText_2->setStyleSheet(QString(cpu_text_c.c_str()));
    ui->UsedCpuText_3->setStyleSheet(QString(cpu_text_c.c_str()));
    ui->UsedMemText_2->setStyleSheet(QString(mem_text_c.c_str()));

    ui->Title_2->setStyleSheet(QString("font-weight: 900; font-size: 17px;  margin-left: 250px; ;  margin-right: 250px;"));

};

void MainWindow::update_database(){
    /**
    * @brief This function reads the /proc filesystem and gets information
    * about all the processes. It fills all_task_info and insert data to 
    * the database.
    */
    // clear old information to fill new one
    all_tasks_info.clear();
    
    long mem_total = get_mem_total();
    time_t now = time(0);
    
    if (pushed_time_checkpoints >= 100){ // check whether we have not more than 100 different timepoints
        // if there are more than 100 different timepoints (the information older than 100 seconds) we delete it
        QString query = "DELETE FROM all_finfos WHERE time_checkp = (SELECT MIN(time_checkp) FROM all_finfos);";
        QSqlQuery qry(db);
        if(!qry.exec(query)){
            QMessageBox::warning(this,
                                         "Database creation",
                                         "Cannot delete checkpoint",
                                         QMessageBox::Ok);
            qApp->quit();
        }
        pushed_time_checkpoints--;
    }
    QSqlQuery qry(db);
    qry.prepare("INSERT INTO all_finfos VALUES (?, ?, ?, ?, ?, ?, ?, ?);");
    QVariantList checkps;
    QVariantList procnames;
    QVariantList pids;
    QVariantList procstates;
    QVariantList cpuusages;
    QVariantList vm_s;
    QVariantList psss;
    QVariantList memusages;

    for(auto &p : std::filesystem::directory_iterator("/proc/")){ // read /proc and fill all_tasks_info
        std::string file_path =p.path();
        struct task_manager_file_info finfo;
        std::vector<std::string> vec = split(file_path, "/");
        std::string filename = vec[vec.size()-1];
        if(!is_number(filename)){
            continue;
        }
        finfo.pid = stoi(filename);
        get_cpu_and_name(&finfo, file_path);
        if (finfo.process_name == ""){
            continue;
        }
        get_memory_info(&finfo, file_path, mem_total);
        get_proc_state(&finfo, file_path);



        checkps << QString::fromStdString(std::to_string(now));
        procnames << QString::fromStdString(finfo.process_name);
        pids << QString::fromStdString(std::to_string(finfo.pid));
        procstates << QString::fromStdString(finfo.proc_state);
        cpuusages << QString::fromStdString(std::to_string(finfo.cpu_usage));
        vm_s << QString::fromStdString(std::to_string(finfo.vm_size));
        psss << QString::fromStdString(std::to_string(finfo.pss));
        memusages << QString::fromStdString(std::to_string(finfo.mem_percentage));

        all_tasks_info.push_back(finfo);

    }
    qry.addBindValue(checkps);
    qry.addBindValue(procnames);
    qry.addBindValue(pids);
    qry.addBindValue(procstates);
    qry.addBindValue(cpuusages);
    qry.addBindValue(vm_s);
    qry.addBindValue(psss);
    qry.addBindValue(memusages);

    if(!qry.execBatch()){
        QMessageBox::warning(this,
                                     "Database creation",
                                     "Cannot add a row",
                                     QMessageBox::Ok);
        qApp->quit();
    }


    pushed_time_checkpoints++;
    render_window();
}

void MainWindow::render_window() {
    /**
    * @brief This function checks what page should be drawn now and 
    * calls needed functions.
    */
    if (actv_wind.processes) {
        update_table();
    }
    else if (actv_wind.cpu) {
        update_cpu_graph();

    } else if (actv_wind.memory) {
        update_mem_graph();

    } else if(actv_wind.graph_proc){
        update_proc_graph();
    } else {
        update_table();
    }
}
