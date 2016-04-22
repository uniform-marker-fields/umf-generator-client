#include <ctype.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "generate.h"
#include "MFSGrayscale.h"
#include "MFHGrayscale.h"
#include "MFSColor.h"
#include "MFHColor.h"
#include "MFSImage.h"

#include <istream>
#include <ostream>
#include <boost/asio.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using boost::asio::ip::tcp;
using namespace std;

int experiment_flag = FALSE;
	
	//!!!EXPERIMENT
	char tmp_xxx[1024] = "";
	//!!!

/**
 * All the info from the XML file is copied to this struct
 */
struct markerInfo
{
	unsigned int gid;
	unsigned int width;
	unsigned int height;
	unsigned int kernel;
	unsigned long kernel_type;
	string data;
	unsigned int type;
	string colors;
	double runtime;
	
	unsigned int img_id;
	string img_alg;
	string img_server;
	string img_path;
	
	unsigned int module_type;
	
	unsigned int threshold_equal;
	string cost_neighbors;
	float cost_similarity;
	
	string img_conv;
};

/**
 * Exceptions for this file
 */
class GenerateException: public exception
{
	const char* msg;
	
	virtual const char* what() const throw()
	{
		return msg;
	}
	
	public:
	GenerateException(const char *msg): msg(msg)
	{
	}
};

/**
 * From a marker info returns the instance of the correct class
 * 		if there is no proper class throws an exception
 * 		if the object can not be created an exception is thrown
 */
MarkerField *getMarkerField(markerInfo m)
{	
	/*
	 * Torus is not implemented
	 */
	if ((TYPE_TORUS_BIT & m.type) == TYPE_TORUS_BIT)
	{
		throw GenerateException("Torus is not implemented");
	}
	else
	{
		if (m.module_type == MODULE_TYPE_HEXA)
		{
			/*
			 * Image marker field
			 */
			if (m.img_id > 0)
			{
				throw GenerateException("Hexa image marker fields are not supported, yet!");
			}
			/*
			 * Coloured marker field
			 */
			else if ((TYPE_COLOR_BIT & m.type) == TYPE_COLOR_BIT)
			{
				return new MFHColor(m.width, m.height, m.kernel, m.kernel_type, m.type, m.threshold_equal, m.cost_neighbors.c_str(), m.cost_similarity, m.colors.c_str(), m.data);
			}
			/*
			 * Grayscale marker field
			 */
			else
			{
				return new MFHGrayscale(m.width, m.height, m.kernel, m.kernel_type, m.type, m.threshold_equal, m.cost_neighbors.c_str(), m.cost_similarity, m.data);
			}
		}
		else
		{
			/*
			 * Image marker field
			 */
			if (m.img_id > 0)
			{
				string imageString;
				download(m.img_server.c_str(), m.img_path.c_str(), imageString);
				
				return new MFSImage(m.width, m.height, m.kernel, m.kernel_type, m.type, m.threshold_equal, m.cost_neighbors.c_str(), m.cost_similarity, m.colors.c_str(), m.img_alg.c_str(), m.data, imageString, m.img_conv.c_str());
			}
			/*
			 * Coloured marker field
			 */
			else if ((TYPE_COLOR_BIT & m.type) == TYPE_COLOR_BIT)
			{
				return new MFSColor(m.width, m.height, m.kernel, m.kernel_type, m.type, m.threshold_equal, m.cost_neighbors.c_str(), m.cost_similarity, m.colors.c_str(), m.data);
			}
			/*
			 * Grayscale marker field
			 */
			else
			{
				return new MFSGrayscale(m.width, m.height, m.kernel, m.kernel_type, m.type, m.threshold_equal, m.cost_neighbors.c_str(), m.cost_similarity, m.data);
			}
		}
	}
}

/**
 * Very light weight URL safe converter
 */
 string makeURLSafe(string s)
 {
	 string r = "";
	 for (unsigned int i = 0; i < s.size(); i++)
	 {
		switch (s[i]) {
			case '#':
				r += "%23";
				break;
			case ',':
				r += "%2C";
				break;
			case ':':
				r += "%3A";
				break;
			case ';':
				r += "%3B";
				break;
			default:
				r += s[i];
		}
	 }
	 return r;
 }


