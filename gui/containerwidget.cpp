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
#include <QGridLayout>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>
#include "containerwidget.h"

ContainerWidget::ContainerWidget(QWidget *parent) : QWidget(parent)
{
//    filepath_nine_pieces = ".\\IQFit\\data\\combinations_nine_pieces.txt";
//    filepath_ten_pieces = ".\\IQFit\\data\\combinations_ten_pieces.txt";
    filepath_nine_pieces = ".\\..\\IQFit\\data\\combinations_nine_pieces.txt";
    filepath_ten_pieces = ".\\..\\IQFit\\data\\combinations_ten_pieces.txt";

    solutions = new QList<Solution>();

    board = new BoardWidget(this);
    rowsolver = new RowSolver(board);

    QGridLayout* layout = new QGridLayout();

    nineOrTenComboBox = new QComboBox(this);
    nineOrTenComboBox->addItem("nine pieces", filepath_nine_pieces);
    nineOrTenComboBox->addItem("ten pieces", filepath_ten_pieces);
    layout->addWidget(nineOrTenComboBox, 0, 0, 1, 1);

    clearButton = new QPushButton(this);
    clearButton->setText("clear board");
    layout->addWidget(clearButton, 1, 0, 1, 1);

    comboPieces = new QComboBox(this);
    for(int i=0; i<10; ++i) {
        pieces.append(Piece::createPiece(i));
        comboPieces->addItem(pieces.last()->getName());
    }
    layout->addWidget(comboPieces, 1, 1, 1, 1);

    comboVersions = new QComboBox(this);
    comboVersions->addItem("A");
    comboVersions->addItem("B");
    layout->addWidget(comboVersions, 1, 2, 1, 1);

    comboRotations = new QComboBox(this);
    comboRotations->addItem("0 deg", 0);
    comboRotations->addItem("90 deg", 1);
    comboRotations->addItem("180 deg", 2);
    comboRotations->addItem("270 deg", 3);
    layout->addWidget(comboRotations, 1, 3, 1, 1);

    yLabel = new QLabel(this);
    yLabel->setText("y=");
    layout->addWidget(yLabel, 1, 4, 1, 1);

    yInput = new QComboBox(this);
    for(int y=0; y<10; ++y) {
        yInput->addItem(QString::number(y));
    }
    layout->addWidget(yInput, 1, 5, 1, 1);

    xLabel = new QLabel(this);
    xLabel->setText("x=");
    layout->addWidget(xLabel, 1, 6, 1, 1);

    xInput = new QComboBox(this);
    for(int x=0; x<5; ++x) {
        xInput->addItem(QString::number(x));
    }
    layout->addWidget(xInput, 1, 7, 1, 1);

    addButton = new QPushButton(this);
    addButton->setText("add piece");
    layout->addWidget(addButton, 1, 8, 1, 1);

    removeButton = new QPushButton(this);
    removeButton->setText("remove piece");
    removeButton->setEnabled(false);
    layout->addWidget(removeButton, 2, 8, 1, 1);

    layout->addWidget(board, 2, 0, 10, 5);

    solveButton = new QPushButton(this);
    solveButton->setText("solve");
    layout->addWidget(solveButton, 1, 9, 1, 1);

    solveRowButton = new QPushButton(this);
    solveRowButton->setText("solve: rows");
    layout->addWidget(solveRowButton, 2, 9, 1, 1);

    solveBruteForceButton = new QPushButton(this);
    solveBruteForceButton->setText("solve: brute force");
    solveBruteForceButton->setEnabled(false);
    layout->addWidget(solveBruteForceButton, 3, 9, 1, 1);

    solverSpeed = new QLabel(this);
    solverSpeed->setText(QString("solver speed: x%1").arg(QString::number(1)));
    layout->addWidget(solverSpeed, 4, 9, 1, 1);

    solverSpeedSlider = new QSlider(Qt::Horizontal, this);
    solverSpeedSlider->setRange(1,50);
    solverSpeedSlider->setSingleStep(1);
    layout->addWidget(solverSpeedSlider, 5, 9, 1, 1);

    solutionsLabel = new QLabel(this);
    solutionsLabel->setText("click solve to find solutions");
    layout->addWidget(solutionsLabel, 9, 8, 1, 2);

    goToSolutionButton = new QPushButton(this);
    goToSolutionButton->setText("go to:");
    goToSolutionButton->setEnabled(false);
    layout->addWidget(goToSolutionButton, 10, 8, 1, 1);

    selectSolutionLineEdit = new QLineEdit(this);
    selectSolutionLineEdit->setEnabled(false);
    layout->addWidget(selectSolutionLineEdit, 10, 9, 1, 1);

    previousSolutionButton = new QPushButton(this);
    previousSolutionButton->setText("previous");
    previousSolutionButton->setEnabled(false);
    layout->addWidget(previousSolutionButton, 11, 8, 1, 1);

    nextSolutionButton = new QPushButton(this);
    nextSolutionButton->setText("next");
    nextSolutionButton->setEnabled(false);
    layout->addWidget(nextSolutionButton, 11, 9, 1, 1);

    this->setLayout(layout);

    this->connect(addButton, SIGNAL(clicked(bool)), this, SLOT(addPiece(bool)));
    this->connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removePiece(bool)));
    this->connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearBoard(bool)));
    this->connect(solveButton, SIGNAL(clicked(bool)), this, SLOT(solveBoard(bool)));
