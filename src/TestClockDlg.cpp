// TestClockDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TestClock.h"
#include "TestClockDlg.h"

#define _USE_MATH_DEFINES 1

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestClockDlg 대화 상자

#define TIMER_ID        1   // Timer ID
#define TIMER_INTERVAL  200 // Timer Interval

#define DEFAULT_HOUR_GRAD_SIZE       3  // Size of the hour gradation
#define DEFAULT_MINUTE_GRAD_SIZE     2  // Size of the minute gradation
#define DEFAULT_SECOND_HAND_SIZE     2  // Size of the second hand
#define DEFAULT_CENTER_POINT_SIZE    5  // Size of the center point

#define DEFAULT_HOUR_HAND_DEGREE     18.0 // Degree of the hour hand (for thicker hand)
#define DEFAULT_MINUTE_HAND_DEGREE   12.0 // Degree of the minute hand (for thicker hand)

CTestClockDlg::CTestClockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestClockDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nBtnAreaHeight = 0;
	m_nTimerId = 0;
	m_bInit = FALSE;
	m_nRadius = 0;
	m_nRadiusOrg = 0;
	m_nHourGradSize = 0;
	m_nMinuteGradSize = 0;
	m_nSecondHandSize = 0;
	m_nCenterPointSize = 0;
}

void CTestClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}