/**
 * Prints info about the marker
 */
void printMarkerInfo(markerInfo m)
{
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
	cout << "GID: " << m.gid << endl;
	
	cout << "MODULE_TYPE: ";
	switch(m.module_type)
	{
		case MODULE_TYPE_SQUARE:
			cout << "square";
			break;
		case MODULE_TYPE_HEXA:
			cout << "hexa";
			break;
		default:
			cout << "unknowen";
			break;
	}
	cout << endl;
	
	cout << "SIZE: " << m.width << "x" << m.height << endl;
	cout << "KERNEL SIZE: " << m.kernel << endl;
	
	cout << "KERNEL TYPE: ";
	unsigned long t = m.kernel_type;
	for (unsigned int i = 0; i < (m.kernel-1)*m.kernel/2; i++)
	{
		cout << (1 & t ? "1" : "0");
		t >>= 1;
	}
	cout << endl;
	
	cout << "DATA: " << m.data.substr(0,64) << (m.data.size() > 64 ? "..." : "") << endl;
	cout << ((TYPE_TORUS_BIT & m.type) == TYPE_TORUS_BIT ? "TORUS" : "NORMAL" ) << " " << ((TYPE_COLOR_BIT & m.type) == TYPE_COLOR_BIT ? "COLOR" : "GRAYSCALE" ) << endl;
	
	cout << "RANGE: " << MarkerField::getRangeFromType(m.type) << endl;
	cout << "COLORS: " << m.colors << endl;
	cout << "RUNTIME: " << m.runtime << endl;
	
	cout << "THRESHOLD FOR EQUAL: " << m.threshold_equal << endl;
	cout << "COST FOR NEIGHBORS: " << m.cost_neighbors << endl;
	
	cout << "IMG ID: " << m.img_id << endl;
	cout << "IMG ALGORITHM: " << m.img_alg << endl;
	cout << "IMG SERVER: " << m.img_server << endl;
	cout << "IMG PATH: " << m.img_path << endl;
	cout << "IMG CONVOLUTION MATRIX: " << m.img_conv << endl;
	cout << "COST FOR SIMILARITY: " << m.cost_similarity << endl;
	cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
}

/**
 * DOWNLOAD
 */
int download(const char *server, const char *path, string &data)
{
	try
	{
		boost::asio::io_service io_service;

		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(server, "http");
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;

		// Try each endpoint until we successfully establish a connection.
		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}
		if (error)
		{
			throw boost::system::system_error(error);
		}

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		ostream request_stream(&request);
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << server << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");

		// Check that response is OK.
		istream response_stream(&response);
		string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		string status_message;
		getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			cerr << "Invalid response\n";
			return 1;
		}
		if (status_code != 200)
		{
			cerr << "Download: Response returned with status code " << status_code << "\n";
			return 1;
		}

		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Process the response headers.
		string header;
		while (getline(response_stream, header) && header != "\r")
		{
			//cout << header << "\n";
		}
		//cout << "\n";
		
		
		ostringstream xmlStream;

		// Write whatever content we already have to output.
		if (response.size() > 0)
		{
			//cout << &response;
			xmlStream << &response;
		}

		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
		{
			xmlStream << &response;
		}
		if (error != boost::asio::error::eof)
		{
			throw boost::system::system_error(error);
		}
		
		data = xmlStream.str();
	
		//if (xmlString.length() > 0) {
		//	cout << xmlString << endl;
		//}
			
		return 0;
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
		return 1;
	}
}

/**
 * MAIN LOOP
 */