//    this->connect(solveButton, SIGNAL(clicked(bool)), this, SLOT(changeSolutionsLabelToSearching(bool)));
    this->connect(previousSolutionButton, SIGNAL(clicked(bool)), this, SLOT(previousSolution(bool)));
    this->connect(nextSolutionButton, SIGNAL(clicked(bool)), this, SLOT(nextSolution(bool)));
    this->connect(goToSolutionButton, SIGNAL(clicked(bool)), this, SLOT(goToSolution(bool)));
    this->connect(solveRowButton, SIGNAL(clicked(bool)), this, SLOT(solveBoardRows(bool)));
    this->connect(solverSpeedSlider, SIGNAL(valueChanged(int)), this, SLOT(updateSolverSpeed(int)));

    clearBoard(true);
}

ContainerWidget::~ContainerWidget()
{
    delete solutions;
    delete rowsolver;

    workerThread.quit();
    workerThread.wait(1000);
}

void ContainerWidget::addPiece(bool b)
{
    pieces.at(comboPieces->currentIndex())->setUsed();

    QString pattern;

    if(comboVersions->currentText() == "A") {
        pattern += "1";
    } else if(comboVersions->currentText() == "B") {
        pattern += "2";
    }

    if(comboRotations->currentText() == "0 deg") {
        pattern += "0";
    } else if(comboRotations->currentText() == "90 deg") {
        pattern += "1";
    } else if(comboRotations->currentText() == "180 deg") {
        pattern += "2";
    } else if(comboRotations->currentText() == "270 deg") {
        pattern += "3";
    }

    pattern += yInput->currentText();
    pattern += xInput->currentText();

    pieces.at(comboPieces->currentIndex())->setPattern(pattern);

    this->board->changeSite(pieces.at(comboPieces->currentIndex()), comboVersions->currentText()[0], yInput->currentText().toInt(), xInput->currentText().toInt(), comboRotations->itemData(comboRotations->currentIndex()).toInt(), false);

    placedPieces.append(pieces.at(comboPieces->currentIndex()));

    removeButton->setText(QString("remove piece: %1 (%2)").arg( (*(placedPieces.end()-1))->getName(), QString::number((*(placedPieces.end()-1))->getPosition()+1) ));

    if(removeButton->isEnabled() == false) {
        removeButton->setEnabled(true);
    }
}