BEGIN_MESSAGE_MAP(CTestClockDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CTestClockDlg 메시지 처리기

BOOL CTestClockDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	// Get the margin information of the 'OK' and 'Cancel' button.

	CRect rectOrg;
	CRect rectOrgBtnOk;
	CRect rectOrgBtnCancel;

	GetClientRect(rectOrg);

	m_rectOrg = rectOrg;

	ClientToScreen(rectOrg);

	m_btnOk.GetWindowRect(rectOrgBtnOk);
	m_btnCancel.GetWindowRect(rectOrgBtnCancel);

	SetMargin(rectOrgBtnOk, rectOrg, m_marginBtnOk);
	SetMargin(rectOrgBtnCancel, rectOrg, m_marginBtnCancel);

	m_nBtnAreaHeight = m_marginBtnCancel.bottom * 2 + rectOrgBtnCancel.Height(); // Set the height of the button area

	SetRadius(rectOrg.Width(), rectOrg.Height());
	m_nRadiusOrg = m_nRadius;

	SetCenterPoint(rectOrg.Width(), rectOrg.Height());
	SetGradationPoints();

	m_nHourGradSize = DEFAULT_HOUR_GRAD_SIZE;
	m_nMinuteGradSize = DEFAULT_MINUTE_GRAD_SIZE;
	m_nSecondHandSize = DEFAULT_SECOND_HAND_SIZE;
	m_nCenterPointSize = DEFAULT_CENTER_POINT_SIZE;

	m_timeCurrent = CTime::GetCurrentTime();
	SetClockHandPoints();

	m_bInit = TRUE;

	m_nTimerId = SetTimer(TIMER_ID, TIMER_INTERVAL, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestClockDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dcPaint(this); // device context for painting

		CDC dc;
		if (dc.CreateCompatibleDC(&dcPaint))
		{
			CRect rect;
			GetClientRect(rect);

			CBitmap bitmap;
			if (bitmap.CreateCompatibleBitmap(&dcPaint, rect.Width(), rect.Height()))
			{
				CBitmap* pOldBitmap = dc.SelectObject(&bitmap);

				CBrush bgBrush;
				bgBrush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));

				dc.FillRect(rect, &bgBrush);

				// Draw the minute gradation

				CPen* pOldPen = static_cast<CPen*>(dc.SelectStockObject(BLACK_PEN));
				CBrush* pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(DKGRAY_BRUSH));

				int i;

				for (i = 0; i < MINUTE_COUNT; i++)
					dc.Ellipse(m_arrMinuteGrad[i].x - m_nMinuteGradSize, m_arrMinuteGrad[i].y - m_nMinuteGradSize,
						m_arrMinuteGrad[i].x + m_nMinuteGradSize, m_arrMinuteGrad[i].y + m_nMinuteGradSize);

				dc.SelectObject(pOldBrush);

				// Draw the hour gradation

				CPen bluePen;
				bluePen.CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

				pOldPen = dc.SelectObject(&bluePen);

				CBrush lightBlueBrush;
				lightBlueBrush.CreateSolidBrush(RGB(0, 128, 192));

				pOldBrush = dc.SelectObject(&lightBlueBrush);

				for (i = 0; i < HOUR_COUNT; i++)
					dc.Ellipse(m_arrHourGrad[i].x - m_nHourGradSize, m_arrHourGrad[i].y - m_nHourGradSize,
						m_arrHourGrad[i].x + m_nHourGradSize, m_arrHourGrad[i].y + m_nHourGradSize);

				dc.SelectObject(pOldBrush);
				dc.SelectObject(pOldPen);

				// Draw the hour hand

				pOldPen = static_cast<CPen*>(dc.SelectStockObject(BLACK_PEN));
				pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(LTGRAY_BRUSH));

				dc.Polygon(m_pointHour, HOUR_HAND_POLYGON_COUNT);

				dc.SelectObject(pOldBrush);

				// Draw the minute hand

				pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(GRAY_BRUSH));

				dc.Polygon(m_pointMinute, MINUTE_HAND_POLYGON_COUNT);

				dc.SelectObject(pOldBrush);
				dc.SelectObject(pOldPen);

				// Draw the second hand

				CPen penSecondHand;
				penSecondHand.CreatePen(PS_SOLID, m_nSecondHandSize, RGB(0, 0, 0));

				pOldPen = dc.SelectObject(&penSecondHand);

				dc.MoveTo(m_pointSecondFrom);
				dc.LineTo(m_pointSecondTo);

				// Draw the center point

				pOldBrush = static_cast<CBrush*>(dc.SelectStockObject(WHITE_BRUSH));

				dc.Ellipse(m_pointCenter.x - m_nCenterPointSize, m_pointCenter.y - m_nCenterPointSize,
					m_pointCenter.x + m_nCenterPointSize, m_pointCenter.y + m_nCenterPointSize);

				dc.SelectObject(pOldBrush);
				dc.SelectObject(pOldPen);

				dcPaint.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dc, 0, 0, SRCCOPY);

				dc.SelectObject(pOldBitmap);
			}
		}
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestClockDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestClockDlg::OnSize(UINT nType, int cx, int cy)
{
	if (m_bInit == TRUE)
	{
		SetRadius(cx, cy);
		SetCenterPoint(cx, cy);
		SetGradationPoints();

		m_nHourGradSize = MulDiv(DEFAULT_HOUR_GRAD_SIZE, m_nRadius, m_nRadiusOrg);
		m_nMinuteGradSize = MulDiv(DEFAULT_MINUTE_GRAD_SIZE, m_nRadius, m_nRadiusOrg);
		m_nSecondHandSize = MulDiv(DEFAULT_SECOND_HAND_SIZE, m_nRadius, m_nRadiusOrg);
		m_nCenterPointSize = MulDiv(DEFAULT_CENTER_POINT_SIZE, m_nRadius, m_nRadiusOrg);

		SetClockHandPoints();

		// Set position of the 'OK' and 'Cancel' button

		CRect rect;
		CRect rectBtnOk;
		CRect rectBtnOkToMove;
		CRect rectBtnCancel;
		CRect rectBtnCancelToMove;

		GetClientRect(rect);
		ClientToScreen(rect);

		m_btnOk.GetWindowRect(rectBtnOk);
		ScreenToClient(rectBtnOk);

		m_btnCancel.GetWindowRect(rectBtnCancel);
		ScreenToClient(rectBtnCancel);

		rectBtnOkToMove.right = rect.Width() - m_marginBtnOk.right;
		rectBtnOkToMove.bottom = rect.Height() - m_marginBtnOk.bottom;
		rectBtnOkToMove.left = rectBtnOkToMove.right - rectBtnOk.Width();
		rectBtnOkToMove.top = rectBtnOkToMove.bottom - rectBtnOk.Height();

		rectBtnCancelToMove.right = rect.Width() - m_marginBtnCancel.right;
		rectBtnCancelToMove.bottom = rect.Height() - m_marginBtnCancel.bottom;
		rectBtnCancelToMove.left = rectBtnCancelToMove.right - rectBtnCancel.Width();
		rectBtnCancelToMove.top = rectBtnCancelToMove.bottom - rectBtnCancel.Height();

		HDWP hdwp = ::BeginDeferWindowPos(2);
		if (hdwp)
			hdwp = DeferWindowPos(hdwp, m_btnOk.GetSafeHwnd(), HWND_TOP, rectBtnOkToMove.left, rectBtnOkToMove.top, rectBtnOkToMove.Width(), rectBtnOkToMove.Height(), SWP_NOZORDER); 
		if (hdwp)
			DeferWindowPos(hdwp, m_btnCancel.GetSafeHwnd(), HWND_TOP, rectBtnCancelToMove.left, rectBtnCancelToMove.top, rectBtnCancelToMove.Width(), rectBtnCancelToMove.Height(), SWP_NOZORDER); 
		if (hdwp)
			EndDeferWindowPos(hdwp);

		Invalidate();
		UpdateWindow();
	}

	CDialog::OnSize(nType, cx, cy);
}

void CTestClockDlg::OnOK()
{
	if (m_nTimerId != 0)
	{
		KillTimer(m_nTimerId);
		m_nTimerId = 0;
	}

	CDialog::OnOK();
}

void CTestClockDlg::OnCancel()
{
	if (m_nTimerId != 0)
	{
		KillTimer(m_nTimerId);
		m_nTimerId = 0;
	}

	CDialog::OnCancel();
}

