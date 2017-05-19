#pragma once
#include "Program.h"

class IO
{
	IReadFile* textfile;

public:
	IO() : textfile(0) {}

	bool open(stringc fileName)
	{
		IFileSystem* filesys = device->getFileSystem();
		if ( !filesys->existFile ( fileName ) )
		{
			stringc s="Cannot find ";
			s+=fileName;
			device->getLogger()->log(s.c_str());
			return false;
		}

		textfile = filesys->createAndOpenFile( fileName );
		if ( !textfile )
		{
			stringc s="Cannot open ";
			s+=fileName;
			device->getLogger()->log(s.c_str());
			return false;
		}
		return true;
	}

	void close()
	{
		if(textfile!=0) textfile->drop();
		textfile=0;
	}

	bool readLine(stringc& str )
	{
		char  ch;
		str = ""; 
		while ( 0 != textfile->read( &ch, 1 ) )
		{ 
			if ( ch == '\n' || ch=='\r')
			{
				str+="\0";
				return true;
			}
			else
				str += ch;
		}

		return false; 
	}

	bool readToken(stringc& str )
	{
		char  ch;
		str = "";
		while ( 0 != textfile->read( &ch, 1 ) )
		{ 
			if ( ( ch == ' ') || ( ch == '\t') )
				return true;
			else
				str += ch;
		}
		return false;  
	}

}; 
