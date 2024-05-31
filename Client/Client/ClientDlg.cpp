#include "pch.h"
#include "Client.h"
#include "ClientDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CClientDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_CONNECT_BUTTON, &CClientDlg::OnBnClickedConnectButton)
    ON_BN_CLICKED(IDC_SEND_BUTTON, &CClientDlg::OnBnClickedSendButton)
    ON_BN_CLICKED(IDC_DISCONNECT_BUTTON, &CClientDlg::OnBnClickedDisconnectButton)
    ON_CONTROL_RANGE(BN_CLICKED, IDC_CARD1, IDC_CARD10, &CClientDlg::OnBnClickedCardButton)
END_MESSAGE_MAP()

CClientDlg::CClientDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LOG, m_logCtrl);
    DDX_Control(pDX, IDC_CONNECT_BUTTON, m_connectButton);
    DDX_Control(pDX, IDC_SEND_BUTTON, m_sendButton);
    DDX_Control(pDX, IDC_DISCONNECT_BUTTON, m_disconnectButton);

    for (int i = 0; i < 10; ++i)
    {
        DDX_Control(pDX, IDC_CARD1 + i, m_cardButtons[i]);
    }
}

BOOL CClientDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        AfxMessageBox(_T("WSAStartup failed"));
        return FALSE;
    }

    return TRUE;
}

void CClientDlg::OnDestroy()
{
    CDialogEx::OnDestroy();

    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
    }

    WSACleanup();
}

void CClientDlg::OnBnClickedConnectButton()
{
    ConnectToServer();
}

void CClientDlg::OnBnClickedSendButton()
{
    CString message = _T("SEND");
    SendMessageToServer(message);
}

void CClientDlg::OnBnClickedDisconnectButton()
{
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        AppendToLog(_T("Disconnected from server"));
    }
}

void CClientDlg::ConnectToServer()
{
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        DisplayErrorMessage(_T("Socket creation failed"));
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP адрес сервера
    serverAddr.sin_port = htons(12345); // Порт сервера

    if (connect(clientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        DisplayErrorMessage(_T("Connect failed"));
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
        return;
    }

    AppendToLog(_T("Connected to server"));

    // Обновляем состояние кнопок
    m_connectButton.EnableWindow(FALSE);
    m_sendButton.EnableWindow(TRUE);
    m_disconnectButton.EnableWindow(TRUE);

}

void CClientDlg::SendMessageToServer(CString message)
{
    if (clientSocket == INVALID_SOCKET) {
        DisplayErrorMessage(_T("Client is not connected to server"));
        return;
    }

    CT2CA pszConvertedAnsiString(message);
    std::string strStd(pszConvertedAnsiString);

    int sendResult = send(clientSocket, strStd.c_str(), strStd.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        DisplayErrorMessage(_T("Send failed"));
        closesocket(clientSocket);
        return;
    }

    AppendToLog(_T("Sent: ") + message);
}

void CClientDlg::DisplayErrorMessage(CString msg)
{
    int errorCode = WSAGetLastError();
    CString errorMsg;
    errorMsg.Format(_T("%s (Error Code: %d)"), msg, errorCode);
    AppendToLog(errorMsg);
    AfxMessageBox(errorMsg);
}

void CClientDlg::AppendToLog(CString logMsg)
{
    CString currentLog;
    m_logCtrl.GetWindowText(currentLog);

    if (!currentLog.IsEmpty()) {
        currentLog += _T("\r\n");
    }

    currentLog += logMsg;
    m_logCtrl.SetWindowText(currentLog);

    m_logCtrl.LineScroll(m_logCtrl.GetLineCount());
}

void CClientDlg::OnBnClickedCardButton(UINT nID)
{
    int cardIndex = nID - IDC_CARD1;
    CString message;
    message.Format(_T("CLICK %d"), cardIndex);
    SendMessageToServer(message);
}
void CClientDlg::OnPaint()
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

HCURSOR CClientDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}
void CClientDlg::UpdateCardState(int cardIndex, bool revealed, int player)
{
    if (revealed)
    {
        if (player == 1)
        {
            m_cardButtons[cardIndex].SetButtonStyle(RGB(255, 0, 0)); // Красный для игрока 1
        }
        else
        {
            m_cardButtons[cardIndex].SetButtonStyle(RGB(0, 0, 255)); // Синий для игрока 2
        }
    }
    else
    {
        m_cardButtons[cardIndex].SetButtonStyle(RGB(255, 255, 255)); // Белый для скрытой карточки
    }
    Invalidate();
}
