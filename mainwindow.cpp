#include <QTimer>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "additional_functions.h"
#include "parse_proc_dir.h"
#include "structures.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),

    ui(new Ui::MainWindow)
    {
        ui->setupUi(this);

        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        std::string curr_path = std::filesystem::current_path();
        std::string path_to_db = curr_path + "/db.sqlite";
        if(std::filesystem::exists(path_to_db)){
            std::filesystem::remove(path_to_db);
        }

        db.setDatabaseName(path_to_db.c_str());
        if(!db.open()){
            std::cout << "Cannot create database!!!" << std::endl;
            QMessageBox::warning(this,
                                         "Database creation",
                                         "Cannot create database",
                                         QMessageBox::Ok);
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
        }

        wind_timer = new QTimer(this);
        connect(wind_timer, &QTimer::timeout, this, &MainWindow::update_table);

        ui->tableWidget->verticalHeader()->setVisible(false);

        QObject::connect( ui->tableWidget->horizontalHeader(), SIGNAL( sectionClicked( int ) ), this, SLOT( on_myTable_sectionClicked( int ) ) );

    }


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update_table() {
    int in = 0;

        updating_t = 1;
        all_tasks_info.clear();
        long mem_total = get_mem_total();
        time_t now = time(0);
        if (pushed_time_checkpoints >= 100){
            QString query = "DELETE FROM all_finfos WHERE time_checkp = (SELECT MIN(time_checkp) FROM all_finfos);";
            QSqlQuery qry;
            if(!qry.exec(query)){
                QMessageBox::warning(this,
                                             "Database creation",
                                             "Cannot delete checkpoint",
                                             QMessageBox::Ok);
                return;
            }
            pushed_time_checkpoints--;
        }
        for(auto &p : std::filesystem::directory_iterator("/proc/")){
            std::string file_path =p.path();
            struct task_manager_file_info finfo;
            std::vector<std::string> vec = split(file_path, "/");
            std::string filename = vec[vec.size()-1];
            if(!is_number(filename)){
                continue;
            }
            finfo.pid = stoi(filename);
            get_cpu_and_name(&finfo, file_path);
            get_memory_info(&finfo, file_path, mem_total);
            get_proc_state(&finfo, file_path);



//            std::string query_std = "INSERT INTO all_finfos VALUES (" +
//                    std::to_string(now) + ", '" + finfo.process_name +
//                    "', " + std::to_string(finfo.pid) + ", '" + finfo.proc_state +
//                    "', " + std::to_string(finfo.cpu_usage) + ", " + std::to_string(finfo.vm_size) + ", "
//                    + std::to_string(finfo.pss) + ", " + std::to_string(finfo.mem_percentage) + ");";
//            QString query = QString::fromStdString(query_std);

            QSqlQuery qry;

            qry.prepare("INSERT INTO all_finfos VALUES (?, ?, ?, ?, ?, ?, ?, ?);");

            qry.addBindValue(QString::fromStdString(std::to_string(now)));
            qry.addBindValue(QString::fromStdString(finfo.process_name));
            qry.addBindValue(QString::fromStdString(std::to_string(finfo.pid)));
            qry.addBindValue(QString::fromStdString(finfo.proc_state));
            qry.addBindValue(QString::fromStdString(std::to_string(finfo.cpu_usage)));
            qry.addBindValue(QString::fromStdString(std::to_string(finfo.vm_size)));
            qry.addBindValue(QString::fromStdString(std::to_string(finfo.pss)));
            qry.addBindValue(QString::fromStdString(std::to_string(finfo.mem_percentage)));


            if(!qry.exec()){
                QMessageBox::warning(this,
                                             "Database creation",
                                             "Cannot add a row",
                                             QMessageBox::Ok);
            }


            all_tasks_info.push_back(finfo);


        }

        pushed_time_checkpoints++;
        std::cout << pushed_time_checkpoints << std::endl;

        /*add rows and columns*/
        ui->tableWidget->setRowCount(all_tasks_info.size());

        sort_table(&all_tasks_info);

        ui->tableWidget->clearContents();
        ui->tableWidget->update();
        /*add stuff inside the table view*/
            for(int i=0; i<ui->tableWidget->rowCount(); i++)
            {
                for(int j=0; j<7; j++)
                {
                    QTableWidgetItem *pCell = ui->tableWidget->item(i, j);
                    if(!pCell)
                    {
                        pCell = new QTableWidgetItem;
                        ui->tableWidget->setItem(i, j, pCell);
                    }
                }
                QTableWidgetItem *pCell0 = ui->tableWidget->item(i, 0);
                pCell0->setText(all_tasks_info[i].process_name.c_str());

                QTableWidgetItem *pCell1 = ui->tableWidget->item(i, 1);
                pCell1->setText(std::to_string(all_tasks_info[i].pid).c_str());

                QTableWidgetItem *pCell2 = ui->tableWidget->item(i, 2);
                pCell2->setText(all_tasks_info[i].proc_state.c_str());

                QTableWidgetItem *pCell3 = ui->tableWidget->item(i, 3);
                std::string s = double_to_string_precision(all_tasks_info[i].cpu_usage, 1);
                pCell3->setText((s + '%').c_str());

                QTableWidgetItem *pCell4 = ui->tableWidget->item(i, 4);
                std::string vm_s = memory_suitable_view(all_tasks_info[i].vm_size);
                pCell4->setText(vm_s.c_str());

                QTableWidgetItem *pCell5 = ui->tableWidget->item(i, 5);
                std::string pss_s = memory_suitable_view(all_tasks_info[i].pss);
                pCell5->setText(pss_s.c_str());

                QTableWidgetItem *pCell6 = ui->tableWidget->item(i, 6);
                pCell6->setText((std::to_string(all_tasks_info[i].mem_percentage) + " %").c_str());

            }
            ui->tableWidget->update();
            updating_t = 0;
            wind_timer->start(1000);
            in++;

}

