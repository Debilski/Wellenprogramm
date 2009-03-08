#include <QtGui>
#include "main_window.h"


// if we include <QtGui> there is no need to include every class used: <QString>, <QFileDialog>,...

MainWindow::MainWindow(QMainWindow *parent)
{
  setAttribute(Qt::WA_MacMetalStyle);
  setupUi(this); // this sets up GUI
  setWindowTitle("Select Lattice Size");
  // signals/slots mechanism in action

  lattice_menu = new QMenu(this);
  {
    QList< QVariant > templist;
    QList< QList< QVariant > > metalist;

    metalist << (templist << 4 << 8); templist.clear();
    metalist << (templist << 8 << 8); templist.clear();

    metalist << (templist << 32 << 32); templist.clear();
    metalist << (templist << 16 << 32); templist.clear();
    metalist << (templist << 64 << 64); templist.clear();
    metalist << (templist << 16 << 1024); templist.clear();
    metalist << (templist << 8 << 512); templist.clear();
    metalist << (templist << 4 << 256); templist.clear();
    metalist << (templist << 128 << 128); templist.clear();
    metalist << (templist << 128 << 256); templist.clear();
    metalist << (templist << 128 << 512); templist.clear();
    metalist << (templist << 128 << 1024); templist.clear();
    metalist << (templist << 256 << 256); templist.clear();
    metalist << (templist << 256 << 512); templist.clear();
    metalist << (templist << 256 << 2048); templist.clear();
    metalist << (templist << 512 << 512); templist.clear();
metalist << (templist << 256 << 128); templist.clear();

    for (int i = 0; i < metalist.size(); ++i) {
      QString sizeString = QString("%1/%2").arg( metalist.at(i).at(0).toInt() ).arg( metalist.at(i).at(1).toInt() );
      QAction *newAct = new QAction(sizeString, this);
      newAct->setData(metalist.at(i));
      lattice_menu->addAction(newAct);
    }
    this->show();
  }

  chain_menu = new QMenu(this);
  {
    QList< QVariant > templist;
    QList< QList< QVariant > > metalist;

    metalist << (templist << 32 << 32); templist.clear();
    metalist << (templist << 128 << 512); templist.clear();
    metalist << (templist << 128 << 1024); templist.clear();
    metalist << (templist << 256 << 2048); templist.clear();
    metalist << (templist << 256 << 2048); templist.clear();

    metalist << (templist << 8 << 4096); templist.clear();
    metalist << (templist << 32 << 1024); templist.clear();


    for (int i = 0; i < metalist.size(); ++i) {
      QString sizeString = QString("%1/%2").arg( metalist.at(i).at(0).toInt() ).arg( metalist.at(i).at(1).toInt() );
      QAction *newAct = new QAction(sizeString, this);
      newAct->setData(metalist.at(i));
      chain_menu->addAction(newAct);
    }

  }

  connect (lattice_menu, SIGNAL( triggered(QAction*) ), this, SLOT( openPlot2D(QAction*) ) );

  button2d->setMenu(lattice_menu);
  //connect( pushButton_3, SIGNAL( clicked() ), this, SLOT( openPlot2D() ) );
  connect( actionAbout, SIGNAL( triggered() ), this, SLOT( about() ) );
  //connect( closeEvent, SIGNAL(
  //connect( pushButton_clear, SIGNAL( clicked() ), this, SLOT( clear() ) );
  //connect( pushButton_about, SIGNAL( clicked() ), this, SLOT( about() ) );

  readSettings();
/*
  TinyDoubleEdit tEdit(this, 10);


  if ( tEdit.exec() ) {
  double c = tEdit.value();
  std::cout << c;
  setStatusTip( QString("%1").arg(c) );
  }
  */
}

void MainWindow::closeEvent ( QCloseEvent * event )
{
        writeSettings();
        event->accept();

  emit window_closed();
  QMainWindow::closeEvent(event);
  //this->closeAllWindows()
}


void MainWindow::getPath()
{
  QString path;

  path = QFileDialog::getOpenFileName(this,
                                       "Choose a file to open",
                                       QString::null,
                                       QString::null);

}


void MainWindow::openPlot2D()
{
  plotWin_2D = new Waveprogram2DPlot(this);
}

void MainWindow::openPlot2D(int realSize, int latticeSize)
{
  plotWin_2D = new Waveprogram2DPlot(this, realSize, latticeSize);
}

void MainWindow::openPlot2D(QAction * act)
{
  QVariant qv = act->data();
  plotWin_2D = new Waveprogram2DPlot(this, qv.toList().at(0).toInt(), qv.toList().at(1).toInt());
}


void MainWindow::about()
{
  QMessageBox::about(this,"About myQtApp",
                      "Test \n\n"
                      "Bye.\n");
}

void MainWindow::readSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    //resize(settings.value("size", QSize(400, 400)).toSize());
    //move(settings.value("pos", QPoint(200, 200)).toPoint());
    settings.endGroup();

}

void MainWindow::writeSettings() {
    QSettings settings;
    settings.beginGroup("MainWindow");
//    settings.setValue("size", size());
//    settings.setValue("pos", pos());
    settings.setValue("geometry", saveGeometry());

    settings.endGroup();

}