void ContainerWidget::removePiece(bool b)
{
    Piece* p = placedPieces.takeLast();
    QChar version;
    if(p->getPattern()[0] == '1') {
        version = 'A';
    } else {
        version = 'B';
    }
    this->board->changeSite(p, version, p->getPattern()[2].digitValue(), p->getPattern()[3].digitValue(), p->getPattern()[1].digitValue(), true);

    p->setPattern("");
    p->setUsed(false);

    if(!placedPieces.isEmpty()) {
        removeButton->setText(QString("remove piece: %1 (%2)").arg( (*(placedPieces.end()-1))->getName(), QString::number((*(placedPieces.end()-1))->getPosition()+1) ));
    } else {
        removeButton->setText("remove piece");
        removeButton->setEnabled(false);
    }
}

void ContainerWidget::clearBoard(bool b)
{
    removeButton->setEnabled(false);
    placedPieces.clear();
    board->clear();
    for(int i=0; i<10; ++i) {
        pieces.at(i)->clear();
    }
    goToSolutionButton->setEnabled(false);
    previousSolutionButton->setEnabled(false);
    nextSolutionButton->setEnabled(false);
    solutionsLabel->setText("click solve to find solutions");
}

void ContainerWidget::solveBoard(bool b)
{
    QString debugString;

    QList<Piece*> usedPieces;
    for(int i=0; i<10; ++i) {
        if(pieces.at(i)->isUsed()) {
            usedPieces.append(pieces.at(i));
            debugString = "using piece " + pieces.at(i)->getName() + " @ " + pieces.at(i)->getPattern();
            qDebug(debugString.toStdString().c_str());
        }
    }

    QFile file(nineOrTenComboBox->itemData(nineOrTenComboBox->currentIndex()).toString());
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug("cannot find file.");
        return;
    }

    goToSolutionButton->setEnabled(false);
    selectSolutionLineEdit->setEnabled(false);
    previousSolutionButton->setEnabled(false);
    nextSolutionButton->setEnabled(false);
    solutions->clear();

    struct Solution sol;
    int line_number=1;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList patterns = line.split(',');
        bool validCombination=true;
        for(int i=0; i<usedPieces.size(); ++i) {
            if(usedPieces.at(i)->getPattern() != patterns.at(usedPieces.at(i)->getPosition())) {
                validCombination=false;
            }
        }
        if(validCombination) {
            sol.patterns = patterns;
            sol.id = line_number;
            solutions->append(sol);
        }
        line_number++;
    }

    debugString = "found "; debugString += QString::number(solutions->size()); debugString += " solutions.";
    qDebug(debugString.toStdString().c_str());

    if(solutions->size() == 0) {
        solutionsLabel->setText("no solutions found");
    } else {
        solutionsLabel->setText(QString("solution %1 (%2) / %3").arg("1", QString::number(solutions->at(0).id), QString::number(solutions->size())));
        currentSolution=0;

        populateBoard();

        if(solutions->size() > 1) {
            nextSolutionButton->setEnabled(true);
        }

        goToSolutionButton->setEnabled(true);
        selectSolutionLineEdit->setEnabled(true);
    }
}

void ContainerWidget::previousSolution(bool b)
{
    currentSolution -= 1;
    solutionsLabel->setText(QString("solution %1 (%2) / %3").arg(QString::number(currentSolution+1), QString::number(solutions->at(currentSolution).id), QString::number(solutions->size())));
    populateBoard();
    if(currentSolution == 0) {
        previousSolutionButton->setEnabled(false);
    }
    if(!nextSolutionButton->isEnabled()) {
        nextSolutionButton->setEnabled(true);
    }
}

