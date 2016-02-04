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

#ifndef PIECE_H
#define PIECE_H

#include <QChar>
#include <QString>

class Piece
{
public:
    Piece();

    static Piece* createPiece(int const nr, QString pattern="");
    int get(int x, int y);
    int get(QChar version, int x, int y);
    void set(QString version, int x, int y);
    QChar getVersion();
    void setVersion(QChar version);
    QString getName();
    QString getFilepath();
    QString getPattern();
    void setPattern(QString pattern);
    int getYRange();
    void setYRange(int y_range);
    int getXRange(int const rotation);
    bool isUsed();
    void setUsed(bool used=true);
    bool isSkipped();
    void setSkip(bool skip=true);
    int getPosition();
    void clear();

private:
    QChar version;
    int position;
    QString name;
    QString pattern;
    bool skip;
    bool used;
    int A[4][2];
    int B[4][2];
    int y_range;
    int x_range_A[4];
    int x_range_B[4];
    QString filepath;
};

#endif // PIECE_H