void MainWindow::on_myTable_sectionClicked(int index) {
    for (int i = 0; i < static_cast<int>(sort_flags.flags.size()); i++){
        if(i == index){
            sort_flags.flags[i] += 1;
            sort_flags.flags[i] %= 2;
        } else {
            sort_flags.flags[i] = -1;
        }
    }
}

void MainWindow::sort_table(std::vector<struct task_manager_file_info>* all_tasks_info) {


    if (sort_flags.flags[0] != -1) {
        if (!sort_flags.flags[0]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    int size_of_words = finfo1.process_name.size() < finfo2.process_name.size() ? finfo1.process_name.size(): finfo2.process_name.size();
                    for(int i = 0; i < size_of_words; i++){
                        bool not_lowered = finfo1.process_name[i] < finfo2.process_name[i];
                        bool lowered1 = std::tolower(finfo1.process_name[i]) < std::tolower(finfo2.process_name[i]);
                        bool lowered2 = std::tolower(finfo2.process_name[i]) < std::tolower(finfo1.process_name[i]);
                        if (finfo1.process_name[i] == finfo2.process_name[i]){
                            continue;
                        } else if (lowered1 == lowered2){
                            return !not_lowered;
                        }else if (lowered1 != lowered2){
                            return lowered1;
                        }
                    }
                    return finfo1.process_name < finfo2.process_name;
                });
        }
        else {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    int size_of_words = finfo1.process_name.size() < finfo2.process_name.size() ? finfo1.process_name.size(): finfo2.process_name.size();
                    for(int i = 0; i < size_of_words; i++){
                        bool not_lowered = finfo1.process_name[i] > finfo2.process_name[i];
                        bool lowered1 = std::tolower(finfo1.process_name[i]) > std::tolower(finfo2.process_name[i]);
                        bool lowered2 = std::tolower(finfo2.process_name[i]) > std::tolower(finfo1.process_name[i]);
                        if (finfo1.process_name[i] == finfo2.process_name[i]){
                            continue;
                        } else if (lowered1 == lowered2){
                            return !not_lowered;
                        }else if (lowered1 != lowered2){
                            return lowered1;
                        }
                    }
                    return finfo1.process_name > finfo2.process_name;
                });
        }
    }

    else if (sort_flags.flags[1] != -1) {
        if (sort_flags.flags[1]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.pid > finfo2.pid;
                });
            return;
        } else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.pid < finfo2.pid;
            });
        }
    }

    else if (sort_flags.flags[2] != -1) {
        if (sort_flags.flags[2]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.proc_state > finfo2.proc_state;
                });
            return;
        } else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.proc_state < finfo2.proc_state;
            });
        }
    }

    else if (sort_flags.flags[3] != -1) {
        if (!sort_flags.flags[3]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.cpu_usage > finfo2.cpu_usage;
                });
            return;
        }
        else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.cpu_usage < finfo2.cpu_usage;
            });
        }
    }

    else if (sort_flags.flags[4]  != -1) {
        if (!sort_flags.flags[4]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.vm_size > finfo2.vm_size;
                });
            return;
        } else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.vm_size < finfo2.vm_size;
            });
        }
    }

    else if (sort_flags.flags[5]  != -1) {
        if (!sort_flags.flags[5] ) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.pss > finfo2.pss;
                });
            return;
        } else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.pss < finfo2.pss;
            });
        }
    }

    else if (sort_flags.flags[6] != -1) {
        if (!sort_flags.flags[6]) {
            std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                    return finfo1.mem_percentage > finfo2.mem_percentage;
                });
            return;
        } else {
        std::sort(all_tasks_info->begin(), all_tasks_info->end(), [](struct task_manager_file_info finfo1, struct task_manager_file_info finfo2) {
                return finfo1.mem_percentage < finfo2.mem_percentage;
            });
        }
    }

    else {
        std::cerr << "Error in sort detection!!!" << std::endl;
    }
}



