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

#include "containerwidget.h"
#include "waiter.h"
#include "rowsolver.h"

RowSolver::RowSolver(BoardWidget *boardwidget, QObject *parent) : QObject(parent), boardwidget(boardwidget)
{
    for(int i=0; i<10; ++i) {
        for(int j=0; j<5; ++j) {
            board[i][j] = 0;
        }
    }

    this->connect(this, SIGNAL(placedPiece(Piece*,QChar,int,int,int,bool)), boardwidget, SLOT(changeSite(Piece*,QChar,int,int,int,bool)));

    pieces = new QList<Piece*>();

    waitTimeMilliSec = WAIT_TIME_MILLI_SEC_DEFAULT;
}

RowSolver::~RowSolver()
{
    delete pieces;
}

void RowSolver::setPieces(QList<Piece*> pieces)
{
    for(QList<Piece*>::iterator it=pieces.begin(); it!=pieces.end(); it+=1) {
        this->pieces->append(*it);
    }
}

void RowSolver::start()
{
    iter_rows(0);
    emit workDone();
}

void RowSolver::iter_rows(const int which_row)
{
    int nopen=5;  // number of free sites in the current row;
    for(int j=0; j<5; ++j) {
        nopen -= board[which_row][j];
    }

    if(nopen == 0) {  // row is already complete;
        if(which_row == 9) {  // is the last row;
            for(QList<Piece*>::iterator it=pieces->begin(); it!=pieces->end(); it+=1) {
                if((*it)->isUsed() == false) (*it)->setSkip();  // set each unused piece on skip because skipped pieces are set on unskip after a row was completed in order to make them available for the next row. However there is no additional row after the last one and in case of solutions including only 9 pieces having unused, unskipped pieces can result in an infinite loop (as they will be used in place of the piece that completed the board in subsequent steps which makes it possible to use the completing piece again). Therefore skip them in case the algorithm found a solution.
            }
            this->disconnect(this, SIGNAL(placedPiece(Piece*,QChar,int,int,int,bool)), boardwidget, SLOT(changeSite(Piece*,QChar,int,int,int,bool)));
            emit workDone();
            // halt execution here and wait for resume;
        } else {  // is not the last row;
            iter_rows(which_row+1);  // move on to the next row;
        }
        return;
    }

    QList<Piece*>::iterator piece_it = pieces->begin();
    while(piece_it != pieces->end() && ((*piece_it)->isUsed() || (*piece_it)->isSkipped())) {
        piece_it += 1;
    }
    if(piece_it == pieces->end()) return;  // All pieces are either used or on skip for the current row -> no solution could be found.

    Piece* piece = *piece_it;

    int x_max;  // x-value of the 'rightmost' site the piece can be placed on in order not to exceed the boundaries of the board;
    int success;  // indicates whether the current piece was placed successfully on the board;

    for(int rotation=0; rotation<4; ++rotation) {  // loop over rotations; rotation are to be regarded clockwise;

        if(rotation%2 == 0) {  // 0 degrees or 180 degrees;
            if(which_row + piece->getYRange() > 10) continue;  // piece would exceed y-range of the board;
            x_max = 3;  // x-extent of piece is 2;
        } else {  // 90 degrees or 270 degrees;
            if(which_row + 2 > 10) continue;  // piece would exceed y-range of the board;
            x_max = 5-piece->getYRange();  // original y-range becomes x-range;
        }

        piece->setVersion('A');
        if(piece->getXRange(rotation) <= nopen) {  // check if there are enough free sites in the current row;

            for(int x=0; x<=x_max; ++x) {  // x denotes the x-position of the left upper corner of the enclosing rectangle within which the piece will be placed;

                success = placePieceOnBoard(piece, which_row, x, rotation);
                if(success == 0) {  // no overlap of pieces;

                    emit placedPiece(piece, 'A', which_row, x, rotation, false);
                    Waiter::msleep(waitTimeMilliSec);
                    piece->setUsed();
                    piece->setPattern( QString::number(1000 + 100*rotation + 10*which_row + x) );  // mark piece as used (see declaration of Piece for encoding);
                    nopen -= piece->getXRange(rotation);  // adjust number of free sites;
                    if(nopen == 0) {  // current row is complete;

                        for(QList<Piece*>::iterator it=pieces->begin(); it!=pieces->end(); it+=1) {
                            (*it)->setSkip(false);  // reset skip of pieces in order to make them available for the next row;
                        }
                        iter_rows(which_row+1);  // move on to the next row;
                    } else {  // row is not complete;

                        iter_rows(which_row);  // stay within the current row;
                    }
                    emit placedPiece(piece, 'A', which_row, x, rotation, true);
                    Waiter::msleep(waitTimeMilliSec);
                    removePieceFromBoard(piece, which_row, x, rotation);  // after deeper recursions returned remove the piece from the board in order to place it at another location or to skip it for the current row;
                    piece->setUsed(false);  // adjust used indicator;
                    piece->setPattern("");
                    nopen += piece->getXRange(rotation);  // adjust number of free sites;
                    for(QList<Piece*>::iterator it=piece_it+1; it!=pieces->end(); it+=1) {
                        (*it)->setSkip(false);  // reset skip of subsequent pieces so they can be used in the current row (their skip was set in deeper recursions); preceding pieces, if skipped, remain unchanged as their combination with other pieces was already checked in higher recursion levels (only 'forward' (or better 'downward') generation of combinations, no double generation);
                    }
                }
            }
        }


        // repeat the above procedure for version B; for explanations/comments see the part for version A;
        piece->setVersion('B');
        if(piece->getXRange(rotation) <= nopen) {

            for(int x=0; x<=x_max; ++x) {

                success = placePieceOnBoard(piece, which_row, x, rotation);
                if(success == 0) {

                    emit placedPiece(piece, 'B', which_row, x, rotation, false);
                    Waiter::msleep(waitTimeMilliSec);
                    piece->setUsed();
                    piece->setPattern( QString::number(2000 + 100*rotation + 10*which_row + x) );
                    nopen -= piece->getXRange(rotation);
                    if(nopen == 0) {

                        for(QList<Piece*>::iterator it=pieces->begin(); it!=pieces->end(); it+=1) {
                            (*it)->setSkip(false);
                        }
                        iter_rows(which_row+1);
                    } else {

                        iter_rows(which_row);
                    }
                    emit placedPiece(piece, 'B', which_row, x, rotation, true);
                    Waiter::msleep(waitTimeMilliSec);
                    removePieceFromBoard(piece, which_row, x, rotation);
                    piece->setUsed(false);
                    piece->setPattern("");
                    nopen += piece->getXRange(rotation);
                    for(QList<Piece*>::iterator it=piece_it+1; it!=pieces->end(); it+=1) {
                        (*it)->setSkip(false);
                    }
                }
            }
        }
    }

    // finally, do not use the current piece for the current row (so it can be used for subsequent rows);
    piece->setSkip();
    iter_rows(which_row);  // stay within the current row;
}

