/////////////////////////////////////////////////////////////////////////////
// CAboutDlg Dialog
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVLinkApp commands
/////////////////////////////////////////////////////////////////////////////

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CDC	logoDC;

	if (logoDC.CreateCompatibleDC (&dc))
	{	
		CBitmap logoBM;
		if (logoBM.LoadBitmap (IDB_VLinkLogo2))
		{
			CBitmap *oldBitmap = logoDC.SelectObject (&logoBM);

			dc.BitBlt ( 0,  0, 240, 112, &logoDC, 0, 0, SRCCOPY);

			logoDC.SelectObject (oldBitmap);
			logoBM.DeleteObject ();
		}
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}