void MainWindow::contextMenuEvent( QContextMenuEvent * e ) {
    if(updating_t){
        return;
    }
    int row = ui->tableWidget->selectionModel()->currentIndex().row();
    if (row == -1){
        return;
    }
    process_to_kill = all_tasks_info[row].process_name;
    pid_to_kill = all_tasks_info[row].pid;
    QAction *pKillProc = new QAction("Kill process",this);
    connect(pKillProc,SIGNAL(triggered()),this,SLOT(slotKill()));

    //
    QMenu *pContextMenu = new QMenu( this);
    pContextMenu->addAction(pKillProc);
    //
    if(updating_t){
        delete pContextMenu;
        pContextMenu = NULL;
        return;
    }
    pContextMenu->exec( e->globalPos() );

    delete pContextMenu;
    pContextMenu = NULL;
}

void MainWindow::slotKill(){
    std::string delete_message = "Do you really want to kill process: ";
    delete_message += process_to_kill + " ?";
    if (QMessageBox::warning(this,
                             "Killing process",
                             delete_message.c_str(),
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
        /* If the answer is no* */
        return;
    } else {
        for(auto finfo : all_tasks_info){
            if (finfo.pid == pid_to_kill){
                if (finfo.process_name != process_to_kill){
                    QMessageBox::warning(this,
                                                 "Killing process",
                                                 "No such process found",
                                                 QMessageBox::Ok);
                    std::cout << "No such process found" << std::endl;
                }
                break;
            }
        }
        if (pid_to_kill < 0){
            QMessageBox::warning(this,
                                         "Killing process",
                                         "No such process found",
                                         QMessageBox::Ok);
            std::cout << "No such process found" << std::endl;
        }
        if(kill(pid_to_kill, SIGTERM)!=0){
            QMessageBox::warning(this,
                                         "Killing process",
                                         "Cannot kill process",
                                         QMessageBox::Ok);
            std::cout << "Cannot kill process" << std::endl;
        }


    }
}


