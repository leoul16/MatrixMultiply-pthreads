#include "mmultiply.h"

/*
 * Leoul Yiheyis
 * 905707550
 * leoul16@vt.edu
 * ECE 3574
 * Homework 6
*/



/*
    Null pointer thread initializing function. Populates a matrix with data from an input file.
*/
void* popMatrix(void *arg){

    bool ok;

    struct matrix* newMatrix = (struct matrix*)arg;

    QFile data(newMatrix->datafile);
    if(!data.open(QIODevice::ReadOnly | QIODevice::Text)) {

        qDebug() << "Please enter a valid data file.";// << endl;
        exit(1);
    }

    QTextStream datastream(&data);

    while(!datastream.atEnd()) {

        QString line = datastream.readLine();
        //istrings
        QStringList vals = line.split(QRegExp("(\\s|\\s+|\\t)"));

        QList< int > col;

        for (int i = 0; i < vals.size(); i++) {
            int val = vals[i].toInt(&ok);

            if (!ok) {

                qDebug() << "The input data was not formatted to be integers.";// << endl;

                exit(1);
            }

            col.append(val);
        }

        newMatrix -> col = vals.size();
        (newMatrix -> row)++;
        newMatrix -> data.append(col);
    }

    return 0;
}

// Initialize text stream to temporary file for all threads to access.
QTextStream tempOut (&tempOutFile);

/*
    Null pointer thread initializer function. Finds the value for one element in new Matrix C.
*/
void* mmultiply(void *arg) {

    // Must be long because casting (void *) to int throws an error
    long idx = (long)arg;

    int lim = matrixB.col;

    // Figure out what row and collumn of Matrix C we are working on
    int row = idx / lim;
    int col = idx % lim;

    matrixC[row][col] = 0; // to bypass seg fault error

    // Calculate new value
    for (int i = 0; i < matrixA.col; i++)
        matrixC[row][col] += matrixA.data[row][i] * matrixB.data[i][col];

    // Write it to temporary file "temp.txt"
    tempOut << "C " << row+1 << "," << col+1 << "=" << matrixC[row][col] << endl;

    return 0;
}

/*
    Used for testing. Prints out a matrix to the terminal.
*/
void printMatrix(struct matrix *matrix) {

    QList < QList < int > > datarray = matrix->data;

    int x = 0;

    for (int i = 0; i < datarray.size(); i++) {

        for (int j = 0; j < datarray[i].size(); j++) {

            if (i > x)
                qDebug() << endl;

            qDebug() << datarray[i][j] << " ";

            x = i;
        }
    }
}

/*
    Used for testing. Prints out 2D QList as a matrix onto terminal.
*/
void printMatrix(QList < QList < int > > outputArray) {

    int x = 0;

    for (int i = 0; i < outputArray.size(); i++) {

        for (int j = 0; j < outputArray[i].size(); j++) {

            if (i > x)
                qDebug() << endl;

            qDebug() << outputArray[i][j] << " ";

            x = i;
        }
    }
}

/*
    Returns an matrix of size row X col full of all zeroes.
*/
QList< QList < int > > emptyMatrix(int col, int row) {

    // 2-D array to be returned
    QList < QList < int > > output;

    for (int i = 0; i < col; i++) {

        QList<int> zeros;

        output.append(zeros);

        for (int j = 0; j < row; j++)
            output[i].append(0);
    }

    return output;
}


/*
    Main function for the program.
*/
int main(int argc, char* argv[]) {

    // Checking for correct command line input format
    if (argc != 4) {
        qDebug() << "Error: Please run the program like so ./Leoul_Yiheyis_hw6 input-file1 input-file2 output-file.";
        exit(1);
    }

    // 2 Threads, one for each input file
    pthread_t readThreads[2];

    // Extract input file name and add to appropriate struct matrix
    QString matAfile = argv[1];
    matrixA.datafile = matAfile;

    QString matBfile = argv[2];
    matrixB.datafile = matBfile;

    QString outfile  = argv[3]; // Output file

    // Create a thread to read data into matrix A & B
    pthread_create(&readThreads[0], NULL, popMatrix, (void *) &matrixA);
    pthread_create(&readThreads[1], NULL, popMatrix, (void *) &matrixB);

    // Join each thread when done
    for (int i = 0; i < 2; i++)
        pthread_join(readThreads[i], NULL);

    // Checking for valid dimensions
    if (matrixA.col != matrixB.row) {
        qDebug() << "Error: Matrix inner dimension mismatch.";
        exit(1);
    }

    // Dimensions for new Matrix C
    int matrixCrow = matrixA.row;
    int matrixCcol = matrixB.col;

    // Populates Matrix C with all zeroes
    matrixC = emptyMatrix(matrixCrow, matrixCcol);

    // Array of threads for each element in Matrix C to be calculated
    pthread_t matrixCThreads[matrixCrow * matrixCcol];

    // Temporary file for each thread to write value to
    tempOutFile.open(QIODevice::WriteOnly);

    // Creates a thread to calculate each value in new Matrix C
    for (int i = 0; i < matrixCrow*matrixCcol; i++)
        pthread_create(&matrixCThreads[i], NULL, mmultiply, (void*) i);


    // For loop to join all created threads
    for (int i = 0; i < matrixCrow*matrixCcol; i++)
        pthread_join(matrixCThreads[i], NULL);

    // Open ouput file
    QFile outputFile(outfile);

    // Check if output file can be opened
    if (!outputFile.open(QIODevice::WriteOnly))
        qDebug() << "Error opening output file. Please provide valid file to be written to.";

    QTextStream out(&outputFile);

    int x = 0; // Varaible to check whether to move to new line

    // Print all values to output file
    for (int i = 0; i < matrixC.size(); i++) {

        for (int j = 0; j < matrixC[i].size(); j++) {

            if (i > x)
                out << endl;

            out << matrixC[i][j] << "\t";

            x = i;
        }
    }

    out << endl; // Print one last end line

    pthread_exit(NULL); // properly exit pthread
}
