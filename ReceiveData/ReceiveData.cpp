#include <lsl_cpp.h>
#include <iostream>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xinclude.h>
#include <libxml/xmlIO.h>
#include <armadillo>
#include <thread>
#include <string>
#include <mutex>
#include <armadillo>
#include <chrono>
#include <functional>
#include <atomic>



using namespace std;
using namespace arma;

#define NUM_THREADS  1

#define NUM_ROWS 15

#define SUB_MAT_ROWS 1 

std::thread tds[NUM_THREADS];

//typedef std::vector<zeros<mat>(SUB_MAT_ROWS,135)> stdvec[NUM_THREADS;

//below class file is used for shared resources


int numOfTimes = 1;
int extractStartRow =0; 
int extractEndRow = 0;
int mainMatrixFilled =0;
int matrixRow =0;

mat mainBuffer = zeros<mat>(NUM_ROWS,135);

class LogFile {

//adding mutex for matrix and file stream f

    std::mutex m_mutex;
   // std::lock_guard<std::mutex> locker(m_mutex);

//    mat mainBuffer = zeros<mat>(NUM_ROWS,135); 
     //below two sub buffers has to be dynamically defined based on the number of threads.   
        mat subBuf1 = zeros<mat>(SUB_MAT_ROWS,135);
        mat subBuf2 = zeros<mat>(SUB_MAT_ROWS,135);
                  
public:

//Opening the file in the constructor, this will be created every time when program runs, already existing file will be deleted.

//    LogFile()
//    {
//f.open("log.txt");
//   }

 //This logic is used to update the particular cell, based on the row and column number. with the value provided by the thread.

   void fillMainBuffer(int row, int col, float val){

    std::lock_guard<std::mutex> locker(m_mutex);

//Updating the matrix with a particular value provided by thread.

   mainBuffer(row,col) = val;
   mainMatrixFilled++;
//   cout << "\n INFO:: Mainbuffer filled data is:: " << mainBuffer << endl;

    }


   void fillSubMatrix(int threadId)
{

cout << "\n INFO:: Mainbuffer filled data is:: " << mainBuffer << endl;
while(matrixRow>0) 
{
matrixRow--;
std::this_thread::sleep_for(std::chrono::milliseconds(60));
cout << "INFO::Filling the sub matrixs::\n " << endl; 

extractEndRow = numOfTimes * SUB_MAT_ROWS;
extractStartRow = extractEndRow - SUB_MAT_ROWS;

cout << "\n Info::extractEndRow: " << extractEndRow << endl;
cout << "\n Info::extractStartRow::" << extractStartRow << endl;

//clearing the matrix after 5 mins or before filling next new chunk of data to the sub buffers.
 if (numOfTimes > 1 && mainMatrixFilled >0) 
{
  std::lock_guard<std::mutex> locker(m_mutex);
  if (threadId=0)
  subBuf1.shed_rows(0,SUB_MAT_ROWS-1);
  else
  subBuf2.shed_rows(0,SUB_MAT_ROWS-1);

 // cout << "INFO:: AFTER Deleted the existing SUBBUFFER1 data :: \n" << subBuf1[0] << endl;
  //cout << "INFO:: AFTER Deleted the existing SUBBUFFER2 data :: \n" << subBuf2[10] << endl;

} 

cout << "INFO::Now filling the subbuffer data..\n" << endl;
std::lock_guard<std::mutex> locker(m_mutex);   
if (threadId=0)
subBuf1 = mainBuffer(span(extractStartRow,extractEndRow), span(0,134) );
else
subBuf2 = mainBuffer(span(extractStartRow,extractEndRow), span(0,134) );

//cout << "INFO:: SUBbUFFER1 data is :: \n " << subBuf1 << endl;
cout << "INFO:: SubBuffer2 data is :: \n " << subBuf2 << endl;


  ++numOfTimes;
cout <<"\n INFO::Number of times the sub buffers filled are:::" << numOfTimes << endl;
}
}


};
/**
 * Example program that demonstrates how to resolve a specific stream on the lab network and how to connect to it in order to receive data.
 *
 */