int main (int argc, char **argv)
{
	/*
	 * Initialization
	 */
	string xmlString = "";
	clock_t tStart = clock();
	
		using boost::property_tree::ptree;
		ptree pt;
	
	// default values...
	unsigned int timeout = DEFAULT_TIMEOUT_SECONDS;
	int debug_flag = FALSE;
	int show_flag = FALSE;
	int help_flag = FALSE;
	int send_flag = TRUE;
	//char image_server[1024];
	//char image_path[1024];
	char server_url[1024] = SERVER_URL;
	char get_path[1024] = GETMAP_PATH;
	char post_path[1024] = POSTMAP_PATH;
	int c;

	/* READING ARGUMENTS*/
	while (1)
	{
		static struct option long_options[] =
		{
			// These options set a flag.
			{"debug", no_argument,       &debug_flag, TRUE},
			{"show", no_argument,       &show_flag, TRUE},
			{"not-send", no_argument,       &send_flag, FALSE},
			{"experiment", no_argument,       &experiment_flag, TRUE},
			// These options don't set a flag.
			{"help", no_argument,       0, 'h'},
			{"server-url", required_argument,       0, 's'},
			{"get-path", required_argument,       0, 'g'},
			{"post-path", required_argument,       0, 'p'},
			{"xxx", required_argument, 0, 'x'},
			//{"image-server",     required_argument,       0, 's'},
			//{"image-path",  required_argument,       0, 'p'},
			{0, 0, 0, 0}
		};
		
		// getopt_long stores the option index here.
		int option_index = 0;

		c = getopt_long (argc, argv, "h", long_options, &option_index);

		// Detect the end of the options.
		if (c == -1) break;

		switch (c)
		{
			case 0:
			// If this option set a flag, do nothing else now.
				if (long_options[option_index].flag != 0)
					break;
				printf ("option %s", long_options[option_index].name);
				if (optarg)
					printf (" with arg %s", optarg);
				printf ("\n");
				break;
				
			case 's':
				strcpy(server_url, optarg);
				break;
				
			case 'g':
				strcpy(get_path, optarg);
				break;
				
			case 'p':
				strcpy(post_path, optarg);
				break;
				
			case 'x':
				strcpy(tmp_xxx, optarg);
				break;

			//case 's':
			//	strcpy(image_server, optarg);
			//	break;

			//case 'p':
			//	strcpy(image_path, optarg);
			//	break;
			case 'h':
				help_flag = TRUE;
				break;

			case '?':
				// getopt_long already printed an error message.
				break;

			default:
				abort ();
		}
	}

	if (optind < argc)
	{
		timeout = atoi(argv[optind]);
		if (timeout <= 0) timeout = DEFAULT_TIMEOUT_SECONDS;
	}
	
	timeout = timeout * 95 / 100;
	
	/* PRINTING HELP */
	if (help_flag)
	{
		cout << "usage: generate [options] [runtime]" << endl << endl;
		cout << "  runtime is the runtime of the program in seconds" << endl << endl;
		cout << "  options:" << endl;
		cout << "    --debug              print info about the process" << endl;
		cout << "    --show               visualisation of the results" << endl;
		cout << "    --not-send           if flag is set the result is not sent back to the server" << endl;
		cout << "    --experiment         experimental flag" << endl;
		cout << "    --help               prints help" << endl;
		cout << "    --server-url=<url>   you can set the server to use" << endl;
		cout << "    --get-path=<path>    path on the server, where the map is received" << endl;
		cout << "    --post-path=<path>   path on the server, where the result is posted" << endl;
		cout << endl;
		return 0;
	}
	
	// experiment URL
	if (experiment_flag)
	{
		strcat(get_path, EXPERIMENT_FLAG);
	}
	
	/* TRYING TO GET A USABLE MAP */
	try
	{
		do
		{
			download(server_url, get_path, xmlString);
			
			istringstream xmlIStream(xmlString);
			
			// populate tree structure pt
			read_xml(xmlIStream, pt);
			
			if (pt.get_child("markers").empty())
			{
				if ((double)(clock() - tStart)/CLOCKS_PER_SEC + 2*DEFAULT_SLEEP_TIME_SECONDS > timeout)
				{
					if (debug_flag)
					{
						cout << "No map, ending..." << endl;
					}
					return 0;
				}
				if (debug_flag)
				{
					cout << "No map, sleeping..." << endl;
				}
				sleep(DEFAULT_SLEEP_TIME_SECONDS);
			}
		} while (pt.get_child("markers").empty());
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
		return 1;
	}

    /* LOAD INFO FROM XML TO markerInfo STRUCT */
    markerInfo m;
    BOOST_FOREACH( ptree::value_type const& v, pt.get_child("markers") )
    {
        if ( v.first == "marker" )
        {
            m.gid = v.second.get<unsigned>("gid");
            m.width = v.second.get<unsigned>("width");
            m.height = v.second.get<unsigned>("height");
            m.kernel = v.second.get<unsigned>("kernel");
            m.data = v.second.get<string>("data");
            m.type = v.second.get<unsigned>("type");
            m.colors = v.second.get<string>("colors");
            boost::optional< double > is_runtime = v.second.get_optional<double>("runtime");
            if ( !is_runtime )
            {
				m.runtime = 0.0;
			}
			else
			{
				m.runtime = v.second.get<double>("runtime");
			}
			
            boost::optional< unsigned long > is_kernel_type = v.second.get_optional<unsigned long>("kernel_type");
            if ( !is_kernel_type )
            {
				m.kernel_type = 0;
			} else {
				m.kernel_type = v.second.get<unsigned long>("kernel_type");
			}
			
            boost::optional< unsigned int > is_img_id = v.second.get_optional<unsigned int>("img_id");
            if ( !is_img_id )
            {
				m.img_id = 0;
				m.img_alg = "";
				m.img_server = "";
				m.img_path = "";
			}
			else
			{
				m.img_id = v.second.get<unsigned int>("img_id");
				m.img_alg = v.second.get<string>("img_alg");
				m.img_server = v.second.get<string>("img_server");
				m.img_path = v.second.get<string>("img_path");
			}
			
            boost::optional< unsigned int > is_module_type = v.second.get_optional<unsigned int>("module_type");
            if ( !is_module_type )
            {
				m.module_type = MODULE_TYPE_SQUARE;
			}
			else
			{
				m.module_type = v.second.get<unsigned int>("module_type");
			}
			
			
            boost::optional< unsigned int > is_threshold_equal = v.second.get_optional<unsigned int>("threshold_equal");
            if ( !is_threshold_equal )
            {
				m.threshold_equal = 0;
				m.cost_neighbors = "8.0,8.0:0;10.0,0.0:128";
				m.cost_similarity = 1.0;
				m.img_conv = "1";
			}
			else
			{
				m.threshold_equal = v.second.get<unsigned int>("threshold_equal");
				m.cost_neighbors = v.second.get<string>("cost_neighbors");
				m.cost_similarity = v.second.get<float>("cost_similarity");
				m.img_conv = v.second.get<string>("img_conv");
			}
        }
    }
	
	string post_data = "";
	
	if (debug_flag)
	{
		printMarkerInfo(m);
		cout << "initializing... " << endl;
	}
	
	/* GET THE INSTANCE OF THE CORRECT CLASS */
	MarkerField *field;
	try
	{
		field = getMarkerField(m);
	}
	catch (exception& e)
	{
		cerr << "Exception: " << e.what() << "\n";
		return 1;
	}
	
	if (debug_flag)
	{
		cout << "USING: " << field->getClassName() << endl;
	}
	
	/* LOOP TO MAKE THE COMPUTATIONS */
	//			post_data = "data=";
	//			MFSGrayscale gray(m.data, m.width, m.height, m.kernel, getTypeRange(m.type));
	//			gray.setKernelType(m.kernel_type);
	//			gray.firstCheck();
	double oneRunTme = (double)(clock() - tStart)/CLOCKS_PER_SEC;
	unsigned int conflictCount = field->getConflictCount();
	if (debug_flag)
	{
		field->printStatistics();
	}	
	if (show_flag)
	{
		field->show();
		//getc(stdin);
	}
	while ((double)(clock() - tStart)/CLOCKS_PER_SEC + 2 * oneRunTme < timeout)
	{
		if (debug_flag)
		{
			cout << "computing... " << "(" << ((double)(clock() - tStart)/CLOCKS_PER_SEC + oneRunTme) << " s)" << endl;
		}
		field->mutate();
		if (debug_flag)
		{
			field->printStatistics();
		}

		if ((double)(clock() - tStart)/CLOCKS_PER_SEC > DEFAULT_END_TIME * timeout && conflictCount > field->getConflictCount()) {
			break;
		}
		if (show_flag) {
			field->show();
		}
	}
	//			post_data += gray.getData();
			
	//			cost = gray.getCost();
	
	//cout << field->getReadableData() << endl;
	
	post_data += "data=";
	post_data += field->getReadableData();
	post_data += "&colors=";
	post_data += makeURLSafe(field->getColorsText());
	
	post_data += "&rt=" + boost::lexical_cast<string>( m.runtime + ((double)(clock() - tStart)/CLOCKS_PER_SEC) );
	post_data += "&c=" + boost::lexical_cast<string>( field->getCost() );
	
	post_data += "&c_n=";
	post_data += makeURLSafe(m.cost_neighbors);
	post_data += "&c_s=" + boost::lexical_cast<string>( m.cost_similarity );
	post_data += "&img_conv=";
	post_data += makeURLSafe(m.img_conv);
	
	
	/* SENDING THE RESULTS BACK */
	if (send_flag)
	{
		if (debug_flag)
		{
			cout << "sending map..." << endl;
		}
		try
		{
			boost::asio::io_service io_service;

			// Get a list of endpoints corresponding to the server name.
			tcp::resolver resolver(io_service);
			tcp::resolver::query query(server_url, "http");
			tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			tcp::resolver::iterator end;

			// Try each endpoint until we successfully establish a connection.
			tcp::socket socket(io_service);
			boost::system::error_code error = boost::asio::error::host_not_found;
			while (error && endpoint_iterator != end) {
				socket.close();
				socket.connect(*endpoint_iterator++, error);
			}
			if (error) {
				throw boost::system::system_error(error);
			}

			// Form the request. We specify the "Connection: close" header so that the
			// server will close the socket after transmitting the response. This will
			// allow us to treat all data up until the EOF as the content.
			boost::asio::streambuf request;
			ostream request_stream(&request);
			request_stream << "POST " << post_path << "?id=" << m.gid << "&t=" << field->getType() << "&w=" << field->getWidth() << "&h=" << field->getHeight() << "&k=" << field->getKernelSize() << "&k_t=" << field->getKernelType();
			request_stream << "&img_id=" << m.img_id << "&img_alg=" << field->getImgAlgText() << "&m_t=" << m.module_type << "&t_e=" << m.threshold_equal << " HTTP/1.0\r\n";
			request_stream << "Host: " << server_url << "\r\n";
			request_stream << "Accept: */*\r\n";
			request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";
			request_stream << "Content-Length: " << post_data.size() << "\r\n"; //+7+(8*getTypeRange(m.type)) << "\r\n";
			request_stream << "Connection: close\r\n\r\n";
			
			request_stream << post_data; // << "&colors=" << m.colors;

			// Send the request.
			boost::asio::write(socket, request);

			// Read the response status line.
			boost::asio::streambuf response;
			boost::asio::read_until(socket, response, "\r\n");

			// Check that response is OK.
			istream response_stream(&response);
			string http_version;
			response_stream >> http_version;
			unsigned int status_code;
			response_stream >> status_code;
			string status_message;
			getline(response_stream, status_message);
			if (!response_stream || http_version.substr(0, 5) != "HTTP/") {
				cerr << "Invalid response\n";
				return 1;
			}
			if (status_code != 200) {
				cerr << "Upload: Response returned with status code " << status_code << "\n";
				return 1;
			}

			// Read the response headers, which are terminated by a blank line.
			boost::asio::read_until(socket, response, "\r\n\r\n");

			// Process the response headers.
			string header;
			while (getline(response_stream, header) && header != "\r")
			{
			}

			// Write whatever content we already have to output.
			if (response.size() > 0) {
				if (debug_flag)
				{
					cout << &response;
				}
			}

			// Read until EOF, writing data to output as we go.
			while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error))
			{
				if (debug_flag)
				{
					cout << &response;
				}
			}
			if (error != boost::asio::error::eof)
			{
				throw boost::system::system_error(error);
			}
		}
		catch (exception& e)
		{
			cerr << "Exception: " << e.what() << "\n";
			return 1;
		}
	}
	
	delete field;
	return 0;
}