void CTestClockDlg::SetMargin(LPCRECT rect, LPCRECT rectOrg, LPRECT margin)
{
	margin->left = rect->left - rectOrg->left;
	margin->top = rect->top - rectOrg->top;
	margin->right = rectOrg->right - rect->right;
	margin->bottom = rectOrg->bottom - rect->bottom;
}

void CTestClockDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == m_nTimerId)
	{
		CTime time = CTime::GetCurrentTime();

		if (m_timeCurrent != time)
		{
			m_timeCurrent = time;

			SetClockHandPoints();

			Invalidate();
			UpdateWindow();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CTestClockDlg::SetRadius(int width, int height)
{
	int clientHeight = height - m_nBtnAreaHeight;

	if (clientHeight < width)
		m_nRadius = (clientHeight - MulDiv(m_marginBtnCancel.bottom * 2, clientHeight, m_rectOrg.Height() - m_nBtnAreaHeight)) / 2;
	else
		m_nRadius = (width - MulDiv(m_marginBtnCancel.bottom * 2, width, m_rectOrg.Width())) / 2;
}

int CTestClockDlg::GetCenterHeight(int height)
{
	return (height - m_nBtnAreaHeight) / 2;
}

double CTestClockDlg::GetRadian(double degree)
{
	return degree / (double)180.0 * (double)M_PI;
}

double CTestClockDlg::GetSecondDegree(int second)
{
	return (double)(second * 6 - 90);
}

double CTestClockDlg::GetMinuteDegree(int minute, int second)
{
	return ((double)minute + (double)second / 60.0) * 6.0 - 90.0;
}

double CTestClockDlg::GetHourDegree(int hour, int minute, int second)
{
	return ((double)hour + (double)minute / 60.0 + (double)second / 3600.0) * 30.0 - 90.0;
}

void CTestClockDlg::SetCirclePoint(double radian, int radius, LPPOINT point)
{
	point->x = m_pointCenter.x + (int)((double)(radius) * cos(radian) + 0.5);
	point->y = m_pointCenter.y + (int)((double)(radius) * sin(radian) + 0.5);
}

void CTestClockDlg::SetGradationPoints()
{
	int i;

	for (i = 0; i < HOUR_COUNT; i++)
		SetCirclePoint(GetRadian(i * 30), m_nRadius, &m_arrHourGrad[i]);

	for (i = 0; i < MINUTE_COUNT; i++)
		SetCirclePoint(GetRadian(i * 6), m_nRadius, &m_arrMinuteGrad[i]);
}

void CTestClockDlg::SetCenterPoint(int width, int height)
{
	CRect rect(0, 0, width, height);
	rect.bottom -= m_nBtnAreaHeight;
	m_pointCenter = rect.CenterPoint();
}

void CTestClockDlg::SetClockHandPoints()
{
	// Set the point of the second hand
	double degree = GetSecondDegree(m_timeCurrent.GetSecond());

	SetCirclePoint(GetRadian(degree + 180.0), MulDiv(m_nRadius, 3, 20), &m_pointSecondFrom);
	SetCirclePoint(GetRadian(degree), MulDiv(m_nRadius, 19, 20), &m_pointSecondTo);

	// Set the point of the minute hand
	degree = GetMinuteDegree(m_timeCurrent.GetMinute(), m_timeCurrent.GetSecond());

	SetCirclePoint(GetRadian(degree + 180.0), MulDiv(m_nRadius, 1, 5), &m_pointMinute[0]);
	SetCirclePoint(GetRadian(degree - DEFAULT_MINUTE_HAND_DEGREE), MulDiv(m_nRadius, 1, 10), &m_pointMinute[1]);
	SetCirclePoint(GetRadian(degree), MulDiv(m_nRadius, 9, 10), &m_pointMinute[2]);
	SetCirclePoint(GetRadian(degree + DEFAULT_MINUTE_HAND_DEGREE), MulDiv(m_nRadius, 1, 10), &m_pointMinute[3]);

	// Set the point of the hour hand
	degree = GetHourDegree(m_timeCurrent.GetHour(), m_timeCurrent.GetMinute(), m_timeCurrent.GetSecond());

	SetCirclePoint(GetRadian(degree + 180.0), MulDiv(m_nRadius, 1, 5), &m_pointHour[0]);
	SetCirclePoint(GetRadian(degree - DEFAULT_HOUR_HAND_DEGREE), MulDiv(m_nRadius, 1, 10), &m_pointHour[1]);
	SetCirclePoint(GetRadian(degree), MulDiv(m_nRadius, 6, 10), &m_pointHour[2]);
	SetCirclePoint(GetRadian(degree + DEFAULT_HOUR_HAND_DEGREE), MulDiv(m_nRadius, 1, 10), &m_pointHour[3]);
}


BOOL CTestClockDlg::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


void CTestClockDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 300;

	CDialog::OnGetMinMaxInfo(lpMMI);
}
