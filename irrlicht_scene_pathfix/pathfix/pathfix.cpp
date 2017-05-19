/*
28.1.13 (c) mjt

pathfix -ohjelma joka ottaa parametrina .irr filun, käy sen läpi etsien kaikki 
.irrmesh  jonka se muuttaa scenen_nimi + alkupnimi.irmesh esim 
talo.irr  cube.irrmesh  ->  talo_cube.irrmesh  (ja alkuun oikea hakemisto)
se sit lataa kyseisen meshin jonka tietoja se muuttaa:
* vaihtaa kaikki "/lightmap" -> scenen_nimi+"LM.png"  (se myös renamee lightmap.png tuon nimiseks),
lisätään alkuun oikea hakemisto
* eti  "value="  ja  ".jpg" / ".png"  samalla rivillä, niin otetaan se tiedostonimi ylös, unohdetaan
se hakemisto ja vaihdetaan hakemisto oikeaksi.

hakemisto olkoon eka parametri.. eli   pathfix.exe PATH  SCENEFILE.IRR
tee .bat filu jossa sit   pathfix.exe ../../media/  %1  
niin tuohon voi dragata noit .irr filuja.
*/
#include <stdio.h>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;
void loadMesh(string name, string newname, string path, string lmfile);
string stringToUpper(string myString);
bool replace(std::string& str, const std::string& from, const std::string& to);

void main(int argc, char* argv[])
{

	if(argc<3)
	{
		cout<<"Usage:\n   "<<argv[0]<<" PATH  .IRR-FILE\n";
		system ("pause");
		return;
	}


	string scenename, lmfile;
	ifstream infile;

	infile.open(argv[2]);
	scenename = argv[2];
	scenename = scenename.substr(0, scenename.find('.'));

	if((int)scenename.find("/")>0 || (int)scenename.find("\\")>0) // poista hakemisto
	{
		int end = (int)scenename.find_last_of("\\")+1;
		if(end<=0) end = scenename.find_last_of('/')+1;	
		scenename = scenename.substr(end, scenename.length()-end);
	}

	lmfile=scenename + "_LM.png";

	rename("lightmap.png", lmfile.c_str());
	cout<<"lightmap.png -> " +lmfile+"\n";
	string newdata="";
	while(infile.eof()==false)
	{
		string str;
		getline(infile, str);

		int m=str.find(".irrmesh");
		if(m>0)
		{
			int start=str.find("value=\"")+7;
			string orig_meshfile=str.substr(start,  m-start+8);
			string newmeshname=scenename +"_"+ orig_meshfile;

			loadMesh(orig_meshfile, newmeshname, argv[1], lmfile);

			cout<<orig_meshfile+" -> "+newmeshname+"\n";
			replace(str, orig_meshfile, argv[1] + newmeshname);
		}
		newdata+=str+"\n";
	}
	infile.close();

	ofstream outfile (argv[2],ofstream::binary);
	outfile.write(newdata.c_str(), newdata.length());
	outfile.close();		
}

void loadMesh(string name, string newname, string path, string lmfile)
{
	string newdata="";
	ifstream infile;
	infile.open(name);

	while(infile.eof()==false)
	{
		string str;
		getline(infile, str);

		if((int)str.find("\"/lightmap\"")>0)
		{
			replace(str, "/lightmap", path+lmfile);
		}
		else
		{
			string tmp=str;
			tmp=stringToUpper(tmp);
			int m=0;
			int jpg=(int)tmp.find(".jpg");
			int png=(int)tmp.find(".png");
			if(jpg>0) m=jpg;
			if(png>0) m=png;
			if(m>0 && (int)tmp.find("texture name")>0)
			{
				int start=str.find("value=\"")+7;
				string texname=str.substr(start,  m-start+4);
				string origtexname=texname;

				// vaihda hakemisto
				if((int)texname.find("/")>0)
				{
					int end = texname.find_last_of('/')+1;
					texname = texname.substr(end, texname.length()-end);
				}
				texname=path+texname;

				replace(str, origtexname, texname);
			}
		}
		newdata+=str+"\n";
	}
	infile.close();

	ofstream outfile (newname, ofstream::binary);
	outfile.write(newdata.c_str(), newdata.length());
	outfile.close();		
}

string stringToUpper(string myString)
{
	const int length = myString.length();
	for(int i=0; i!=length; ++i)
	{
		myString[i] = tolower(myString[i]);
	}
	return myString;
}

bool replace(std::string& str, const std::string& from, const std::string& to) 
{
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
