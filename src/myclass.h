#pragma once

#include "config.h"
#include <QApplication>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>

class MyClass : public QWidget {
    Q_OBJECT

    QVBoxLayout *layout;
    QLabel *label;
    QPushButton *pushButton;

  public:
    MyClass(QWidget *wgt = nullptr) : QWidget(wgt) {
        layout = new QVBoxLayout;
        label = new QLabel("PROJECT_NAME: " PROJECT_NAME
                           "\nPROJECT_VERSION: " PROJECT_VERSION);
        pushButton = new QPushButton("Ok");

        this->setLayout(layout);
        layout->addWidget(label);
        layout->addWidget(pushButton);

        label->setAlignment(Qt::AlignCenter);
        connect(pushButton, SIGNAL(clicked()), QApplication::instance(),
                SLOT(quit()));
    }

  public slots:

  signals:

  protected:
};