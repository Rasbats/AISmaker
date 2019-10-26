/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  AISmaker Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                   *
 *   $EMAIL$                                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#include <wx/progdlg.h>
#include <wx/wx.h>
#include "wx/dir.h"
#include "AISmaker_pi.h"
#include <cstdint>
#include <algorithm>
#include <string>
#include <wx/tokenzr.h>
#include <bitset>
#include <wx/textfile.h>
#include <time.h>
#include <wx/file.h>

class AISmaker_pi;

#define FAIL(X) do { error = X; goto failed; } while(0)

static const std::vector<std::pair<uint32_t, char>> payloadencoding = { 
	{ 0, '0' }, { 1, '1'}, { 2, '2'}, { 3, '3'}, { 4, '4'}, { 5, '5'}, { 6, '6'}, { 7, '7'}, { 8, '8'}, { 9, '9'}, { 10, ':'}, {11 , ';'},
{12, '<'}, { 13 , '=' }, { 14, '>'}, { 15, '?'}, { 16, '@'}, { 17, 'A'}, { 18, 'B'}, { 19, 'C'}, { 20, 'D'}, { 21, 'E'}, { 22, 'F'}, { 23, 'G'}, { 24, 'H'}, {
25 , 'I'}, { 26 , 'J'}, { 27 , 'K'}, { 28 , 'L'}, { 29 , 'M'}, { 30 , 'N'}, { 31 , 'O'}, { 32 , 'P'}, { 33 , 'Q'}, { 34 , 'R'}, {
35 , 'S'}, { 36 , 'T'}, { 37 , 'U'}, { 38 , 'V'}, { 39 , 'W'}, { 40 , '`'}, { 41 , 'a'}, { 42 , 'b'}, { 43 , 'c'}, { 44 , 'd'}, {
45 , 'e'}, { 46 , 'f'}, { 47 , 'g'}, { 48 , 'h'}, { 49 , 'i'}, { 50 , 'j'}, { 51 , 'k'}, { 52 , 'l'}, { 53 , 'm'}, { 54 , 'n'}, {
	55, 'o'}, { 56, 'p'}, { 57, 'q'}, { 58, 'r'}, { 59, 's'}, { 60, 't'}, { 61, 'u'}, { 62, 'v'}, { 63, 'w' }
};


// Sixbit ASCII encoding
static const std::vector<std::pair<char, uint32_t >>sixbitencoding = {
	{ '@',0}, { 'A' , 1}, { 'B' , 2}, { 'C' , 3}, { 'D' , 4}, { 'E' , 5}, { 'F' , 6}, { 'G' , 7}, { 'H' , 8}, { 'I' , 9}, 
	{ 'J' , 10}, { 'K' , 11}, { 'L' , 12}, { 'M' , 13}, { 'N' , 14}, 
	{ 'O' , 15}, { 'P' , 16}, { 'Q' , 17}, { 'R' , 18}, { 'S' , 19}, { 'T' , 20}, { 'U' , 21}, { 'V' , 22}, { 'W' , 23}, { 'X' , 24}, 
	{ 'Y' , 25}, { 'Z' , 26}, { '[' , 27}, { '\\' , 28}, { ']' , 29}, { '^' , 30}, { '_' , 31}, {' ' , 32}, { '1' , 33}, { '"' , 34}, 
	{ '#' , 35}, { '$' , 36}, { '%' , 37}, { '&' , 38}, { '\'' , 39}, { '(' , 40}, { ')' , 41}, { '*' , 42}, { '+' , 43}, { '}' , 44},
	{ '-' , 45}, { '.' , 46}, { '/' , 47}, {'0' , 48}, { '1' , 49}, { '2' , 50}, { '3' , 51}, { '4' , 52}, { '5' , 53}, { '6' , 54}, 
	{ '7', 55 }, { '8', 56 }, {'9' , 57 }, { ',', 58 }, { ';', 59 }, { '<', 60 }, { '=', 61 }, { '>', 62 },{ '?', 63 }
};


