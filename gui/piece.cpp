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

#include <QFile>
#include "piece.h"

Piece::Piece()
{
    for(int y=0; y<4; ++y) {
        for(int x=0; x<2; ++x) {
            this->A[y][x] = 0;
            this->B[y][x] = 0;
        }
    }
    this->pattern = "";
    this->used = false;
    this->skip = false;
    this->version = 'A';
}

int Piece::get(int x, int y)
{
    if(version == 'A') {
        return this->A[y][x];
    } else if(version == 'B') {
        return this->B[y][x];
    }

    qDebug("wrong version");

    return -1;
}

int Piece::get(QChar version, int x, int y)
{
    if(version == 'A') {
        return this->A[y][x];
    } else if(version == 'B') {
        return this->B[y][x];
    }

    qDebug("wrong version");

    return -1;
}

void Piece::set(QString version, int x, int y)
{
    if(version == "A") {
        this->A[y][x] = 1;
    } else if(version == "B") {
        this->B[y][x] = 1;
    }
}

QChar Piece::getVersion()
{
    return version;
}

void Piece::setVersion(QChar version)
{
    this->version = version;
}

QString Piece::getName()
{
    return name;
}

QString Piece::getFilepath()
{
    return filepath;
}

QString Piece::getPattern()
{
    return pattern;
}

void Piece::setPattern(QString pattern)
{
    this->pattern = pattern;
}

int Piece::getYRange()
{
    return y_range;
}

void Piece::setYRange(int y_range)
{
    this->y_range = y_range;
}

int Piece::getXRange(const int rotation)
{
    if(version == 'A') {
        return x_range_A[rotation];
    } else if(version == 'B') {
        return x_range_B[rotation];
    }
    return -1;
}

bool Piece::isUsed()
{
    return used;
}

void Piece::setUsed(bool used)
{
    this->used = used;
}

bool Piece::isSkipped()
{
    return skip;
}

void Piece::setSkip(bool skip)
{
    this->skip = skip;
}

int Piece::getPosition()
{
    return position;
}

void Piece::clear()
{
    this->pattern = "";
    this->used = false;
}

Piece* Piece::createPiece(const int nr, QString pattern)
{
    Piece* p = new Piece();

    p->setPattern(pattern);

    p->position = nr;

    if(nr == 0) {

        p->name = "lightgreen";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 1, 2);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);

        p->setYRange(3);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 3;
        p->x_range_A[2] = 2;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 3;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 1) {

        p->name = "darkgreen";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 1, 1);
        p->set("A", 0, 2);

        p->set("B", 0, 0);
        p->set("B", 0, 1);
        p->set("B", 1, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);

        p->setYRange(3);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 3;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 1;
        p->x_range_B[1] = 3;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 2) {

        p->name = "purple";

        p->set("A", 0, 0);
        p->set("A", 1, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 1, 1);
        p->set("B", 0, 2);

        p->setYRange(3);

        p->x_range_A[0] = 2;
        p->x_range_A[1] = 3;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 3;
        p->x_range_B[2] = 1;
        p->x_range_B[3] = 2;

    } else if(nr == 3) {

        p->name = "darkblue";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 1, 1);
        p->set("A", 0, 2);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);

        p->setYRange(3);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 3;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 3;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 4) {

        p->name = "blue";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 0, 3);
        p->set("A", 1, 3);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);
        p->set("B", 0, 3);

        p->setYRange(4);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 2;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 1;
        p->x_range_B[3] = 2;

    } else if(nr == 5) {

        p->name = "red";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 0, 3);
        p->set("A", 1, 3);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 0, 2);
        p->set("B", 0, 3);
        p->set("B", 1, 3);

        p->setYRange(4);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 2;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 6) {

        p->name = "orange";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 1, 2);
        p->set("A", 0, 3);

        p->set("B", 0, 0);
        p->set("B", 0, 1);
        p->set("B", 1, 1);
        p->set("B", 0, 2);
        p->set("B", 0, 3);
        p->set("B", 1, 3);

        p->setYRange(4);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 1;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 7) {

        p->name = "lightblue";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 1, 2);
        p->set("A", 0, 3);

        p->set("B", 0, 0);
        p->set("B", 0, 1);
        p->set("B", 1, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);
        p->set("B", 0, 3);

        p->setYRange(4);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 1;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 1;
        p->x_range_B[3] = 2;

    } else if(nr == 8) {

        p->name = "pink";

        p->set("A", 0, 0);
        p->set("A", 0, 1);
        p->set("A", 1, 1);
        p->set("A", 0, 2);
        p->set("A", 0, 3);

        p->set("B", 0, 0);
        p->set("B", 0, 1);
        p->set("B", 0, 2);
        p->set("B", 1, 2);
        p->set("B", 0, 3);
        p->set("B", 1, 3);

        p->setYRange(4);

        p->x_range_A[0] = 1;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 1;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 2;
        p->x_range_B[3] = 2;

    } else if(nr == 9) {

        p->name = "yellow";

        p->set("A", 0, 0);
        p->set("A", 1, 0);
        p->set("A", 0, 1);
        p->set("A", 0, 2);
        p->set("A", 0, 3);

        p->set("B", 0, 0);
        p->set("B", 1, 0);
        p->set("B", 0, 1);
        p->set("B", 1, 1);
        p->set("B", 0, 2);
        p->set("B", 0, 3);

        p->setYRange(4);

        p->x_range_A[0] = 2;
        p->x_range_A[1] = 4;
        p->x_range_A[2] = 1;
        p->x_range_A[3] = 1;

        p->x_range_B[0] = 2;
        p->x_range_B[1] = 4;
        p->x_range_B[2] = 1;
        p->x_range_B[3] = 2;

    }

    p->filepath = "./../IQFit/images/50x50/" + p->getName() + ".png";
//    p->filepath = "./IQFit/images/50x50/" + p->getName() + ".png";

    return p;
}
