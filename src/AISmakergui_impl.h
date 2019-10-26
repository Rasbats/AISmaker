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

#ifndef _CALCULATORGUI_IMPL_H_
#define _CALCULATORGUI_IMPL_H_

#ifdef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "AISmakergui.h"
#include "AISmaker_pi.h"
#include "ocpn_plugin.h"
#include <sstream>

using namespace std;

class AISmaker_pi;
class PlugIn_ViewPort;

struct LineDict {

	wxString TYPE;
	wxString CHANNEL;
	wxString MMSI;
	wxString ATON_TYPE;
	wxString ATON_NAME;
	wxString STATUS;
	wxString SPEED;
	wxString LON;
	wxString LAT;
	wxString COURSE;
	wxString HEADING;
	wxString TIMESTAMP;
	wxString CALLSIGN;
	wxString SHIPNAME;
	wxString SHIPTYPE;
	wxString DRAUGHT;
	wxString DESTINATION;
	wxString IMO;

};


class Position{
public:

	wxString lat, lon, wpt_num;
	Position *prev, *next; /* doubly linked circular list of positions */
	int routepoint;

};


class AisMaker : public m_dialog
{
public:
        AisMaker( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("AISmaker"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( -1,-1 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );	
		
		AISmaker_pi *plugin; 
		void SetViewPort(PlugIn_ViewPort *vp);
		
		LineDict myLineDict;
		string sentences[2];
		string	aisnmea[2];

private:

	string Str2Str(string str, char* charsToRemove);
	float Str2Float(string str, char* exc);
	int Str2Int(string str, char* exc);
	string Int2BString(int value, int length);
	int findIntFromLetter(char letter);
	char findCharFromNumber(int mp);
	string Str2Six(string str, int length);
	int BString2Int(char * bitlist);
	string NMEAencapsulate(string BigString, int numsixes);
	//string NMEAencoding(string BigString);
	wxString makeCheckSum(wxString mySentence);
	//wxString makeNewCheckSum(wxString mySentence);
	
	string nmeaEncode1(string LineDict);
	string* nmeaEncode5(string LineDict);
	string nmeaEncode18(string LineDict);
	string nmeaEncode21(string LineDict);
	string nmeaEncode24A(string LineDict);
	string nmeaEncode24B(string LineDict);

	int GetMessageId(string line);

	void OnMakeAIS(wxCommandEvent& event);
	void OnClose( wxCloseEvent& event );
    bool error_found;
    bool dbg;
	
	

	int padBits = 0;

	bool write_file;


	PlugIn_ViewPort  *m_vp;

};


#endif