static const std::vector<std::pair<char, uint32_t>> NMEA_TABLE = {
	{'0' , 0 }, {'1', 1 }, {'2', 2 }, {'3',  3 }, {'4', 4 }, {'5', 5 }, {'6', 6 }, {'7', 7 }, {'8', 8 }, {'9', 9 }, 
	{':', 10 }, {';', 11 }, {'<', 12 }, {'=', 13 }, {'>', 14 }, 
	{'?', 15 }, {'@', 16 }, {'A', 17 }, {'B', 18 }, {'C', 19 }, {'D', 20 }, {'E', 21 }, {'F', 22 }, {'G', 23 }, {'H', 24 }, 
	{'I', 25 }, {'J', 26 }, {'K', 27 }, {'L', 28 }, {'M', 29 }, {'N', 30 }, {'O', 31 }, {'P', 32 }, {'Q', 33 }, {'R', 34 }, 
	{'S', 35 }, {'T', 36 }, {'U', 37 }, {'V', 38 }, {'W', 39 }, {'`', 40 }, {'a', 41 }, {'b', 42 }, {'c', 43 }, {'d', 44 }, 
	{'e', 45 }, {'f', 46 }, {'g', 47 }, {'h', 48 }, {'i', 49 }, {'j', 50 }, {'k', 51 }, {'l', 52 }, {'m', 53 }, {'n', 54 }, 
	{'o', 55 }, {'p', 56 }, {'q', 57 }, {'r', 58 }, {'s', 59 }, {'t', 60 }, {'u', 61 }, {'v', 62 }, {'w', 63 }
};

static const std::vector<std::pair<uint8_t, char>> SIXBIT_ASCII_TABLE = {
	{ 0, '@' }, { 1, 'A' }, { 2, 'B' }, { 3, 'C' }, { 4, 'D' }, { 5, 'E' }, { 6, 'F' }, { 7, 'G' }, { 8, 'H' },
	{ 9, 'I' }, { 10, 'J' }, { 11, 'K' }, { 12, 'L' }, { 13, 'M' }, { 14, 'N' }, { 15, 'O' }, { 16, 'P' },
	{ 17, 'Q' }, { 18, 'R' }, { 19, 'S' }, { 20, 'T' }, { 21, 'U' }, { 22, 'V' }, { 23, 'W' }, { 24, 'X' },
	{ 25, 'Y' }, { 26, 'Z' }, { 27, '[' }, { 28, '\\' }, { 29, ']' }, { 30, '^' }, { 31, '_' }, { 32, ' ' },
	{ 33, '!' }, { 34, '\"' }, { 35, '#' }, { 36, '$' }, { 37, '%' }, { 38, '&' }, { 39, '\'' }, { 40, '(' },
	{ 41, ')' }, { 42, '*' }, { 43, '+' }, { 44, ',' }, { 45, '-' }, { 46, '.' }, { 47, '/' }, { 48, '0' },
	{ 49, '1' }, { 50, '2' }, { 51, '3' }, { 52, '4' }, { 53, '5' }, { 54, '6' }, { 55, '7' }, { 56, '8' },
	{ 57, '9' }, { 58, ':' }, { 59, ';' }, { 60, '<' }, { 61, '=' }, { 62, '>' }, { 63, '?' },
};

string AisMaker::Str2Str(string str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
	return str;
}

float AisMaker::Str2Float(string str, char* exc){
	float result;
	string floatString = Str2Str(str, exc);
	result = strtof((floatString).c_str(), 0); // string to float

	return result;
}

int AisMaker::Str2Int(string str, char* exc){
	int result;
	string intString = Str2Str(str, exc);
	result = atoi((intString).c_str()); // string to float
	return result;
}

string AisMaker::Int2BString(int value, int length){
	
	string result = "";
	bitset<100>myBitset(value);
	result = myBitset.to_string();

	result = result.substr(result.size() - length, length);
	return result;
}

int AisMaker::findIntFromLetter(char letter){
	auto i = std::find_if(SIXBIT_ASCII_TABLE.begin(), SIXBIT_ASCII_TABLE.end(),
		[letter](const std::pair<uint8_t, char> & p) { return p.second == letter; });
	return i != SIXBIT_ASCII_TABLE.end() ? i->first : 0xff;
}

char AisMaker::findCharFromNumber(int mp){

	auto i = std::find_if(NMEA_TABLE.begin(), NMEA_TABLE.end(),
		[mp](const std::pair<char, uint32_t > p) { return p.second == mp; });
	return i != NMEA_TABLE.end() ? i->first : 0xff;
}


string AisMaker::Str2Six(string str, int length){

	    string result;
		char letter;

		for (int i = 0; i< str.size(); i++){
			
			letter = str[i];
			int si = findIntFromLetter(letter);
			result = result + Int2BString(si, 6);
		}
		while( result.size() < length){

			int sj = findIntFromLetter('@');
			result = result + Int2BString(sj, 6);
		}
		return result;
}

