#include "pch.h"
#include "Server.h"
#include "ServerDlg.h"
#include "afxdialogex.h"
#include <afxsock.h>
#include <algorithm> // добавляем этот заголовок для использования std::all_of
#include <string> // добавляем этот заголовок для использования std::stgring


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_START_BUTTON, &CServerDlg::OnBnClickedStartButton)
END_MESSAGE_MAP()

CServerDlg::CServerDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_SERVER_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOG, m_logCtrl);
    DDX_Control(pDX, IDC_START_BUTTON, m_startButton);
}

BOOL CServerDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    AfxSocketInit();

    return TRUE;
}

void CServerDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CServerDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CServerDlg::OnBnClickedStartButton()
{
    if (!serverSocket.Create(12345) || !serverSocket.Listen())
    {
        AfxMessageBox(_T("Failed to start server"));
        return;
    }

    UpdateLog(_T("Server started. Waiting for clients..."));

    if (serverSocket.Accept(clientSocket1))
    {
        UpdateLog(_T("Client 1 connected"));
    }

    if (serverSocket.Accept(clientSocket2))
    {
        UpdateLog(_T("Client 2 connected"));
    }

    InitGame();
}

void CServerDlg::InitGame()
{
    gameState = {};
    for (int i = 0; i < 10; ++i)
    {
        gameState.cards[i] = false;
    }
    gameState.currentPlayer = 0;

    CString startMessage = _T("Game started. Player 1's turn.");
    SendToClient(clientSocket1, startMessage);
    SendToClient(clientSocket2, startMessage);
    UpdateLog(startMessage);
}

void CServerDlg::UpdateLog(CString logMsg)
{
    CString currentLog;
    m_logCtrl.GetWindowText(currentLog);
    if (!currentLog.IsEmpty())
    {
        currentLog += _T("\r\n");
    }
    currentLog += logMsg;
    m_logCtrl.SetWindowText(currentLog);
    m_logCtrl.LineScroll(m_logCtrl.GetLineCount());
}

void CServerDlg::SendToClient(CSocket& client, CString message)
{
    CT2CA pszConvertedAnsiString(message);
    std::string strStd(pszConvertedAnsiString);
    client.Send(strStd.c_str(), strStd.length());
}

void CServerDlg::HandleClientMessage(CSocket& client, CString message)
{
    if (message.Left(5) == _T("CLICK"))
    {
        int cardIndex = _ttoi(message.Mid(6));
        if (gameState.cards[cardIndex] || gameState.revealedCards[0] == cardIndex)
        {
            return;
        }

        gameState.revealedCards[gameState.revealedCards[0] == -1 ? 0 : 1] = cardIndex;

        if (gameState.revealedCards[1] != -1)
        {
            CheckPair();
        }

        CString updateMessage;
        updateMessage.Format(_T("REVEAL %d"), cardIndex);
        SendToClient(clientSocket1, updateMessage);
        SendToClient(clientSocket2, updateMessage);
        UpdateLog(updateMessage);
    }
}

void CServerDlg::CheckPair()
{
    int card1 = gameState.revealedCards[0];
    int card2 = gameState.revealedCards[1];

    if (card1 / 2 == card2 / 2)
    {
        gameState.pairsFound[gameState.currentPlayer]++;
        gameState.cards[card1] = true;
        gameState.cards[card2] = true;

        CString message;
        message.Format(_T("PAIR %d %d"), card1, card2);
        SendToClient(clientSocket1, message);
        SendToClient(clientSocket2, message);
        UpdateLog(message);
    }

    gameState.revealedCards[0] = gameState.revealedCards[1] = -1;
    gameState.currentPlayer = (gameState.currentPlayer + 1) % 2;

    if (std::all_of(std::begin(gameState.cards), std::end(gameState.cards), [](bool b) { return b; }))
    {
        EndGame();
    }
}

void CServerDlg::EndGame()
{
    CString message;
    if (gameState.pairsFound[0] > gameState.pairsFound[1])
    {
        message = _T("Player 1 wins!");
    }
    else if (gameState.pairsFound[1] > gameState.pairsFound[0])
    {
        message = _T("Player 2 wins!");
    }
    else
    {
        message = _T("It's a tie!");
    }
    SendToClient(clientSocket1, message);
    SendToClient(clientSocket2, message);
    UpdateLog(message);
}
