#include <thread>

#include <fstream>

#include <iostream>

#include <string>

#include <thread>

#include <fstream>

#include <iostream>

#include <string>

#include <mutex>

#include <armadillo>

#include <chrono>

#include <functional>

#include <atomic>



using namespace std;

using namespace arma;

#define NUM_THREADS    4

#define NUM_ROWS 3

#define NUM_COLUMNS 4



int ROW_NUM =0;



//below class file is used for shared resources

class LogFile {



//adding mutex for matrix and file stream f

    std::mutex m_mutex;

int thread_id;

//try to define this dynamically...using ones function.

 mat A = {{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
//    mat A;
 //   A.zeros(NUM_ROWS,NUM_COLUMNS);

 fstream f;

public:

//Opening the file in the constructor, this will be created every time when program runs, already existing file will be deleted.

    LogFile()

    {

   // f.open("log.txt",std::fstream::app);

     f.open("log.txt");

    }

 //This logic is used to update the particular cell, based on the row and column number. with the value provided by the thread. 

   void update_row(int row, int col, int val, int id){

    std::lock_guard<std::mutex> locker(m_mutex);

    int orig_val= A(row,col);

//Printing the value from the matrix before the thread updates the value in matrix.

     f << "\n original value from Matrix A is::" << orig_val << endl;

//Updating the matrix with a particular value provided by thread.

   A(row,col) = val;

//Printing the current state of the matrix to the log file.

     f << "\n From Thread:: " << id << " row:: " << row << " Col:: " << col << endl;

     f << "\n matrix  From:: " << id << " is \n " << A << endl;





    }



};

//this is the function called by thread t1 to write into log file

void function_1(LogFile& log, int &n ) {

//decrementing the n value that is the row number, as this is passed by reference the value of n will be decremented in main //program.

   int t=n;

   --n;

   for (int i=0; i<NUM_COLUMNS; i++)


{

    //From each thread below function is called, for each thread (t) all the columns (i) is looped in the matrix.

       log.update_row(t,i,i-10,t);

     }

//making the thread sleep for 10 milli seconds.

     std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

//this is the main program used to create multiple threads in a loop.

int main() {

//creating the object for the log file class.

LogFile log;

int n = NUM_ROWS;

//Defining the ts array with thread instances.

std::thread ts[NUM_THREADS];



 for (int j=0; j<NUM_THREADS; j++)

{

//Creating multiple threads, and each thread is calling function_1 and passing two values as reference log file class object and //number of rows.

   ts[j] = std::thread(function_1, std::ref(log), std::ref(n));



}



//Joining all the threads back.

 for ( int t=0; t<NUM_THREADS; t++)

{

  ts[t].join();

}



 return 0;




}