int AisMaker::BString2Int(char * bitlist){
	int s = std::bitset<6>(bitlist).to_ulong();
	return s;
}

string AisMaker::NMEAencapsulate(string BigString, int numsixes){
	
	string capsule = "";
	int chindex;
	int substart = 0;
	bool padneeded = false;
	int padBits = 0;

	int* intChars =(int*)calloc(numsixes,6);
	char * myChars;// = &BigString[0u];
	for (chindex = 0; chindex < numsixes; chindex++){				
		string StrVal = BigString.substr(substart, 6);
		if (StrVal.length() < 6) {
			padneeded = true;
		}
		//
		myChars = &StrVal[0u];
		if (!padneeded) {
			intChars[chindex] = BString2Int(myChars);
		}
		else {
			intChars[chindex] = BString2Int(myChars);
			padBits = 6 - StrVal.length();
			intChars[chindex] << (int)padBits;
		}
		substart += 6;
	}
	// Now intChars contains the encoded bits for the AIS string
	for (chindex = 0; chindex < numsixes; chindex++) {
		char plChar = findCharFromNumber(intChars[chindex]);
		capsule = capsule + plChar;
	}
	

	// Now we have the NMEA payload in capsule
	free(intChars);
	return capsule;
}

/*
string AisMaker::NMEAencoding(string BigString) {

	int start = 0;
	int stop = 0;
	
	string StrVal;
	char* myChars;
	string buf = "";
	bool bPad = false;

	while (start < BigString.size()) {
		stop = start + 6 - 1;
		if (stop >= BigString.size()) {
			padBits = stop - BigString.size() + 1;
			bPad = true;			
			stop = BigString.size() - 1;
		}
		StrVal = BigString.substr(start, stop);				
		myChars = &StrVal[0];

		int data = BString2Int(myChars);
		if (bPad) {
			data << padBits;
		}
		char value = findCharFromNumber(data);

		//wxMessageBox(value);

		buf = buf + value;
		start = stop + 1;
	}

	//wxMessageBox(buf);

	// Now we have the NMEA payload in buf
	return buf;
}

*/

/*
wxString AisMaker::makeNewCheckSum(wxString mySentence) {
	
	
	string sentence = mySentence;
	const char* nmea_data = sentence.c_str();
	int len = strlen(nmea_data);
	
	int crc = 0;
	int i;

	// the first $ sign and the last two bytes of original CRC + the * sign
	for (i = 0; i < len; i++) {
		crc ^= nmea_data[i];
	}

	
	std::stringstream sstream;
	sstream << std::hex << std::uppercase << crc;
	std::string result = sstream.str();

	
	return result;

}
*/
wxString AisMaker::makeCheckSum(wxString mySentence){
	int i;
	unsigned char XOR;

	wxString s(mySentence);
	wxCharBuffer buffer = s.ToUTF8();
	char *Buff = buffer.data();	// data() returns const char *
	unsigned long iLen = strlen(Buff);
	for (XOR = 0, i = 0; i < iLen; i++)
		XOR ^= (unsigned char)Buff[i];
	stringstream tmpss;
	tmpss << hex << uppercase << (int)XOR << endl;
	wxString mystr = tmpss.str();
	return mystr;
}


