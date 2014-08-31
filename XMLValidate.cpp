// XMLValidate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// ---------------------------------------------------------------------------
//  Simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------
class DOMCountErrorHandler : public DOMErrorHandler
{
public:
	// -----------------------------------------------------------------------
	//  Constructors and Destructor
	// -----------------------------------------------------------------------
	DOMCountErrorHandler();
	~DOMCountErrorHandler();


	// -----------------------------------------------------------------------
	//  Getter methods
	// -----------------------------------------------------------------------
	bool getSawErrors() const;


	// -----------------------------------------------------------------------
	//  Implementation of the DOM ErrorHandler interface
	// -----------------------------------------------------------------------
	bool handleError(const DOMError& domError);
	void resetErrors();


private :
	// -----------------------------------------------------------------------
	//  Unimplemented constructors and operators
	// -----------------------------------------------------------------------
	DOMCountErrorHandler(const DOMCountErrorHandler&);
	void operator=(const DOMCountErrorHandler&);


	// -----------------------------------------------------------------------
	//  Private data members
	//
	//  fSawErrors
	//      This is set if we get any errors, and is queryable via a getter
	//      method. Its used by the main code to suppress output if there are
	//      errors.
	// -----------------------------------------------------------------------
	bool    fSawErrors;
};

inline bool DOMCountErrorHandler::getSawErrors() const
{
	return fSawErrors;
}

DOMCountErrorHandler::DOMCountErrorHandler() :

fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


bool DOMCountErrorHandler::handleError(const DOMError& domError)
{
	fSawErrors = true;
	if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
		std::wcerr << L"Warning at file ";
	else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
		std::wcerr << L"Error at file ";
	else
		std::wcerr << L"Fatal Error at file ";

	std::wcerr << domError.getLocation()->getURI()
		<< L", line " << domError.getLocation()->getLineNumber()
		<< L", char " << domError.getLocation()->getColumnNumber() << std::endl;

	std::wcerr <<  L"Message: " << domError.getMessage() << std::endl;

	return true;
}

void DOMCountErrorHandler::resetErrors()
{
	fSawErrors = false;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if( argc < 2 )
	{
		std::wcout << L"Usage: XMLValidate.exe <Document.xml> <SchemaName SchemaFile.xsd>" << std::endl;
		std::wcout << L"Schemas are optional. Multiple pairs can be specified." << std::endl;
		return 1;
	}
	else
	{
		//std::wcout << L"XMLValidate parsing " << argv[1] << std::endl;
	}

	// Parse commandline
	std::wstring path;
	std::wstring schemas;
	for( int i=2; i<argc; ++i )
	{
		std::wstring param = argv[i];
		std::wcout << param.c_str() << L" " << std::endl;

		schemas += argv[i];
		if( i+1 < argc )
			schemas += L" ";
	}
	XMLCh* xch_schemas = (XMLCh*)schemas.c_str();

	try {
		//std::wcout << L"Initializing xerces" << std::endl;
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& toCatch) {
		std::wcout << L"Error: " << toCatch.getMessage() << std::endl;
		return 1;
	}


	// Instantiate the DOM parser.
	//std::wcout << L"Initializing parser" << std::endl;
	static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
	DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
	DOMBuilder *parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

	parser->setFeature(XMLUni::fgDOMNamespaces, true);
	parser->setFeature(XMLUni::fgXercesSchema, true);
	parser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
	parser->setFeature(XMLUni::fgDOMValidation, true);
	parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

	if( argc > 2 )
	{
		parser->setProperty(XMLUni::fgXercesSchemaExternalSchemaLocation, xch_schemas);
		//parser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, xch_schemas);
	}
    
	// And create our error handler and install it
	//std::wcout << L"Initializing handler" << std::endl;
	DOMCountErrorHandler errorHandler;
	parser->setErrorHandler(&errorHandler);

	try 
		{


		// reset document pool
		parser->resetDocumentPool();

		// parse
		//std::wcout << L"Parsing " << argv[1] << std::endl;
		parser->parseURI(argv[1]);
	}
	catch (const XMLException& toCatch) 
	{
		std::wcout << L"Error: " << toCatch.getMessage() << std::endl;
		return 1;
	}
	catch (const DOMException& toCatch) 
	{
		std::wcout << L"DOM Error: " << toCatch.msg << std::endl;
		return 1;
	}
	catch (...) 
	{
		std::wcout << L"Error: Unexpected Exception " << std::endl ;
		return 1;
	}

	//
	//  Delete the parser itself.  Must be done prior to calling Terminate, below.
	//
	if( parser )
		parser->release();

	// Terminate Xerces
	//std::wcout << L"Terminating xerces." << std::endl;
	XMLPlatformUtils::Terminate();

	//std::wcout << L"Success." << std::endl;
	return errorHandler.getSawErrors() ? 1 : 0;
}

