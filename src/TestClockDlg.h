// TestClockDlg.h : ��� ����
//

#pragma once

#define HOUR_COUNT   12
#define MINUTE_COUNT 60

#define HOUR_HAND_POLYGON_COUNT   4
#define MINUTE_HAND_POLYGON_COUNT 4
#define SECOND_HAND_POLYGON_COUNT 4


// CTestClockDlg ��ȭ ����
class CTestClockDlg : public CDialog
{
// �����Դϴ�.
public:
	CTestClockDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TESTCLOCK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	DECLARE_MESSAGE_MAP()

private:
	CButton m_btnOk;     // 'OK' Button
	CButton m_btnCancel; // 'Cancel' Button

	CRect m_rectOrg;          // Original rect of CTestClockDlg
	CRect m_marginBtnOk;      // Margin value of the 'OK' button
	CRect m_marginBtnCancel;  // Margin value of the 'Cancel' button
	int m_nBtnAreaHeight;
	int m_nRadius;
	int m_nRadiusOrg;
	CTime m_timeCurrent;

	UINT_PTR m_nTimerId; // Timer ID

	BOOL m_bInit; // It is set to TRUE after WM_INITDIALOG is called.

	CPoint m_arrHourGrad[HOUR_COUNT];
	CPoint m_arrMinuteGrad[MINUTE_COUNT];
	CPoint m_pointHour[HOUR_HAND_POLYGON_COUNT];
	CPoint m_pointMinute[MINUTE_HAND_POLYGON_COUNT];
	CPoint m_pointSecondFrom;
	CPoint m_pointSecondTo;
	CPoint m_pointCenter;
	int m_nHourGradSize;
	int m_nMinuteGradSize;
	int m_nSecondHandSize;
	int m_nCenterPointSize;

	void SetMargin(LPCRECT rect, LPCRECT rectOrg, LPRECT margin);
	void SetRadius(int width, int height);
	void SetCenterPoint(int width, int height);

	int GetCenterHeight(int height);

	double GetSecondDegree(int second);
	double GetMinuteDegree(int minute, int second);
	double GetHourDegree(int hour, int minute, int second);

	double GetRadian(double degree);

	void SetCirclePoint(double radian, int radius, LPPOINT point);

	void SetGradationPoints();
	void SetClockHandPoints();
	void SetMinuteGradSize(int height);
};
