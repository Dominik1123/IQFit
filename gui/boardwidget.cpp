/***************************************************************************************
 *
 * This program solves the 2D puzzle "IQ Fit".
 * Copyright (C) 2016  Dominik Vilsmeier
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***************************************************************************************/

#include <QHBoxLayout>
#include <QPixmap>
#include <QWidget>
#include "boardwidget.h"

BoardWidget::BoardWidget(QWidget *parent) : QWidget(parent)
{
    QWidget* w;

    layout = new QGridLayout();
    for(int i=0; i<10; ++i) {
        QHBoxLayout *hlayout = new QHBoxLayout();
        for(int j=0; j<5; ++j) {
            w = new QWidget(this);
            w->setStyleSheet("QWidget { border: 1px solid black; }");
            hlayout->addWidget(w);
        }
        layout->addLayout(hlayout, i, 0, 1, 5);
    }

    this->setLayout(layout);
}

BoardWidget::~BoardWidget()
{
}

void BoardWidget::changeSite(Piece *p, QChar version, int const y, int const x, int const rotation, bool remove)
{
    QString stylesheet = "QWidget { border: 1px solid black; background-image: url(" + p->getFilepath() + "); }";
    if(remove) {
        stylesheet = "QWidget { border: 1px solid black; }";
    }

    int x_range, y_range;
    if(rotation == 0) {

        x_range = 2;
        y_range = p->getYRange();

        for(int xx=0; xx<x_range; ++xx) {
            for(int yy=0; yy<y_range; ++yy) {
                if(p->get(version, xx, yy) == 1) {
                    this->layout->itemAt(y+yy)->layout()->itemAt(x+xx)->widget()->setStyleSheet(stylesheet);
                }
            }
        }

    } else if(rotation == 1) {

        x_range = p->getYRange();
        y_range = 2;

        for(int xx=0; xx<x_range; ++xx) {
            for(int yy=0; yy<y_range; ++yy) {
                if(p->get(version, yy, x_range-1-xx) == 1) {
                    this->layout->itemAt(y+yy)->layout()->itemAt(x+xx)->widget()->setStyleSheet(stylesheet);
                }
            }
        }

    } else if(rotation == 2) {

        x_range = 2;
        y_range = p->getYRange();

        for(int xx=0; xx<x_range; ++xx) {
            for(int yy=0; yy<y_range; ++yy) {
                if(p->get(version, x_range-1-xx, y_range-1-yy) == 1) {
                    this->layout->itemAt(y+yy)->layout()->itemAt(x+xx)->widget()->setStyleSheet(stylesheet);
                }
            }
        }

    } else if(rotation == 3) {

        x_range = p->getYRange();
        y_range = 2;

        for(int xx=0; xx<x_range; ++xx) {
            for(int yy=0; yy<y_range; ++yy) {
                if(p->get(version, y_range-1-yy, xx) == 1) {
                    this->layout->itemAt(y+yy)->layout()->itemAt(x+xx)->widget()->setStyleSheet(stylesheet);
                }
            }
        }
    }
}

void BoardWidget::clear()
{
    for(int y=0; y<10; ++y) {
        for(int x=0; x<5; ++x) {
            this->layout->itemAt(y)->layout()->itemAt(x)->widget()->setStyleSheet("QWidget { border: 1px solid black; }");
        }
    }
}
