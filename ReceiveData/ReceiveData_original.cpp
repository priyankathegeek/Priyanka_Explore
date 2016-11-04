#include <lsl_cpp.h>
#include <iostream>
#include <sstream>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xinclude.h>
#include <libxml/xmlIO.h>
using namespace std;
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

int main(int argc, char* argv[]) {
	string field, value;
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
		while (true) {
			// get the sample
			float sample[num_channels];
			double ts = inlet.pull_sample(sample,num_channels);
			// display
			for (unsigned c=0;c<num_channels;c++){
				cout << sample[c];
				if(c!=(num_channels-1))
					cout  << "\t" ;
			}
			cout << endl;
		}
	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
