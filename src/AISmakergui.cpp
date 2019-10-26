///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AISmakergui.h"

///////////////////////////////////////////////////////////////////////////

m_dialog::m_dialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 1, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_stInformation = new wxStaticText( this, wxID_ANY, wxT("INSTRUCTIONS:\nPress the 'Make' button.\n\nSelect the input file.\nSelect the output file.\n\nWait for the message advising that \n the encoding is complete."), wxDefaultPosition, wxDefaultSize, 0 );
	m_stInformation->Wrap( -1 );
	fgSizer1->Add( m_stInformation, 0, wxALL, 5 );

	m_buttonAIS = new wxButton( this, wxID_ANY, wxT("Make AIS NMEA Messages"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_buttonAIS, 0, wxALL, 5 );


	this->SetSizer( fgSizer1 );
	this->Layout();
	fgSizer1->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_buttonAIS->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_dialog::OnMakeAIS ), NULL, this );
}

m_dialog::~m_dialog()
{
	// Disconnect Events
	m_buttonAIS->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( m_dialog::OnMakeAIS ), NULL, this );

}
