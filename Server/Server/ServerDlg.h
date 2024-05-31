#pragma once

#include <afxsock.h>

class CServerDlg : public CDialogEx
{
public:
    CServerDlg(CWnd* pParent = nullptr);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_SERVER_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;

    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnBnClickedStartButton();
    DECLARE_MESSAGE_MAP()

private:
    CSocket serverSocket;
    CSocket clientSocket1;
    CSocket clientSocket2;

    CEdit m_logCtrl;
    CButton m_startButton;

    struct GameState
    {
        bool cards[10];
        int pairsFound[2];
        int currentPlayer;
        int revealedCards[2];
    } gameState;

    void InitGame();
    void UpdateLog(CString logMsg);
    void SendToClient(CSocket& client, CString message);
    void HandleClientMessage(CSocket& client, CString message);
    void CheckPair();
    void EndGame();
};
