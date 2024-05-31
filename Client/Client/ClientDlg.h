class CClientDlg : public CDialogEx
{
    // Construction
public:
    CClientDlg(CWnd* pParent = nullptr); // стандартный конструктор

// Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_CLIENT_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX); // поддержка DDX/DDV

// Implementation
protected:
    HICON m_hIcon;
    CEdit m_logCtrl;
    CButton m_connectButton;
    CButton m_sendButton;
    CButton m_disconnectButton;
    CButton m_cardButtons[10]; // Добавляем массив кнопок для карточек
    SOCKET clientSocket;
    sockaddr_in serverAddr;

    void ConnectToServer();
    void SendMessageToServer(CString message);
    void DisplayErrorMessage(CString msg);
    void AppendToLog(CString logMsg);
    void UpdateCardState(int cardIndex, bool revealed, int player);

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedConnectButton();
    afx_msg void OnBnClickedSendButton();
    afx_msg void OnBnClickedDisconnectButton();
    afx_msg void OnBnClickedCardButton(UINT nID);
};
