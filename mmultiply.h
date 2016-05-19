#ifndef MMULTIPLY
#define MMULTIPLY

/*
 * Leoul Yiheyis
 * 905707550
 * leoul16@vt.edu
 * ECE 3574
 * Homework 6
*/

#include <QStringList>
#include <QList>
#include <pthread.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>


                   // STRUCT DECLERATION
struct matrix {
    // 2D array to hold data
    QList < QList<int> > data;

    // To keep track of the number of row and collumns in said matrix
    int row, col;

    // Name of the file that will provide the data for this matrix
    QString datafile;
};


//--------------------------------------------------------------------------------------------------

                    // GLOBAL DECLERATIONS

// Made global so each calculating thread can easily access it
QFile tempOutFile("temp.txt");

// Matrices that will be multiplied together
struct matrix matrixA;
struct matrix matrixB;

// 2D array that will hold the values for output matrix C
QList < QList < int > > matrixC;


//--------------------------------------------------------------------------------------------------

                    // FUNTION DECLERATIONS

/*
    Null pointer thread initializing function. Populates a matrix with data from an input file.
*/
void* popMatric(void *arg);

/*
    Null pointer thread initializer function. Finds the value for one element in new Matrix C.
*/
void* mmultiply(void *arg);

/*
    Used for testing. Prints out a matrix to the terminal.
*/
void printMatrix(struct matrix *matrix);

/*
    Used for testing. Prints out 2D QList as a matrix onto terminal.
*/
void printMatrix(QList < QList < int > > outputArray);

/*
    Returns an matrix of size row X col full of all zeroes.
*/
QList < QList < int > > emptyMatrix(int col, int row);

#endif // MMULTIPLY