int RowSolver::placePieceOnBoard(Piece *piece, const int which_row, const int x0, const int rotation)
{
    int tmp_board[4][4];  // temporal array for storing the new board values (so in case of an overlap already written board values don't need to be reset);
    int y_max, x_max;  // the actual y- and x-extent of the piece;

    if(rotation == 0) {  // 0 degrees;

        x_max = 2;
        y_max = piece->getYRange();
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                tmp_board[y][x] = board[which_row+y][x0+x] + piece->get(x, y);

                if(tmp_board[y][x] > 1) {  // check if pieces overlap;
                    return -1;
                }
            }
        }

        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
            }
        }

    } else if(rotation == 1) {  // 90 degrees;

        x_max = piece->getYRange();
        y_max = 2;
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                tmp_board[y][x] = board[which_row+y][x0+x] + piece->get(y, x_max-1-x);

                if(tmp_board[y][x] > 1) {  // check if pieces overlap;
                    return -1;
                }
            }
        }

        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
            }
        }

    } else if(rotation == 2) {  // 180 degrees;

        x_max = 2;
        y_max = piece->getYRange();
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                tmp_board[y][x] = board[which_row+y][x0+x] + piece->get(x_max-1-x, y_max-1-y);

                if(tmp_board[y][x] > 1) {  // check if pieces overlap;
                    return -1;
                }
            }
        }

        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
            }
        }

    } else {  // rotation == 3, 270 degrees;

        x_max = piece->getYRange();
        y_max = 2;
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                tmp_board[y][x] = board[which_row+y][x0+x] + piece->get(y_max-1-y, x);

                if(tmp_board[y][x] > 1) {  // check if pieces overlap;
                    return -1;
                }
            }
        }

        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] = tmp_board[y][x];  // if there is no overlap copy the board values from the temp array to the board array;
            }
        }
    }

    return 0;
}

void RowSolver::removePieceFromBoard(Piece *piece, const int which_row, const int x0, const int rotation)
{
    int y_max, x_max;

    if(rotation == 0) {

        x_max = 2;
        y_max = piece->getYRange();
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] -= piece->get(x, y);
            }
        }

    } else if(rotation == 1) {

        x_max = piece->getYRange();
        y_max = 2;
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] -= piece->get(y, x_max-1-x);
            }
        }

    } else if(rotation == 2) {

        x_max = 2;
        y_max = piece->getYRange();
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] -= piece->get(x_max-1-x, y_max-1-y);
            }
        }

    } else {  // rotation == 3;

        x_max = piece->getYRange();
        y_max = 2;
        for(int x=0; x<x_max; ++x) {
            for(int y=0; y<y_max; ++y) {

                board[which_row+y][x0+x] -= piece->get(y_max-1-y, x);
            }
        }
    }
}