string AisMaker::nmeaEncode1(string LineDict) {
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1, token.length());

		st.Replace("\"", "");
		if (index == _T("TYPE")) {
			myLineDict.TYPE = st;
		}
		else if (index == _T("MMSI")) {
			myLineDict.MMSI = st;
		}
		else if (index == _T("STATUS")) {
			myLineDict.STATUS = st;
		}
		else if (index == _T("SPEED")) {
			myLineDict.SPEED = st;
		}
		else if (index == _T("LON")) {
			myLineDict.LON = st;
		}
		else if (index == _T("LAT")) {
			myLineDict.LAT = st;
		}
		else if (index == _T("COURSE")) {
			myLineDict.COURSE = st;
		}
		else if (index == _T("HEADING")) {
			myLineDict.HEADING = st;
		}
		else if (index == _T("TIMESTAMP")) {
			myLineDict.TIMESTAMP = st;
		}
	}

	if (myLineDict.TYPE == _T("1")) {
		
		string MessageID = Int2BString(Str2Int("1", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();
		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string Spare1 = Int2BString(0, 8);	

		string sSPEED;
		sSPEED = myLineDict.SPEED.mb_str();
		float  sog = Str2Float(sSPEED, "");
		string SOG = Int2BString(sog, 10);

		string PosAccuracy = Int2BString(1, 1);
		string Mi = Int2BString(0, 2);

		string sStatus;
		sStatus = myLineDict.STATUS.mb_str();
		string Status = Int2BString(Str2Int(sStatus, ""), 4);
		
		string sLAT;
		sLAT = myLineDict.LAT.mb_str();
		float lat = Str2Float(sLAT, "");
		string Latitude = Int2BString(int(lat * 600000), 27);

		string sLON;
		sLON = myLineDict.LON.mb_str();
		float lon = Str2Float(sLON, "");
		string Longitude = Int2BString(int(lon * 600000), 28);	

		string sCOURSE;
		sCOURSE = myLineDict.COURSE.mb_str();
		float cog = Str2Float(sCOURSE, "");
		string COG = Int2BString(int(cog * 10), 12);

		string sHEADING;
		sHEADING = myLineDict.HEADING.mb_str();
		int	heading = Str2Int(sHEADING, "");
		string Heading = Int2BString(heading, 9);

		string sTIMESTAMP;
		sTIMESTAMP = myLineDict.TIMESTAMP.mb_str();
		string tStamp = sTIMESTAMP;
		int	tSecond = wxGetUTCTime();
		string	TimeStamp = Int2BString(tSecond, 6);

		string Spare = Int2BString(0, 3);
		string Spare2 = Int2BString(0, 2);

		string State = Int2BString(393222, 27);
		string RAIM = Int2BString(0, 1);
		string CommStat = Int2BString(0, 19);
		string RotAIS = Int2BString(-128, 8);  // # default is "not-available"

		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + Status + RotAIS + SOG + PosAccuracy + Longitude + Latitude + COG + Heading + TimeStamp + Mi + Spare + RAIM + CommStat;
		BigString = BigString + State;

		string capsule = NMEAencapsulate(BigString, 28);
		string aisnmea = "AIVDM,1,1,,A," + capsule + ",O";
		wxString myNMEA = aisnmea;
		wxString myCheck = makeCheckSum(myNMEA);

		aisnmea = '!' + aisnmea + '*' + myCheck;
		return aisnmea;
	}

	return "unknown";

}

string* AisMaker::nmeaEncode5(string LineDict) {
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		// process token here
		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1, token.length());

		st.Replace("\"", "");
		if (index == _T("TYPE")) {
			myLineDict.TYPE = st;
		}
		else if (index == _T("MMSI")) {
			myLineDict.MMSI = st;
		}
		else if (index == _T("IMO")) {
			myLineDict.IMO = st;
		}
		else if (index == _T("CALLSIGN")) {
			myLineDict.CALLSIGN = st;
		}
		else if (index == _T("SHIPNAME")) {
			myLineDict.SHIPNAME = st;
		}
		else if (index == _T("SHIPTYPE")) {
			myLineDict.SHIPTYPE = st;
		}
		else if (index == _T("DRAUGHT")) {
			myLineDict.DRAUGHT = st;
		}
		else if (index == _T("DESTINATION")) {
			myLineDict.DESTINATION = st;
		}

	}

	if (myLineDict.TYPE == _T("5")) {

		string MessageID = Int2BString(Str2Int("5", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();
		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string sIMO;
		sIMO = myLineDict.IMO.mb_str();
		string IMO = Int2BString(Str2Int(sIMO, ""), 30);

		string sCALLSIGN;
		sCALLSIGN = myLineDict.CALLSIGN.mb_str();
		string CALLSIGN = Str2Six(sCALLSIGN, 42);

		string sSHIPNAME;
		sSHIPNAME = myLineDict.SHIPNAME.mb_str();
		string SHIPNAME = Str2Six(sSHIPNAME, 120);

		string sSHIPTYPE;
		sSHIPTYPE = myLineDict.SHIPTYPE.mb_str();
		string SHIPTYPE = Int2BString(Str2Int(sSHIPTYPE, ""), 8);

		string sDRAUGHT;
		sDRAUGHT = myLineDict.DRAUGHT.mb_str();
		string DRAUGHT = Int2BString(Str2Int(sDRAUGHT, ""), 8);

		string sDESTINATION;
		sDESTINATION = myLineDict.DESTINATION.mb_str();
		string DESTINATION = Str2Six(sDESTINATION, 120);

		string ais_version = Int2BString(2, 2);
		string to_bow = Int2BString(0, 9);
		string to_stern = Int2BString(0, 9);
		string to_port = Int2BString(0, 6);
		string to_starboard = Int2BString(0, 6);
		string epfd = Int2BString(1, 4);  // # default is "not-available"
		string month = Int2BString(0, 4);
		string day = Int2BString(0, 5);
		string hour = Int2BString(24, 5);
		string minute = Int2BString(60, 6);
		string dte = Int2BString(0, 1);
		string spare = Int2BString(0, 1);


		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + ais_version + IMO + CALLSIGN + SHIPNAME + SHIPTYPE + to_bow + to_stern + to_port + to_starboard + epfd + month + day + hour + minute + DRAUGHT + DESTINATION + dte + spare;


		if (BigString.size() < 424) {
			wxMessageBox("error");
		}//else 
			//wxMessageBox("ok");

		string enc = NMEAencapsulate(BigString, 71);

		int DATA_SENTENCE_MAX_LENGTH = 47;
		int partPadBits = 0;
		string partEncoded = "";

		int sentenceCount = enc.size() / DATA_SENTENCE_MAX_LENGTH + 1;

		// Split the string
		for (int i = 0; i < sentenceCount; i++) {
			int start = i * DATA_SENTENCE_MAX_LENGTH;
			int end;
			if (i < sentenceCount - 1) {
				end = start + DATA_SENTENCE_MAX_LENGTH;
				partEncoded = enc.substr(start, end);
			}
			else {
				end = enc.size();
				partPadBits = padBits;
				partEncoded = enc.substr(start, end);
			}

			string vdm = "";
			string str = std::to_string(i + 1);
			string capsule = partEncoded;
			vdm += "AIVDM,2," + str + ",0,A,";

			vdm += capsule;
			vdm += ",O";

			sentences[i] = vdm;
		}

		for (int x = 0; x < 2; x++) {

			aisnmea[x] = sentences[x];
			wxString myNMEA = aisnmea[x];
			wxString myCheck = makeCheckSum(myNMEA);

			aisnmea[x] = '!' + aisnmea[x] + '*' + myCheck;
		}

		return aisnmea;
	}
	string uk[2];
	return uk;

}

string AisMaker::nmeaEncode18(string LineDict){
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();		
		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1 , token.length());		

		st.Replace("\"", "");
		if (index == _T("TYPE")){			
			myLineDict.TYPE = st;			
		}
		else if (index == _T("MMSI")){
			myLineDict.MMSI = st;
		}
		else if (index == _T("STATUS")){
			myLineDict.STATUS = st;
		}
		else if (index == _T("SPEED")){
			myLineDict.SPEED = st;
		}
		else if (index == _T("LON")){
			myLineDict.LON = st;
		}
		else if (index == _T("LAT")){
			myLineDict.LAT = st;
		}
		else if (index == _T("COURSE")){
			myLineDict.COURSE = st;
		}
		else if (index == _T("HEADING")){
			myLineDict.HEADING = st;
		}
		else if (index == _T("CHANNEL")){
			myLineDict.CHANNEL = st;
		}
		else if (index == _T("TIMESTAMP")){
			myLineDict.TIMESTAMP = st;
		}
	}

	if (myLineDict.TYPE == _T("18")){

		string MessageID = Int2BString(Str2Int("18", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();	

		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string Spare1 = Int2BString(0, 8);

		string Channel = myLineDict.CHANNEL.mb_str();
		
		string sSPEED;
		sSPEED = myLineDict.SPEED.mb_str();
		float  sog = Str2Float(sSPEED,"");
		string SOG = Int2BString(sog, 10);

		string PosAccuracy = Int2BString(1, 1);

		string sLON;
		sLON = myLineDict.LON.mb_str();
		float lon = Str2Float(sLON, "");
		string Longitude = Int2BString(int(lon * 600000), 28);

		string sLAT;
		sLAT = myLineDict.LAT.mb_str();
		float lat = Str2Float(sLAT, "");
		string Latitude = Int2BString(int(lat * 600000), 27);

		string sCOURSE;
		sCOURSE = myLineDict.COURSE.mb_str();
		float cog = Str2Float(sCOURSE, "");
		string COG = Int2BString(int(cog * 10), 12);

		string sHEADING;
		sHEADING = myLineDict.HEADING.mb_str();
		int	heading = Str2Int(sHEADING, "");
		string Heading = Int2BString(heading, 9);

		string sTIMESTAMP;
		sTIMESTAMP = myLineDict.TIMESTAMP.mb_str();
		string tStamp = sTIMESTAMP;
		int	tSecond = wxGetUTCTime();
		string	TimeStamp = Int2BString(tSecond, 6);


		string Spare2 = Int2BString(0, 2);

		string State = Int2BString(393222, 27);

		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + Spare1 + SOG + PosAccuracy + Longitude + Latitude + COG + Heading + TimeStamp + Spare2;
		BigString = BigString + State;

		string capsule = NMEAencapsulate(BigString, 28);
		string aisnmea = "AIVDM,1,1,," + Channel + "," + capsule + ",O";
		wxString myNMEA = aisnmea;
		wxString myCheck = makeCheckSum(myNMEA);

		aisnmea = '!' + aisnmea + '*' + myCheck;
        return aisnmea;
	}

	return "unknown";
	
}

string AisMaker::nmeaEncode21(string LineDict) {
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		// process token here

		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1, token.length());

		st.Replace("\"", "");
		if (index == _T("TYPE")) {
			myLineDict.TYPE = st;
		}
		else if (index == _T("MMSI")) {
			myLineDict.MMSI = st;
		}
		else if (index == _T("ATON_TYPE")) {
			myLineDict.ATON_TYPE = st;
		}
		else if (index == _T("ATON_NAME")) {
			myLineDict.ATON_NAME = st;
		}
		else if (index == _T("LON")) {
			myLineDict.LON = st;
		}
		else if (index == _T("LAT")) {
			myLineDict.LAT = st;
		}
	}

	if (myLineDict.TYPE == _T("21")) {

		string MessageID = Int2BString(Str2Int("21", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();
		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string sATON_TYPE;
		sATON_TYPE = myLineDict.ATON_TYPE.mb_str();
		string ATON_TYPE = Int2BString(Str2Int(sATON_TYPE, ""), 5);

		string sATON_NAME;
		sATON_NAME = myLineDict.ATON_NAME.mb_str();
		string ATON_NAME = Str2Six(sATON_NAME, 120);

		string sLON;
		sLON = myLineDict.LON.mb_str();
		float lon = Str2Float(sLON, "");
		string Longitude = Int2BString(int(lon * 600000), 28);

		string sLAT;
		sLAT = myLineDict.LAT.mb_str();
		float lat = Str2Float(sLAT, "");
		string Latitude = Int2BString(int(lat * 600000), 27);
		
		string pos_accuracy = Int2BString(0, 1);
		string to_bow = Int2BString(0, 9);
		string to_stern = Int2BString(0, 9);
		string to_port = Int2BString(0, 6);
		string to_starboard = Int2BString(0, 6);
		string epfd = Int2BString(1, 4);  // # default is "not-available"
		string second = Int2BString(60, 6);
		string off_position = Int2BString(0, 1);
		string status = Int2BString(Str2Int("00000000",""), 8);
		string raim = Int2BString(0, 1);
		string real_virtual = Int2BString(1, 1);
		string mode = Int2BString(0, 1);

		string spare = Int2BString(0, 1);


		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + ATON_TYPE + ATON_NAME + pos_accuracy + Longitude + Latitude + to_bow + to_stern + to_port + to_starboard + epfd + second + off_position + status + raim + real_virtual + mode + spare;

		if (BigString.size() < 272) {
			wxMessageBox("error");
		}//else 
			//wxMessageBox("ok");

		string enc = NMEAencapsulate(BigString, 46);

		string Channel = "A";

		string capsule = NMEAencapsulate(BigString, 46);

		string aisnmea = "AIVDM,1,1,," + Channel + "," + capsule + ",O";
		wxString myNMEA = aisnmea;
		wxString myCheck = makeCheckSum(myNMEA);

		aisnmea = '!' + aisnmea + '*' + myCheck;
		return aisnmea;
	}
	
	return "unknown";

}


string AisMaker::nmeaEncode24A(string LineDict) {
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();	
		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1, token.length());

		st.Replace("\"", "");
		if (index == _T("TYPE")) {
			myLineDict.TYPE = st;
		}
		else if (index == _T("MMSI")) {
			myLineDict.MMSI = st;
		}
		else if (index == _T("SHIPNAME")) {
			myLineDict.SHIPNAME = st;
		}		

	}

	if (myLineDict.TYPE == _T("24")) {

		string MessageID = Int2BString(Str2Int("24", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();
		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string sSHIPNAME;
		sSHIPNAME = myLineDict.SHIPNAME.mb_str();
		string SHIPNAME = Str2Six(sSHIPNAME, 120);

		string Spare2 = Int2BString(0, 2);
		string Spare8 = Int2BString(0, 8);

		string PartNum = Int2BString(0, 2);
		
		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + PartNum + SHIPNAME;

		

		string Channel = "B";

		string capsule = NMEAencapsulate(BigString, 27);	

		string aisnmea = "AIVDM,1,1,," + Channel + "," + capsule + ",O";
		wxString myNMEA = aisnmea;
		wxString myCheck = makeCheckSum(myNMEA);

		aisnmea = '!' + aisnmea + '*' + myCheck;
		return aisnmea;
	}

	return "unknown";

}


string AisMaker::nmeaEncode24B(string LineDict) {
	wxString st;

	wxStringTokenizer tokenizer(LineDict);
	while (tokenizer.HasMoreTokens())
	{
		wxString token = tokenizer.GetNextToken();
		wxString index = token.substr(0, token.find("="));

		st = token.substr(token.find("=") + 1, token.length());

		st.Replace("\"", "");
		if (index == _T("TYPE")) {
			myLineDict.TYPE = st;
		}
		else if (index == _T("MMSI")) {
			myLineDict.MMSI = st;
		}		
		else if (index == _T("CALLSIGN")) {
			myLineDict.CALLSIGN = st;
		}
		else if (index == _T("SHIPTYPE")) {
			myLineDict.SHIPTYPE = st;
		}

	}

	//wxMessageBox(myLineDict.TYPE, _T("TYPE"));

	if (myLineDict.TYPE == _T("24")) {

		string MessageID = Int2BString(Str2Int("24", ""), 6);

		string RepeatIndicator = Int2BString(0, 2);

		string sMMSI;
		sMMSI = myLineDict.MMSI.mb_str();
		string MMSI = Int2BString(Str2Int(sMMSI, ""), 30);

		string sCALLSIGN;
		sCALLSIGN = myLineDict.CALLSIGN.mb_str();
		string CALLSIGN = Str2Six(sCALLSIGN, 42);
		
		string sSHIPTYPE;
		sSHIPTYPE = myLineDict.SHIPTYPE.mb_str();
		string SHIPTYPE = Int2BString(Str2Int(sSHIPTYPE, ""), 8);

		string Spare2 = Int2BString(0, 2);

		
		string PartNum = Int2BString(1, 2);
		string VendorId = Str2Six("@@@@@@@", 42);
		string to_bow = Int2BString(0, 9);
		string to_stern = Int2BString(0, 9);
		string to_port = Int2BString(0, 6);
		string to_starboard = Int2BString(0, 6);
		string epfd = Int2BString(1, 4);  // # default is "not-available"


		string BigString = MessageID;
		BigString = BigString + RepeatIndicator;
		BigString = BigString + MMSI + PartNum + SHIPTYPE  + VendorId + CALLSIGN + to_bow + to_stern + to_port + to_starboard + epfd + Spare2;

		string Channel = "B";

		string capsule = NMEAencapsulate(BigString, 28);
		string aisnmea = "AIVDM,1,1,," + Channel + "," + capsule + ",O";
		wxString myNMEA = aisnmea;
		wxString myCheck = makeCheckSum(myNMEA);

		aisnmea = '!' + aisnmea + '*' + myCheck;
		return aisnmea;
	}

	return "unknown";

}

AisMaker::AisMaker( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) 
	: m_dialog( parent, id, title, pos, size, style )
{	
    this->Fit();
    dbg=false; //for debug output set to true
}

void AisMaker::SetViewPort(PlugIn_ViewPort *vp)
{
	if (m_vp == vp)  return;

	m_vp = new PlugIn_ViewPort(*vp);
}


void AisMaker::OnClose(wxCloseEvent& event)
{
	plugin->OnAISmakerDialogClose();
}

int AisMaker::GetMessageId(string line) {

	if (line.find("TYPE=\"1\"") == 0) {
		return 1;
	}
	else if (line.find("TYPE=\"5\"") == 0) {
		return 5;
	}
	else if (line.find("TYPE=\"18\"") == 0) {
		return 18;
	}
	else if (line.find("TYPE=\"21\"") == 0) {
		return 21;
	}
	else if (line.find("TYPE=\"24\"") == 0) {
		return 24;
	}	

	return 0;
}

void AisMaker::OnMakeAIS(wxCommandEvent& event)
{	
	wxString str;
	string sLine;
	string sLine1;

	int myId = 0;

	wxString myString;
	wxString myString51;
	wxString myString52;

	string myNmea[2];
	string* nmeaout;
	string snmea;

	wxString        file;
	wxFileDialog    fdlog(this);

	// ... get the simulator.txt file ...
	// show file dialog and get the path to
	// the file that was selected.
	if (fdlog.ShowModal() != wxID_OK) return;
	file.Clear();
	file = fdlog.GetPath();
	
	
	// open the file
	wxTextFile      tfile;
	tfile.Open(file);

	// read the first line
	str = tfile.GetFirstLine();
	sLine = str.ToStdString();
    myId = GetMessageId(sLine);

	wxString        wfile; // this is going to be the ais messages file

	// show file dialog and get the path to
	// the file that was selected.
	bool error_occured = false;
	wxFileDialog wfAisMaker(this, _("Export ais messages"), wxEmptyString, wxEmptyString, _T("Txt files (*.txt)|*.txt|All files (*.*)|*.*"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (wfAisMaker.ShowModal() == wxID_CANCEL){
		error_occured = true;     // the user changed idea...
		return;
	}

	wfile = wfAisMaker.GetPath();
	if (wfAisMaker.GetPath() == wxEmptyString){ error_occured = true; if (dbg) printf("Empty Path\n"); }

	wxTextFile nmeafile = wfile;
	nmeafile.Open();

	switch (myId) {
		case 1: {
			 sLine1 = nmeaEncode1(sLine); // make the ais nmea message
			 myString = sLine1;

			nmeafile.AddLine(myString, wxTextFileType_None);
			break;
		}
		case 5: {
			nmeaout = nmeaEncode5(sLine); // 2 part message
			myNmea[0] = nmeaout[0];
			myNmea[1] = nmeaout[1];
			myString51 = myNmea[0];
			myString52 = myNmea[1];
			nmeafile.AddLine(myString51, wxTextFileType_None); // the first line
			nmeafile.AddLine(myString52, wxTextFileType_None); // the first line	
			break;

		}
		case 18: {
			sLine1 = nmeaEncode18(sLine);
			myString = sLine1;
			nmeafile.AddLine(myString, wxTextFileType_None);
			break;
		}
		case 21: {
			sLine1 = nmeaEncode21(sLine);
			myString = sLine1;
			nmeafile.AddLine(myString, wxTextFileType_None);
			break;
		}
		case 24: {
			if (sLine.find("PART_NO=\"0\"") != std::string::npos) {
				sLine1 = nmeaEncode24A(sLine);
			}
			else if (sLine.find("PART_NO=\"1\"") != std::string::npos) {
				sLine1 = nmeaEncode24B(sLine);
			}
			
			myString = sLine1;
			nmeafile.AddLine(myString, wxTextFileType_None);
			break;
		}
		
	}

	// read all lines one by one
	// until the end of the file

	while (!tfile.Eof())
	{
		str = tfile.GetNextLine();
		sLine = str.ToStdString();
		
		myId = GetMessageId(sLine);

		switch (myId) {
			case 1: {
				sLine1 = nmeaEncode1(sLine); // make the ais nmea message
				myString = sLine1;

				nmeafile.AddLine(myString, wxTextFileType_None);
				break;
			}
			case 5: {
				nmeaout = nmeaEncode5(sLine);
				myNmea[0] = nmeaout[0];
				myNmea[1] = nmeaout[1];
				myString51 = myNmea[0];
				myString52 = myNmea[1];
				nmeafile.AddLine(myString51, wxTextFileType_None); // the first line
				nmeafile.AddLine(myString52, wxTextFileType_None); // the second line	
				break;

			}
			case 18: {
				sLine1 = nmeaEncode18(sLine);
				myString = sLine1;
				nmeafile.AddLine(myString, wxTextFileType_None);
				break;
			}
			case 21: {
				sLine1 = nmeaEncode21(sLine);
				myString = sLine1;
				nmeafile.AddLine(myString, wxTextFileType_None);
				break;
			}
			case 24: {
				if (sLine.find("PART_NO=\"0\"") != std::string::npos) {
					sLine1 = nmeaEncode24A(sLine);
				}
				else if (sLine.find("PART_NO=\"1\"") != std::string::npos) {
					sLine1 = nmeaEncode24B(sLine);
				}

				myString = sLine1;
				nmeafile.AddLine(myString, wxTextFileType_None);
				break;
			}		
		}

	}

	nmeafile.Write();
	nmeafile.Close();
	tfile.Close();

	wxMessageBox("Encoding is complete", "Encoding");
}
