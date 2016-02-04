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

#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include "boardwidget.h"
#include "piece.h"
#include "rowsolver.h"
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QPushButton>
#include <QSlider>
#include <QStringList>
#include <QThread>
#include <QWidget>

class ContainerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ContainerWidget(QWidget *parent = 0);
    ~ContainerWidget();

private:
    struct Solution {
        QStringList patterns;
        int id;
    };

    BoardWidget* board;
    QList<Solution>* solutions;
    RowSolver* rowsolver;
    QThread workerThread;

    QList<Piece*> placedPieces;

    QPushButton* clearButton;
    QList<Piece*> pieces;
    QComboBox* comboPieces;
    QComboBox* comboVersions;
    QComboBox* comboRotations;
    QLabel* yLabel;
    QComboBox* yInput;
    QLabel* xLabel;
    QComboBox* xInput;
    QPushButton* addButton;
    QPushButton* removeButton;
    QPushButton* solveButton;
    QPushButton* previousSolutionButton;
    QPushButton* nextSolutionButton;
    int currentSolution;
    QLabel* solutionsLabel;
    QLineEdit* selectSolutionLineEdit;
    QPushButton* goToSolutionButton;
    QComboBox* nineOrTenComboBox;
    QString filepath_nine_pieces;
    QString filepath_ten_pieces;
    QPushButton* solveRowButton;
    QPushButton* solveBruteForceButton;
    QSlider* solverSpeedSlider;
    QLabel* solverSpeed;

    void populateBoard();

signals:
    void startWork();

public slots:
    void addPiece(bool b);
    void removePiece(bool b);
    void clearBoard(bool b);
    void solveBoard(bool b);
    void previousSolution(bool b);
    void nextSolution(bool b);
    void goToSolution(bool b);
    void changeSolutionsLabelToSearching(bool b);
    void doNothing(int i);

    void updateSolverSpeed(int value);

    void solveBoardRows(bool b);
    void solverFinished();
};

#endif // CONTAINERWIDGET_H
