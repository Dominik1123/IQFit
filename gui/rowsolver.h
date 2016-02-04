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

#ifndef ROWSOLVER_H
#define ROWSOLVER_H

#include "piece.h"
#include <QChar>
#include <QList>
#include <QObject>

class BoardWidget;

class RowSolver : public QObject
{
    Q_OBJECT
public:
    explicit RowSolver(BoardWidget* boardwidget, QObject *parent = 0);
    ~RowSolver();

    int placePieceOnBoard(Piece* piece, int const which_row, int const x0, int const rotation);
    void setPieces(QList<Piece*> pieces);

    static const int WAIT_TIME_MILLI_SEC_DEFAULT=500;
    int waitTimeMilliSec;

private:
    BoardWidget* boardwidget;
    int board[10][5];
    QList<Piece*>* pieces;

    void iter_rows(int const which_row);
    void removePieceFromBoard(Piece* piece, int const which_row, int const x0, int const rotation);

signals:
    void placedPiece(Piece* p, QChar version, int const y, int const x, int const rotation, bool remove);
    void workDone();

public slots:
    void start();
};

#endif // ROWSOLVER_H