void get_element_value(xmlNode* a_node,std::string& key,std::string& value) {
	xmlNode *cur_node = NULL;
	for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			std::stringstream ss;
			ss << cur_node->name;
			if( ss.str() == key){
				ss.str("");
				ss << xmlNodeGetContent( (const xmlNode *)(cur_node));
				value=ss.str();
				ss.str("");
				return;
			}
		}
		get_element_value(cur_node->children,key,value);
	}
}


//Below function is used to fill the sub buffers from the main buffer. End users have the capabiltiy to these sub buffers only, each sub buffer will get the chunks of incremental 2048 rows from main buffer.
//This 2048 rows chunk will be shredded out after 5 mins and gets the new set of 2048 rows from main buffer.
//each thread will use its own sub buffer, each thread will serve one end user.

//Below function is used to get the 2048 chunks of data from main matrix to the sub matrices owned by each thread, that is serving the end user.
void function_1(LogFile& log, int &n ) {
// std::this_thread::sleep_for(std::chrono::milliseconds(10000));    
 log.fillSubMatrix(n);
//making the thread sleep for 10000 milli seconds.

     //std::this_thread::sleep_for(std::chrono::milliseconds(6666660));

}

int main(int argc, char* argv[]) {
	string field, value;

        LogFile log;
        
	if (argc != 3) {
		cout << "This connects to a stream which has a particular value for a given field and receives data." << endl;
		cout << "Please enter a field name and the desired value (e.g. \"type EEG\" (without the quotes)):" << endl;
		cin >> field >> value;
	} else {
		field = argv[1];
		value = argv[2];
	}
	try {
		// resolve the stream of interet
		cout << "Now resolving streams..." << endl;
		vector<lsl::stream_info> results = lsl::resolve_stream(field,value);
		cout << "Here is what was resolved: " << endl;
		cout << results[0].as_xml() << endl;
		// make an inlet to get data from it
		// start receiving & displaying the data
		/**************************************** XML PARSING **************************/
		std::string xml=results[0].as_xml();
		xmlDocPtr doc;
		xmlNode *root_element = NULL;
		doc = xmlReadMemory(xml.c_str(), xml.length(), "include.xml", NULL, 0);
		if (doc == NULL) {
			std::cout << "xml parsing error" << std::endl;
			throw "not a valid xml";
		}
		root_element = xmlDocGetRootElement(doc);
		//print_element_names(root_element);
		std::string key="channel_count";
		std::string val="";
		get_element_value(root_element,key,val);
		/*free the document */
		//xmlFreeDoc(doc);
		xmlFreeNode(root_element);
		cout << "Now creating the inlet..." << endl;
		lsl::stream_inlet inlet(results[0]);
		std::cout << "press any key to start pulling samples" << std::endl;
		char c=getchar();
		std::stringstream ss;
		ss << val;
		unsigned num_channels=1;
		ss >> num_channels;
		std::cout << "num channels " << num_channels << std::endl;
		cout << "Now pulling samples..." << endl;

            //    int matrixRow =0;
	

	       int spawnChild = 1;

               while (matrixRow <= 12) {
                        
                         cout << "Creating the matrix for row:: " << matrixRow << endl; 
                         //incrementing the matrix row for each and every loop.
                           matrixRow++;
			// get the sample
			float sample[num_channels];
			double ts = inlet.pull_sample(sample,num_channels);
			// display
			for (unsigned c=0;c<num_channels;c++){
				//cout << sample[c];
                               //writing to master matrix from the setMasterMatrix function

                               log.fillMainBuffer(matrixRow,c,sample[c]);
                               cout << "\n Row::" << matrixRow << " Column :" << c << " Value::" << sample[c] << endl;
                               //B(matrixRow,c) = sample[c];
				//if(c!=(num_channels-1))
					//cout  << B(matrixRow ;
			}
		      // cout << B(matrixRow,span(0,134)) << endl;



		}

               cout << "\n INFO:: Mainbuffer filled data is:: " << mainBuffer << endl;


                for (int j=0; j<NUM_THREADS; j++)

                 {  

                  //Creating multiple threads, and each thread is calling function_1 and passing two values as reference log file class object and //number of rows.
                   tds[j] = std::thread(function_1, std::ref(log),std::ref(j) );               

                 } 

                 //Joining all the threads back.

                for ( int t=0; t<NUM_THREADS; t++)

                  {  

                   tds[t].join();

                  } 

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