void ContainerWidget::nextSolution(bool b)
{
    currentSolution += 1;
    solutionsLabel->setText(QString("solution %1 (%2) / %3").arg(QString::number(currentSolution+1), QString::number(solutions->at(currentSolution).id), QString::number(solutions->size())));
    populateBoard();
    if(currentSolution == solutions->size()-1) {
        nextSolutionButton->setEnabled(false);
    }
    if(!previousSolutionButton->isEnabled()) {
        previousSolutionButton->setEnabled(true);
    }
}

void ContainerWidget::goToSolution(bool b)
{
    if(selectSolutionLineEdit->text().toInt() > 0 && selectSolutionLineEdit->text().toInt() <= solutions->size()) {
        currentSolution = selectSolutionLineEdit->text().toInt()-1;
        solutionsLabel->setText(QString("solution %1 (%2) / %3").arg(QString::number(currentSolution+1), QString::number(solutions->at(currentSolution).id), QString::number(solutions->size())));
        populateBoard();
        if(currentSolution == solutions->size()-1) {
            nextSolutionButton->setEnabled(false);
            if(!previousSolutionButton->isEnabled()) {
                previousSolutionButton->setEnabled(true);
            }
        } else if(currentSolution == 0) {
            previousSolutionButton->setEnabled(false);
            if(!nextSolutionButton->isEnabled()) {
                nextSolutionButton->setEnabled(true);
            }
        } else {
            if(!previousSolutionButton->isEnabled()) {
                previousSolutionButton->setEnabled(true);
            }
            if(!nextSolutionButton->isEnabled()) {
                nextSolutionButton->setEnabled(true);
            }
        }
    }
}

void ContainerWidget::changeSolutionsLabelToSearching(bool b)
{
    solutionsLabel->setText("working...");
}

void ContainerWidget::doNothing(int i)
{
}

void ContainerWidget::updateSolverSpeed(int value)
{
    solverSpeed->setText(QString("solver speed: x%1").arg(value));
    rowsolver->waitTimeMilliSec = RowSolver::WAIT_TIME_MILLI_SEC_DEFAULT/value;
}

void ContainerWidget::solveBoardRows(bool b)
{
    clearButton->setEnabled(false);
    addButton->setEnabled(false);
    solveButton->setEnabled(false);
    solveRowButton->setEnabled(false);
    solutionsLabel->setText("solving row by row...");

    QList<Piece*> freePieces;
    for(QList<Piece*>::iterator it=pieces.begin(); it!=pieces.end(); it+=1) {
        if((*it)->isUsed() == false) {
            freePieces.append(*it);
        } else {
            if((*it)->getPattern()[0] == '1') {
                (*it)->setVersion('A');
            } else {
                (*it)->setVersion('B');
            }
            rowsolver->placePieceOnBoard(*it, (*it)->getPattern()[2].digitValue(), (*it)->getPattern()[3].digitValue(), (*it)->getPattern()[1].digitValue());
        }
    }

    rowsolver->setPieces(freePieces);

    rowsolver->moveToThread(&workerThread);
    connect(this, SIGNAL(startWork()), rowsolver, SLOT(start()));
    connect(rowsolver, SIGNAL(workDone()), this, SLOT(solverFinished()));
    workerThread.start();
    emit startWork();
}

void ContainerWidget::solverFinished()
{
    workerThread.quit();
    workerThread.wait(50);

    solutionsLabel->setText("click solve to find solutions");
    solveButton->setEnabled(true);
    solveRowButton->setEnabled(true);
    clearButton->setEnabled(true);
    addButton->setEnabled(true);
}

void ContainerWidget::populateBoard()
{
    QChar version;
    for(int i=0; i<10; ++i) {
        pieces.at(i)->setPattern(solutions->at(currentSolution).patterns[i]);
        if(pieces.at(i)->getPattern()[0] == '1') version = 'A';
        else version = 'B';
        this->board->changeSite(pieces.at(i), version, pieces.at(i)->getPattern()[2].digitValue(), pieces.at(i)->getPattern()[3].digitValue(), pieces.at(i)->getPattern()[1].digitValue(), false);
    }
}
