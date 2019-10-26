/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  AISmaker Plugin
 * Author:   Mike Rossiter
 *
 ***************************************************************************
 *   Copyright (C) 2017 by Mike Rossiter                                *
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

#include "wx/wxprec.h"

#ifndef  WX_PRECOMP
  #include "wx/wx.h"
#endif //precompiled headers

#include "AISmaker_pi.h"
#include "AISmakergui_impl.h"
#include "AISmakergui.h"


class AISmaker_pi;

// the class factories, used to create and destroy instances of the PlugIn

extern "C" DECL_EXP opencpn_plugin* create_pi(void *ppimgr)
{
    return new AISmaker_pi(ppimgr);
}

extern "C" DECL_EXP void destroy_pi(opencpn_plugin* p)
{
    delete p;
}

//---------------------------------------------------------------------------------------------------------
//
//    AISmaker PlugIn Implementation
//
//---------------------------------------------------------------------------------------------------------

#include "icons.h"

//---------------------------------------------------------------------------------------------------------
//
//          PlugIn initialization and de-init
//
//---------------------------------------------------------------------------------------------------------

AISmaker_pi::AISmaker_pi(void *ppimgr)
      :opencpn_plugin_17 (ppimgr)
{
      // Create the PlugIn icons
      initialize_images();
	  m_bShowAISmaker = false;
}

AISmaker_pi::~AISmaker_pi(void)
{
     delete _img_AISmaker_pi;
     delete _img_AISmaker;
     
}

int AISmaker_pi::Init(void)
{
      AddLocaleCatalog( _T("opencpn-AISmaker_pi") );

      // Set some default private member parameters
      m_route_dialog_x = 0;
      m_route_dialog_y = 0;
      ::wxDisplaySize(&m_display_width, &m_display_height);

      //    Get a pointer to the opencpn display canvas, to use as a parent for the POI Manager dialog
      m_parent_window = GetOCPNCanvasWindow();

      //    Get a pointer to the opencpn configuration object
      m_pconfig = GetOCPNConfigObject();

      //    And load the configuration items
      LoadConfig();

      //    This PlugIn needs a toolbar icon, so request its insertion
	if(m_bAISmakerShowIcon)
      m_leftclick_tool_id  = InsertPlugInTool(_T(""), _img_AISmaker, _img_AISmaker, wxITEM_CHECK,
            _("AISmaker"), _T(""), NULL,
             CALCULATOR_TOOL_POSITION, 0, this);

      m_pDialog = NULL;

      return (WANTS_OVERLAY_CALLBACK |
              WANTS_OPENGL_OVERLAY_CALLBACK |		  
              WANTS_TOOLBAR_CALLBACK    |
              INSTALLS_TOOLBAR_TOOL     |
              WANTS_CONFIG             |
			  WANTS_PLUGIN_MESSAGING

           );
}

bool AISmaker_pi::DeInit(void)
{
      //    Record the dialog position
      if (NULL != m_pDialog)
      {
            //Capture dialog position
            wxPoint p = m_pDialog->GetPosition();
            SetCalculatorDialogX(p.x);
            SetCalculatorDialogY(p.y);
            m_pDialog->Close();
            delete m_pDialog;
            m_pDialog = NULL;

			m_bShowAISmaker = false;
			SetToolbarItemState( m_leftclick_tool_id, m_bShowAISmaker );

      }	
    
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh mainn window

    return true;
}

int AISmaker_pi::GetAPIVersionMajor()
{
      return MY_API_VERSION_MAJOR;
}

int AISmaker_pi::GetAPIVersionMinor()
{
      return MY_API_VERSION_MINOR;
}

int AISmaker_pi::GetPlugInVersionMajor()
{
      return PLUGIN_VERSION_MAJOR;
}

int AISmaker_pi::GetPlugInVersionMinor()
{
      return PLUGIN_VERSION_MINOR;
}

wxBitmap *AISmaker_pi::GetPlugInBitmap()
{
      return _img_AISmaker_pi;
}

wxString AISmaker_pi::GetCommonName()
{
      return _("AISmaker");
}


wxString AISmaker_pi::GetShortDescription()
{
      return _("AIS message player");
}

wxString AISmaker_pi::GetLongDescription()
{
      return _("Creates AIS Messages from text file");
}

int AISmaker_pi::GetToolbarToolCount(void)
{
      return 1;
}

void AISmaker_pi::SetColorScheme(PI_ColorScheme cs)
{
      if (NULL == m_pDialog)
            return;

      DimeWindow(m_pDialog);
}

void AISmaker_pi::OnToolbarToolCallback(int id)
{
    
	if(NULL == m_pDialog)
      {
            m_pDialog = new AisMaker(m_parent_window);
            m_pDialog->plugin = this;
            m_pDialog->Move(wxPoint(m_route_dialog_x, m_route_dialog_y));

			// Create the drawing factory
      }

	  m_pDialog->Fit();
	  //Toggle 
	  m_bShowAISmaker = !m_bShowAISmaker;	  

      //    Toggle dialog? 
      if(m_bShowAISmaker) {
          m_pDialog->Show();         
      } else
          m_pDialog->Hide();
     
      // Toggle is handled by the toolbar but we must keep plugin manager b_toggle updated
      // to actual status to ensure correct status upon toolbar rebuild
      SetToolbarItemState( m_leftclick_tool_id, m_bShowAISmaker );

      RequestRefresh(m_parent_window); // refresh main window
}

bool AISmaker_pi::LoadConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T( "/Settings/AISmaker_pi" ) );
			 pConf->Read ( _T( "ShowAISmakerIcon" ), &m_bAISmakerShowIcon, 1 );
           
            m_route_dialog_x =  pConf->Read ( _T ( "DialogPosX" ), 20L );
            m_route_dialog_y =  pConf->Read ( _T ( "DialogPosY" ), 20L );
         
            if((m_route_dialog_x < 0) || (m_route_dialog_x > m_display_width))
                  m_route_dialog_x = 5;
            if((m_route_dialog_y < 0) || (m_route_dialog_y > m_display_height))
                  m_route_dialog_y = 5;
            return true;
      }
      else
            return false;
}

bool AISmaker_pi::SaveConfig(void)
{
      wxFileConfig *pConf = (wxFileConfig *)m_pconfig;

      if(pConf)
      {
            pConf->SetPath ( _T ( "/Settings/AISmaker_pi" ) );
			pConf->Write ( _T ( "ShowAISmakerIcon" ), m_bAISmakerShowIcon );
          
            pConf->Write ( _T ( "DialogPosX" ),   m_route_dialog_x );
            pConf->Write ( _T ( "DialogPosY" ),   m_route_dialog_y );
            
            return true;
      }
      else
            return false;
}

void AISmaker_pi::OnAISmakerDialogClose()
{
    m_bShowAISmaker = false;
    SetToolbarItemState( m_leftclick_tool_id, m_bShowAISmaker );
    m_pDialog->Hide();
    SaveConfig();

    RequestRefresh(m_parent_window); // refresh main window

}

