Public Class MainForm
    Inherits System.Windows.Forms.Form

    Private myConnection As VACCESSLib.Connection
    Private myProtocolState As String
    Const queryCacheSize As Integer = 10
    Private Enum queryCacheIndices
        iQuery = 0
        iResult = 1
        iDate = 2
        iCurrency = 3
    End Enum
    Private queryCache(queryCacheSize, 4) As String
    Private queryCacheSelection(queryCacheSize, 2) As Integer
    Private queryCount As Integer = -1
    Private viewingQuery As Integer = -1
    Private oldestCachedQuery As Integer = 0
    Private focussedTextBox As TextBox
    Private findReplaceDialog As FindReplaceForm

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call
        Me.myConnection = New VACCESSLib.Connection
        Dim sessionNames As String = Me.myConnection.SessionNames
        If (sessionNames <> Nothing) Then
            Me.SessionComboBox.Items.AddRange(Split(sessionNames, Chr(10)))
        Else
            Me.SessionComboBox.Enabled = False
        End If
        Me.LoadPreferences()
    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents StatusBar As System.Windows.Forms.StatusBar
    Friend WithEvents Panel1 As System.Windows.Forms.Panel
    Friend WithEvents ResultsGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents ResultsTextBox As System.Windows.Forms.TextBox
    Friend WithEvents ConnectionGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents Splitter1 As System.Windows.Forms.Splitter
    Friend WithEvents QueryGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents QueryTextBox As System.Windows.Forms.TextBox
    Friend WithEvents DateLabel As System.Windows.Forms.Label
    Friend WithEvents DateTextBox As System.Windows.Forms.TextBox
    Friend WithEvents CurrencyTextBox As System.Windows.Forms.TextBox
    Friend WithEvents CurrencyLabel As System.Windows.Forms.Label
    Friend WithEvents ExecuteButton As System.Windows.Forms.Button
    Friend WithEvents Panel3 As System.Windows.Forms.Panel
    Friend WithEvents Panel4 As System.Windows.Forms.Panel
    Friend WithEvents ProtocolLabel As System.Windows.Forms.Label
    Friend WithEvents ProtocolComboBox As System.Windows.Forms.ComboBox
    Friend WithEvents HostLabel As System.Windows.Forms.Label
    Friend WithEvents UserLabel As System.Windows.Forms.Label
    Friend WithEvents PassLabel As System.Windows.Forms.Label
    Friend WithEvents HostTextBox As System.Windows.Forms.TextBox
    Friend WithEvents UserTextBox As System.Windows.Forms.TextBox
    Friend WithEvents PasswordTextBox As System.Windows.Forms.TextBox
    Friend WithEvents ConnectionOnOffButton As System.Windows.Forms.Button
    Friend WithEvents ConnectionOnOffPanel As System.Windows.Forms.Panel
    Friend WithEvents QueryOptionsPanel As System.Windows.Forms.Panel
    Friend WithEvents FileMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents FileExitMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SessionMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SessionConnectMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SessionDisconnectMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SessionExecuteMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem1 As System.Windows.Forms.MenuItem
    Friend WithEvents SessionPreviousQueryMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SessionNextQueryMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MainMenu1 As System.Windows.Forms.MainMenu
    Friend WithEvents SessionComboBox As System.Windows.Forms.ComboBox
    Friend WithEvents EditMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents EditCutMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents EditCopyMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents EditPasteMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents EditSelectAllMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem2 As System.Windows.Forms.MenuItem
    Friend WithEvents EditFindMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents QueryPanel As System.Windows.Forms.Panel
    Friend WithEvents FormatMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents FormatWordWrapMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents FormatFontMenuItem As System.Windows.Forms.MenuItem
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(MainForm))
        Me.StatusBar = New System.Windows.Forms.StatusBar
        Me.Panel1 = New System.Windows.Forms.Panel
        Me.QueryGroupBox = New System.Windows.Forms.GroupBox
        Me.QueryPanel = New System.Windows.Forms.Panel
        Me.QueryTextBox = New System.Windows.Forms.TextBox
        Me.QueryOptionsPanel = New System.Windows.Forms.Panel
        Me.DateLabel = New System.Windows.Forms.Label
        Me.DateTextBox = New System.Windows.Forms.TextBox
        Me.CurrencyTextBox = New System.Windows.Forms.TextBox
        Me.CurrencyLabel = New System.Windows.Forms.Label
        Me.ExecuteButton = New System.Windows.Forms.Button
        Me.Splitter1 = New System.Windows.Forms.Splitter
        Me.ResultsGroupBox = New System.Windows.Forms.GroupBox
        Me.ResultsTextBox = New System.Windows.Forms.TextBox
        Me.ConnectionGroupBox = New System.Windows.Forms.GroupBox
        Me.Panel3 = New System.Windows.Forms.Panel
        Me.SessionComboBox = New System.Windows.Forms.ComboBox
        Me.ConnectionOnOffPanel = New System.Windows.Forms.Panel
        Me.ConnectionOnOffButton = New System.Windows.Forms.Button
        Me.HostLabel = New System.Windows.Forms.Label
        Me.UserLabel = New System.Windows.Forms.Label
        Me.PassLabel = New System.Windows.Forms.Label
        Me.HostTextBox = New System.Windows.Forms.TextBox
        Me.UserTextBox = New System.Windows.Forms.TextBox
        Me.PasswordTextBox = New System.Windows.Forms.TextBox
        Me.Panel4 = New System.Windows.Forms.Panel
        Me.ProtocolLabel = New System.Windows.Forms.Label
        Me.ProtocolComboBox = New System.Windows.Forms.ComboBox
        Me.FileMenuItem = New System.Windows.Forms.MenuItem
        Me.FileExitMenuItem = New System.Windows.Forms.MenuItem
        Me.SessionMenuItem = New System.Windows.Forms.MenuItem
        Me.SessionConnectMenuItem = New System.Windows.Forms.MenuItem
        Me.SessionDisconnectMenuItem = New System.Windows.Forms.MenuItem
        Me.SessionExecuteMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem1 = New System.Windows.Forms.MenuItem
        Me.SessionPreviousQueryMenuItem = New System.Windows.Forms.MenuItem
        Me.SessionNextQueryMenuItem = New System.Windows.Forms.MenuItem
        Me.MainMenu1 = New System.Windows.Forms.MainMenu
        Me.EditMenuItem = New System.Windows.Forms.MenuItem
        Me.EditCutMenuItem = New System.Windows.Forms.MenuItem
        Me.EditCopyMenuItem = New System.Windows.Forms.MenuItem
        Me.EditPasteMenuItem = New System.Windows.Forms.MenuItem
        Me.EditSelectAllMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem2 = New System.Windows.Forms.MenuItem
        Me.EditFindMenuItem = New System.Windows.Forms.MenuItem
        Me.FormatMenuItem = New System.Windows.Forms.MenuItem
        Me.FormatWordWrapMenuItem = New System.Windows.Forms.MenuItem
        Me.FormatFontMenuItem = New System.Windows.Forms.MenuItem
        Me.Panel1.SuspendLayout()
        Me.QueryGroupBox.SuspendLayout()
        Me.QueryPanel.SuspendLayout()
        Me.QueryOptionsPanel.SuspendLayout()
        Me.ResultsGroupBox.SuspendLayout()
        Me.ConnectionGroupBox.SuspendLayout()
        Me.Panel3.SuspendLayout()
        Me.ConnectionOnOffPanel.SuspendLayout()
        Me.Panel4.SuspendLayout()
        Me.SuspendLayout()
        '
        'StatusBar
        '
        Me.StatusBar.Location = New System.Drawing.Point(0, 590)
        Me.StatusBar.Name = "StatusBar"
        Me.StatusBar.Size = New System.Drawing.Size(744, 22)
        Me.StatusBar.TabIndex = 11
        '
        'Panel1
        '
        Me.Panel1.Controls.Add(Me.QueryGroupBox)
        Me.Panel1.Controls.Add(Me.Splitter1)
        Me.Panel1.Controls.Add(Me.ResultsGroupBox)
        Me.Panel1.Controls.Add(Me.ConnectionGroupBox)
        Me.Panel1.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel1.DockPadding.All = 8
        Me.Panel1.Location = New System.Drawing.Point(0, 0)
        Me.Panel1.Name = "Panel1"
        Me.Panel1.Size = New System.Drawing.Size(744, 590)
        Me.Panel1.TabIndex = 12
        '
        'QueryGroupBox
        '
        Me.QueryGroupBox.Controls.Add(Me.QueryPanel)
        Me.QueryGroupBox.Dock = System.Windows.Forms.DockStyle.Fill
        Me.QueryGroupBox.Location = New System.Drawing.Point(8, 56)
        Me.QueryGroupBox.Name = "QueryGroupBox"
        Me.QueryGroupBox.Size = New System.Drawing.Size(728, 281)
        Me.QueryGroupBox.TabIndex = 105
        Me.QueryGroupBox.TabStop = False
        Me.QueryGroupBox.Text = "Query"
        '
        'QueryPanel
        '
        Me.QueryPanel.Controls.Add(Me.QueryTextBox)
        Me.QueryPanel.Controls.Add(Me.QueryOptionsPanel)
        Me.QueryPanel.Dock = System.Windows.Forms.DockStyle.Fill
        Me.QueryPanel.Location = New System.Drawing.Point(3, 16)
        Me.QueryPanel.Name = "QueryPanel"
        Me.QueryPanel.Size = New System.Drawing.Size(722, 262)
        Me.QueryPanel.TabIndex = 9
        '
        'QueryTextBox
        '
        Me.QueryTextBox.AcceptsReturn = True
        Me.QueryTextBox.AcceptsTab = True
        Me.QueryTextBox.Dock = System.Windows.Forms.DockStyle.Fill
        Me.QueryTextBox.Font = New System.Drawing.Font("Lucida Console", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.QueryTextBox.HideSelection = False
        Me.QueryTextBox.Location = New System.Drawing.Point(0, 0)
        Me.QueryTextBox.Multiline = True
        Me.QueryTextBox.Name = "QueryTextBox"
        Me.QueryTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.QueryTextBox.Size = New System.Drawing.Size(722, 230)
        Me.QueryTextBox.TabIndex = 4
        Me.QueryTextBox.Text = ""
        Me.QueryTextBox.WordWrap = False
        '
        'QueryOptionsPanel
        '
        Me.QueryOptionsPanel.Controls.Add(Me.DateLabel)
        Me.QueryOptionsPanel.Controls.Add(Me.DateTextBox)
        Me.QueryOptionsPanel.Controls.Add(Me.CurrencyTextBox)
        Me.QueryOptionsPanel.Controls.Add(Me.CurrencyLabel)
        Me.QueryOptionsPanel.Controls.Add(Me.ExecuteButton)
        Me.QueryOptionsPanel.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.QueryOptionsPanel.DockPadding.Bottom = 5
        Me.QueryOptionsPanel.DockPadding.Top = 6
        Me.QueryOptionsPanel.Location = New System.Drawing.Point(0, 230)
        Me.QueryOptionsPanel.Name = "QueryOptionsPanel"
        Me.QueryOptionsPanel.Size = New System.Drawing.Size(722, 32)
        Me.QueryOptionsPanel.TabIndex = 8
        '
        'DateLabel
        '
        Me.DateLabel.Location = New System.Drawing.Point(-1, 8)
        Me.DateLabel.Name = "DateLabel"
        Me.DateLabel.Size = New System.Drawing.Size(32, 16)
        Me.DateLabel.TabIndex = 9
        Me.DateLabel.Text = "Date:"
        Me.DateLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'DateTextBox
        '
        Me.DateTextBox.Location = New System.Drawing.Point(31, 8)
        Me.DateTextBox.Name = "DateTextBox"
        Me.DateTextBox.TabIndex = 8
        Me.DateTextBox.Text = ""
        '
        'CurrencyTextBox
        '
        Me.CurrencyTextBox.Location = New System.Drawing.Point(199, 8)
        Me.CurrencyTextBox.Name = "CurrencyTextBox"
        Me.CurrencyTextBox.TabIndex = 10
        Me.CurrencyTextBox.Text = ""
        '
        'CurrencyLabel
        '
        Me.CurrencyLabel.Location = New System.Drawing.Point(135, 8)
        Me.CurrencyLabel.Name = "CurrencyLabel"
        Me.CurrencyLabel.Size = New System.Drawing.Size(56, 16)
        Me.CurrencyLabel.TabIndex = 12
        Me.CurrencyLabel.Text = "Currency:"
        Me.CurrencyLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'ExecuteButton
        '
        Me.ExecuteButton.Dock = System.Windows.Forms.DockStyle.Right
        Me.ExecuteButton.Enabled = False
        Me.ExecuteButton.Location = New System.Drawing.Point(642, 6)
        Me.ExecuteButton.Name = "ExecuteButton"
        Me.ExecuteButton.Size = New System.Drawing.Size(80, 21)
        Me.ExecuteButton.TabIndex = 11
        Me.ExecuteButton.Text = "Execute"
        '
        'Splitter1
        '
        Me.Splitter1.BackColor = System.Drawing.SystemColors.Control
        Me.Splitter1.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.Splitter1.Location = New System.Drawing.Point(8, 337)
        Me.Splitter1.MinExtra = 100
        Me.Splitter1.MinSize = 50
        Me.Splitter1.Name = "Splitter1"
        Me.Splitter1.Size = New System.Drawing.Size(728, 5)
        Me.Splitter1.TabIndex = 104
        Me.Splitter1.TabStop = False
        '
        'ResultsGroupBox
        '
        Me.ResultsGroupBox.Controls.Add(Me.ResultsTextBox)
        Me.ResultsGroupBox.Dock = System.Windows.Forms.DockStyle.Bottom
        Me.ResultsGroupBox.Location = New System.Drawing.Point(8, 342)
        Me.ResultsGroupBox.Name = "ResultsGroupBox"
        Me.ResultsGroupBox.Size = New System.Drawing.Size(728, 240)
        Me.ResultsGroupBox.TabIndex = 103
        Me.ResultsGroupBox.TabStop = False
        Me.ResultsGroupBox.Text = "Results"
        '
        'ResultsTextBox
        '
        Me.ResultsTextBox.AutoSize = False
        Me.ResultsTextBox.Dock = System.Windows.Forms.DockStyle.Fill
        Me.ResultsTextBox.Font = New System.Drawing.Font("Lucida Console", 10.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.ResultsTextBox.HideSelection = False
        Me.ResultsTextBox.Location = New System.Drawing.Point(3, 16)
        Me.ResultsTextBox.Multiline = True
        Me.ResultsTextBox.Name = "ResultsTextBox"
        Me.ResultsTextBox.ReadOnly = True
        Me.ResultsTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Both
        Me.ResultsTextBox.Size = New System.Drawing.Size(722, 221)
        Me.ResultsTextBox.TabIndex = 8
        Me.ResultsTextBox.Text = ""
        Me.ResultsTextBox.WordWrap = False
        '
        'ConnectionGroupBox
        '
        Me.ConnectionGroupBox.Controls.Add(Me.Panel3)
        Me.ConnectionGroupBox.Controls.Add(Me.Panel4)
        Me.ConnectionGroupBox.Dock = System.Windows.Forms.DockStyle.Top
        Me.ConnectionGroupBox.Location = New System.Drawing.Point(8, 8)
        Me.ConnectionGroupBox.Name = "ConnectionGroupBox"
        Me.ConnectionGroupBox.Size = New System.Drawing.Size(728, 48)
        Me.ConnectionGroupBox.TabIndex = 101
        Me.ConnectionGroupBox.TabStop = False
        Me.ConnectionGroupBox.Text = "Connection"
        '
        'Panel3
        '
        Me.Panel3.Controls.Add(Me.SessionComboBox)
        Me.Panel3.Controls.Add(Me.ConnectionOnOffPanel)
        Me.Panel3.Controls.Add(Me.HostLabel)
        Me.Panel3.Controls.Add(Me.UserLabel)
        Me.Panel3.Controls.Add(Me.PassLabel)
        Me.Panel3.Controls.Add(Me.HostTextBox)
        Me.Panel3.Controls.Add(Me.UserTextBox)
        Me.Panel3.Controls.Add(Me.PasswordTextBox)
        Me.Panel3.Dock = System.Windows.Forms.DockStyle.Fill
        Me.Panel3.Location = New System.Drawing.Point(184, 16)
        Me.Panel3.Name = "Panel3"
        Me.Panel3.Size = New System.Drawing.Size(541, 29)
        Me.Panel3.TabIndex = 4
        '
        'SessionComboBox
        '
        Me.SessionComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.SessionComboBox.Location = New System.Drawing.Point(48, 4)
        Me.SessionComboBox.Name = "SessionComboBox"
        Me.SessionComboBox.Size = New System.Drawing.Size(232, 21)
        Me.SessionComboBox.TabIndex = 18
        Me.SessionComboBox.Visible = False
        '
        'ConnectionOnOffPanel
        '
        Me.ConnectionOnOffPanel.Controls.Add(Me.ConnectionOnOffButton)
        Me.ConnectionOnOffPanel.Dock = System.Windows.Forms.DockStyle.Right
        Me.ConnectionOnOffPanel.DockPadding.Bottom = 5
        Me.ConnectionOnOffPanel.DockPadding.Top = 3
        Me.ConnectionOnOffPanel.Location = New System.Drawing.Point(461, 0)
        Me.ConnectionOnOffPanel.Name = "ConnectionOnOffPanel"
        Me.ConnectionOnOffPanel.Size = New System.Drawing.Size(80, 29)
        Me.ConnectionOnOffPanel.TabIndex = 17
        '
        'ConnectionOnOffButton
        '
        Me.ConnectionOnOffButton.Dock = System.Windows.Forms.DockStyle.Fill
        Me.ConnectionOnOffButton.Location = New System.Drawing.Point(0, 3)
        Me.ConnectionOnOffButton.Name = "ConnectionOnOffButton"
        Me.ConnectionOnOffButton.Size = New System.Drawing.Size(80, 21)
        Me.ConnectionOnOffButton.TabIndex = 4
        Me.ConnectionOnOffButton.Text = "Connect"
        '
        'HostLabel
        '
        Me.HostLabel.Location = New System.Drawing.Point(8, 6)
        Me.HostLabel.Name = "HostLabel"
        Me.HostLabel.Size = New System.Drawing.Size(40, 16)
        Me.HostLabel.TabIndex = 15
        Me.HostLabel.Text = "Host"
        Me.HostLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'UserLabel
        '
        Me.UserLabel.Location = New System.Drawing.Point(152, 6)
        Me.UserLabel.Name = "UserLabel"
        Me.UserLabel.Size = New System.Drawing.Size(32, 16)
        Me.UserLabel.TabIndex = 16
        Me.UserLabel.Text = "User"
        Me.UserLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'PassLabel
        '
        Me.PassLabel.Location = New System.Drawing.Point(288, 6)
        Me.PassLabel.Name = "PassLabel"
        Me.PassLabel.Size = New System.Drawing.Size(64, 16)
        Me.PassLabel.TabIndex = 14
        Me.PassLabel.Text = "Password"
        Me.PassLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'HostTextBox
        '
        Me.HostTextBox.Location = New System.Drawing.Point(48, 4)
        Me.HostTextBox.Name = "HostTextBox"
        Me.HostTextBox.TabIndex = 11
        Me.HostTextBox.Text = ""
        '
        'UserTextBox
        '
        Me.UserTextBox.Location = New System.Drawing.Point(184, 4)
        Me.UserTextBox.Name = "UserTextBox"
        Me.UserTextBox.TabIndex = 12
        Me.UserTextBox.Text = ""
        '
        'PasswordTextBox
        '
        Me.PasswordTextBox.Location = New System.Drawing.Point(360, 4)
        Me.PasswordTextBox.Name = "PasswordTextBox"
        Me.PasswordTextBox.PasswordChar = Microsoft.VisualBasic.ChrW(183)
        Me.PasswordTextBox.TabIndex = 13
        Me.PasswordTextBox.Text = ""
        '
        'Panel4
        '
        Me.Panel4.Controls.Add(Me.ProtocolLabel)
        Me.Panel4.Controls.Add(Me.ProtocolComboBox)
        Me.Panel4.Dock = System.Windows.Forms.DockStyle.Left
        Me.Panel4.Location = New System.Drawing.Point(3, 16)
        Me.Panel4.Name = "Panel4"
        Me.Panel4.Size = New System.Drawing.Size(181, 29)
        Me.Panel4.TabIndex = 5
        '
        'ProtocolLabel
        '
        Me.ProtocolLabel.Location = New System.Drawing.Point(0, 6)
        Me.ProtocolLabel.Name = "ProtocolLabel"
        Me.ProtocolLabel.Size = New System.Drawing.Size(56, 16)
        Me.ProtocolLabel.TabIndex = 17
        Me.ProtocolLabel.Text = "Protocol"
        Me.ProtocolLabel.TextAlign = System.Drawing.ContentAlignment.BottomRight
        '
        'ProtocolComboBox
        '
        Me.ProtocolComboBox.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.ProtocolComboBox.Items.AddRange(New Object() {"SSH", "RExec", "Plain", "Session"})
        Me.ProtocolComboBox.Location = New System.Drawing.Point(64, 4)
        Me.ProtocolComboBox.Name = "ProtocolComboBox"
        Me.ProtocolComboBox.Size = New System.Drawing.Size(112, 21)
        Me.ProtocolComboBox.TabIndex = 18
        '
        'FileMenuItem
        '
        Me.FileMenuItem.Index = 0
        Me.FileMenuItem.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.FileExitMenuItem})
        Me.FileMenuItem.Text = "&File"
        '
        'FileExitMenuItem
        '
        Me.FileExitMenuItem.Index = 0
        Me.FileExitMenuItem.Text = "&Exit"
        '
        'SessionMenuItem
        '
        Me.SessionMenuItem.Index = 3
        Me.SessionMenuItem.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.SessionConnectMenuItem, Me.SessionDisconnectMenuItem, Me.SessionExecuteMenuItem, Me.MenuItem1, Me.SessionPreviousQueryMenuItem, Me.SessionNextQueryMenuItem})
        Me.SessionMenuItem.Text = "&Session"
        '
        'SessionConnectMenuItem
        '
        Me.SessionConnectMenuItem.Index = 0
        Me.SessionConnectMenuItem.Text = "&Connect"
        '
        'SessionDisconnectMenuItem
        '
        Me.SessionDisconnectMenuItem.Enabled = False
        Me.SessionDisconnectMenuItem.Index = 1
        Me.SessionDisconnectMenuItem.Text = "&Disconnect"
        '
        'SessionExecuteMenuItem
        '
        Me.SessionExecuteMenuItem.Enabled = False
        Me.SessionExecuteMenuItem.Index = 2
        Me.SessionExecuteMenuItem.Shortcut = System.Windows.Forms.Shortcut.F2
        Me.SessionExecuteMenuItem.Text = "&Execute"
        '
        'MenuItem1
        '
        Me.MenuItem1.Index = 3
        Me.MenuItem1.Text = "-"
        '
        'SessionPreviousQueryMenuItem
        '
        Me.SessionPreviousQueryMenuItem.Enabled = False
        Me.SessionPreviousQueryMenuItem.Index = 4
        Me.SessionPreviousQueryMenuItem.Text = "&Previous Query"
        '
        'SessionNextQueryMenuItem
        '
        Me.SessionNextQueryMenuItem.Enabled = False
        Me.SessionNextQueryMenuItem.Index = 5
        Me.SessionNextQueryMenuItem.Text = "&Next Query"
        '
        'MainMenu1
        '
        Me.MainMenu1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.FileMenuItem, Me.EditMenuItem, Me.FormatMenuItem, Me.SessionMenuItem})
        '
        'EditMenuItem
        '
        Me.EditMenuItem.Index = 1
        Me.EditMenuItem.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.EditCutMenuItem, Me.EditCopyMenuItem, Me.EditPasteMenuItem, Me.EditSelectAllMenuItem, Me.MenuItem2, Me.EditFindMenuItem})
        Me.EditMenuItem.Text = "&Edit"
        '
        'EditCutMenuItem
        '
        Me.EditCutMenuItem.Enabled = False
        Me.EditCutMenuItem.Index = 0
        Me.EditCutMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlX
        Me.EditCutMenuItem.Text = "Cu&t"
        '
        'EditCopyMenuItem
        '
        Me.EditCopyMenuItem.Enabled = False
        Me.EditCopyMenuItem.Index = 1
        Me.EditCopyMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlC
        Me.EditCopyMenuItem.Text = "&Copy"
        '
        'EditPasteMenuItem
        '
        Me.EditPasteMenuItem.Enabled = False
        Me.EditPasteMenuItem.Index = 2
        Me.EditPasteMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlV
        Me.EditPasteMenuItem.Text = "&Paste"
        '
        'EditSelectAllMenuItem
        '
        Me.EditSelectAllMenuItem.Enabled = False
        Me.EditSelectAllMenuItem.Index = 3
        Me.EditSelectAllMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlA
        Me.EditSelectAllMenuItem.Text = "Select &All"
        '
        'MenuItem2
        '
        Me.MenuItem2.Index = 4
        Me.MenuItem2.Text = "-"
        '
        'EditFindMenuItem
        '
        Me.EditFindMenuItem.Index = 5
        Me.EditFindMenuItem.Shortcut = System.Windows.Forms.Shortcut.CtrlF
        Me.EditFindMenuItem.Text = "&Find"
        '
        'FormatMenuItem
        '
        Me.FormatMenuItem.Index = 2
        Me.FormatMenuItem.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.FormatWordWrapMenuItem, Me.FormatFontMenuItem})
        Me.FormatMenuItem.Text = "Format"
        '
        'FormatWordWrapMenuItem
        '
        Me.FormatWordWrapMenuItem.Index = 0
        Me.FormatWordWrapMenuItem.Text = "Word Wrap"
        '
        'FormatFontMenuItem
        '
        Me.FormatFontMenuItem.Index = 1
        Me.FormatFontMenuItem.Text = "Font..."
        '
        'MainForm
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(744, 612)
        Me.Controls.Add(Me.Panel1)
        Me.Controls.Add(Me.StatusBar)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Menu = Me.MainMenu1
        Me.MinimumSize = New System.Drawing.Size(752, 600)
        Me.Name = "MainForm"
        Me.Text = "VAccess Session"
        Me.Panel1.ResumeLayout(False)
        Me.QueryGroupBox.ResumeLayout(False)
        Me.QueryPanel.ResumeLayout(False)
        Me.QueryOptionsPanel.ResumeLayout(False)
        Me.ResultsGroupBox.ResumeLayout(False)
        Me.ConnectionGroupBox.ResumeLayout(False)
        Me.Panel3.ResumeLayout(False)
        Me.ConnectionOnOffPanel.ResumeLayout(False)
        Me.Panel4.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub Log(ByVal message As String)
        Me.StatusBar.Text = message
    End Sub

    Private Function IsConnected() As Boolean
        Return (Me.myConnection.IsConnected > 0)
    End Function

    Private Sub Connect()
        If Me.IsConnected() Then
            Me.Disconnect()
        End If
        Me.Log("Connecting...")
        Dim protocol As String = Me.ProtocolComboBox.Text.ToLower
        Dim host As String = Me.HostTextBox.Text
        Dim user As String = Me.UserTextBox.Text
        Dim pass As String = Me.PasswordTextBox.Text
        Dim command As String = "batchvision"
        Dim result As String
        If protocol = "plain" Then
            Dim port As Integer = Convert.ToInt32(user) ' Our user field becomes our port field by ugly hackery on combo box change. TODO: should be Integer rather than Double
            Me.myConnection.Connect(host, port)
        Else
            If protocol = "" Then
                protocol = "ssh"
            ElseIf protocol = "session" Then
                host = Me.SessionComboBox.Text
                user = ""
                pass = ""
            End If
            Me.myConnection.Login(protocol + "://" + host, user, pass, command)
        End If
        Me.ConnectionOnOffButton.Text = "Disconnect"
        Me.SessionConnectMenuItem.Enabled = False
        Me.ProtocolComboBox.Enabled = False
        Me.HostTextBox.Enabled = False
        Me.UserTextBox.Enabled = False
        Me.PasswordTextBox.Enabled = False
        Me.SessionComboBox.Enabled = False
        Me.SessionDisconnectMenuItem.Enabled = True
        Me.ExecuteButton.Enabled = True
        Me.SessionExecuteMenuItem.Enabled = True
        Me.Log("Connected.")
        Me.Text = host & " - VAccess Session"
    End Sub

    Private Sub OnClose(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        Me.Disconnect()
    End Sub

    Private Sub Disconnect()
        If Not Me.myConnection Is Nothing Then
            Me.Log("Disconnecting...")
            Me.myConnection.Disconnect()
            Me.Log("Disconnected.")
        End If
        Me.ConnectionOnOffButton.Text = "Connect"
        Me.SessionConnectMenuItem.Enabled = True
        Me.ProtocolComboBox.Enabled = True
        Me.HostTextBox.Enabled = True
        Me.UserTextBox.Enabled = True
        Me.PasswordTextBox.Enabled = True
        Me.SessionComboBox.Enabled = True
        Me.SessionDisconnectMenuItem.Enabled = False
        Me.ExecuteButton.Enabled = False
        Me.SessionExecuteMenuItem.Enabled = False
        Me.Text = "VAccess Session"
    End Sub

    Private Function RunQuery(ByVal queryString As String, ByVal dateString As String, ByVal currencyString As String) As String
        On Error GoTo HandleError

        ' Ensure that we're connected.
        If Not Me.IsConnected() Then
            MsgBox("Connection required but not found.")
            Exit Function
        End If

        ' Run the query
        Me.Log("Running query...")
        Dim result = Me.myConnection.Submit(queryString, dateString, currencyString)
        Me.Log("Ran query.")

        ' Return result.
        If (result = Nothing) Then Return "(No result.)"
        Return result.Replace(Chr(10), Chr(13) + Chr(10))

        Exit Function
HandleError:
        MsgBox("Error (" & Hex$(Err.Number) & ") from " & Err.Source & ": " & Err.Description)
        Log("Error encountered executing query.")
    End Function

    Private Function ValidateConnectInput() As Boolean
        If (Me.myProtocolState = "session") Then
            If (Me.SessionComboBox.Text = "") Then
                Return False
            Else
                Return True
            End If
        End If
        If (Me.HostTextBox.Text = "") Then Return False
        If (Me.UserTextBox.Text = "") Then Return False
        If (Me.myProtocolState = "plain") Then Return True
        If (Me.PasswordTextBox.Text = "") Then Return False
        Return True
    End Function

    Private Sub ExecuteButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SessionExecuteMenuItem.Click, ExecuteButton.Click
        On Error GoTo HandleError

        ' Get Query Parameters
        Dim queryString
        If (Me.QueryTextBox.SelectionLength > 0) Then
            queryString = Me.QueryTextBox.SelectedText
        Else
            queryString = Me.QueryTextBox.Text
        End If
        Dim dateString = Me.DateTextBox.Text
        If (dateString Is "") Then
            dateString = Nothing
        End If
        Dim currencyString = Me.CurrencyTextBox.Text
        If (currencyString Is "") Then
            currencyString = Nothing
        End If

        ' Submit query.
        Dim result = Me.RunQuery(queryString, dateString, currencyString)

        ' Cache query and results.
        Me.queryCount += 1
        Dim queryQueuePos = queryCount Mod Me.queryCacheSize
        Me.queryCache(queryQueuePos, Me.queryCacheIndices.iQuery) = Me.QueryTextBox.Text
        Me.queryCache(queryQueuePos, Me.queryCacheIndices.iResult) = result
        Me.queryCache(queryQueuePos, Me.queryCacheIndices.iDate) = dateString
        Me.queryCache(queryQueuePos, Me.queryCacheIndices.iCurrency) = currencyString
        Me.queryCacheSelection(queryQueuePos, 0) = Me.QueryTextBox.SelectionStart
        Me.queryCacheSelection(queryQueuePos, 1) = Me.QueryTextBox.SelectionLength
        If (Me.queryCount >= Me.queryCacheSize) Then
            Me.oldestCachedQuery = Me.queryCount - Me.queryCacheSize + 1
        End If
        ' TODO: Also cache date/currency values?

        ' Display result in results text box.
        Me.ViewQueryCache(queryCount, True)

        Exit Sub
HandleError:
        MsgBox("Error (" & Hex$(Err.Number) & ") from " & Err.Source & ": " & Err.Description)
        Log("Error encountered processing query.")
    End Sub

    Private Sub ConnectionDetails_Enter(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles HostTextBox.KeyPress, UserTextBox.KeyPress, PasswordTextBox.KeyPress
        If Asc(e.KeyChar) = Keys.Enter Then
            ConnectionOnOffButton_Click(sender, e)
        End If
    End Sub

    Private Sub ConnectionOnOffButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ConnectionOnOffButton.Click, SessionConnectMenuItem.Click, SessionDisconnectMenuItem.Click ' , UserTextBox.Enter, PasswordTextBox.Enter
        On Error GoTo HandleError
        If Not Me.IsConnected() Then
            If ValidateConnectInput() Then
                Me.Connect()
            Else
                MsgBox("Invalid connection details.")
            End If
        Else
            Me.Disconnect()
        End If
        Exit Sub
HandleError:
        MsgBox("Connectivity Error (" & Hex$(Err.Number) & ") from " & Err.Source & ": " & Err.Description)
        Log("Connectivity error encountered.")
    End Sub

    Private Sub ProtocolComboBox_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ProtocolComboBox.SelectedIndexChanged
        Dim protocol = Me.ProtocolComboBox.Text.ToLower
        If Me.myProtocolState = protocol Then Return
        If (protocol = "plain") Then
            ' Switch to Port by hackery.
            Me.HostLabel.Text = "Host:"
            Me.HostTextBox.Visible = True
            Me.UserLabel.Text = "Port:"
            Me.UserTextBox.Text = ""
            Me.UserLabel.Visible = True
            Me.UserTextBox.Visible = True
            Me.PassLabel.Visible = False
            Me.PasswordTextBox.Visible = False
            Me.SessionComboBox.Visible = False
        ElseIf (protocol = "session") Then
            ' Switch to Name by hackery.
            Me.HostLabel.Text = "Name:"
            Me.HostTextBox.Visible = False
            Me.UserTextBox.Text = ""
            Me.UserLabel.Visible = False
            Me.UserTextBox.Visible = False
            Me.PassLabel.Visible = False
            Me.PasswordTextBox.Visible = False
            Me.SessionComboBox.Visible = True
        ElseIf (Me.myProtocolState <> "ssh" And Me.myProtocolState <> "rexec") Then
            ' Switch to User/Pass by hackery.
            Me.HostLabel.Text = "Host:"
            Me.HostTextBox.Visible = True
            Me.UserLabel.Text = "User:"
            Me.UserTextBox.Text = ""
            Me.UserLabel.Visible = True
            Me.UserTextBox.Visible = True
            Me.PassLabel.Visible = True
            Me.PasswordTextBox.Visible = True
            Me.SessionComboBox.Visible = False
        End If
        Me.myProtocolState = protocol
    End Sub

    Private Sub FileExitMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FileExitMenuItem.Click
        Me.Close()
    End Sub

    Private Sub SessionPreviousQueryMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SessionPreviousQueryMenuItem.Click
        Me.ViewQueryCache(Me.viewingQuery - 1)
    End Sub

    Private Sub SessionNextQueryMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SessionNextQueryMenuItem.Click
        Me.ViewQueryCache(Me.viewingQuery + 1)
    End Sub

    Private Sub ViewQueryCache(ByVal query As Integer, Optional ByVal onlyResult As Boolean = False)
        ' Sanitize arguments.
        If (query < Me.oldestCachedQuery) Then
            MsgBox("Sorry, my query history doesn't go back that far.")
            Exit Sub
        End If
        If (query > Me.queryCount Or query < 0) Then
            MsgBox("Error: Attempted to retrieve invalid query (" & query & ") from history.")
            Exit Sub
        End If

        ' Find the correct index in the queue.
        Dim cacheIndex = query Mod Me.queryCacheSize

        ' Show the correct query/result.
        If (Not onlyResult) Then
            Me.QueryTextBox.Text = Me.queryCache(cacheIndex, Me.queryCacheIndices.iQuery)
            Me.QueryTextBox.SelectionStart = Me.queryCacheSelection(cacheIndex, 0)
            Me.QueryTextBox.SelectionLength = Me.queryCacheSelection(cacheIndex, 1)
            Me.DateTextBox.Text = Me.queryCache(cacheIndex, Me.queryCacheIndices.iDate)
            Me.CurrencyTextBox.Text = Me.queryCache(cacheIndex, Me.queryCacheIndices.iCurrency)
        End If
        Me.ResultsTextBox.Text = Me.queryCache(cacheIndex, Me.queryCacheIndices.iResult)

        ' Update viewingQuery.
        Me.viewingQuery = query

        ' Enable/disable next/previous buttons.
        If (Me.viewingQuery = Me.queryCount) Then
            Me.SessionNextQueryMenuItem.Enabled = False
        Else
            Me.SessionNextQueryMenuItem.Enabled = True
        End If
        If (Me.viewingQuery = Me.oldestCachedQuery) Then
            Me.SessionPreviousQueryMenuItem.Enabled = False
        Else
            Me.SessionPreviousQueryMenuItem.Enabled = True
        End If
    End Sub

    Private Sub TextBox_Enter(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CurrencyTextBox.Enter, DateTextBox.Enter, HostTextBox.Enter, PasswordTextBox.Enter, QueryTextBox.Enter, ResultsTextBox.Enter, UserTextBox.Enter
        Me.focussedTextBox = sender
        Me.EditCopyMenuItem.Enabled = True
        Me.EditSelectAllMenuItem.Enabled = True
        If (Me.focussedTextBox.ReadOnly) Then
            Exit Sub
        End If
        Me.EditCutMenuItem.Enabled = True
        Me.EditPasteMenuItem.Enabled = True
    End Sub

    Private Sub TextBox_Leave(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CurrencyTextBox.Leave, DateTextBox.Leave, HostTextBox.Leave, PasswordTextBox.Leave, QueryTextBox.Leave, ResultsTextBox.Leave, UserTextBox.Leave
        Me.focussedTextBox = Nothing
        Me.EditCopyMenuItem.Enabled = False
        Me.EditCutMenuItem.Enabled = False
        Me.EditPasteMenuItem.Enabled = False
        Me.EditSelectAllMenuItem.Enabled = False
    End Sub

    Private Sub EditCutMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles EditCutMenuItem.Click
        Me.focussedTextBox.Cut()
    End Sub

    Private Sub EditCopyMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles EditCopyMenuItem.Click
        Me.focussedTextBox.Copy()
    End Sub

    Private Sub EditPasteMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles EditPasteMenuItem.Click
        Me.focussedTextBox.Paste()
    End Sub

    Private Sub EditSelectAllMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles EditSelectAllMenuItem.Click
        Me.focussedTextBox.SelectAll()
    End Sub

    Private Sub EditFindMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles EditFindMenuItem.Click
        If (Me.findReplaceDialog Is Nothing) Then
            Me.findReplaceDialog = New FindReplaceForm
        End If
        Me.findReplaceDialog.SetContext(Me.focussedTextBox)
        If (Me.focussedTextBox Is Me.QueryTextBox) Then
            Me.findReplaceDialog.Text = "Find in Query"
        ElseIf (Me.focussedTextBox Is Me.ResultsTextBox) Then
            Me.findReplaceDialog.Text = "Find in Result"
        End If
        Me.findReplaceDialog.Show()
    End Sub

    Private Sub FormatWordWrapMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FormatWordWrapMenuItem.Click
        Dim wrapping As Boolean = Not Me.FormatWordWrapMenuItem.Checked
        Me.FormatWordWrapMenuItem.Checked = wrapping
        Me.QueryTextBox.WordWrap = wrapping
        Me.ResultsTextBox.WordWrap = wrapping
        If wrapping Then
            Me.QueryTextBox.ScrollBars = ScrollBars.Vertical
            Me.ResultsTextBox.ScrollBars = ScrollBars.Vertical
        Else
            Me.QueryTextBox.ScrollBars = ScrollBars.Both
            Me.ResultsTextBox.ScrollBars = ScrollBars.Both
        End If
    End Sub

    Private Sub FormatFontMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FormatFontMenuItem.Click
        Dim d As FontDialog
        d = New FontDialog
        d.Font = Me.QueryTextBox.Font
        d.ShowEffects = False
        If (d.ShowDialog <> DialogResult.Cancel) Then
            Me.QueryTextBox.Font = d.Font
            Me.ResultsTextBox.Font = d.Font
            Me.SavePreference("FontFamily", d.Font.FontFamily.GetName(0))
            Me.SavePreference("FontSize", Convert.ToString(d.Font.Size))
            Me.SavePreference("FontStyle", d.Font.Style.ToString)
        End If
    End Sub

    Private Sub SavePreference(ByVal name As String, ByVal value As String)
        SaveSetting("VAccessSession", "UserPreferences", name, value)
    End Sub

    Private Function LoadPreference(ByVal name, ByVal defaultValue) As String
        Return GetSetting("VAccessSession", "UserPreferences", name, defaultValue)
    End Function

    Private Sub LoadPreferences()
        Dim newFontFamily As String = Me.LoadPreference("FontFamily", "Lucida Console")
        Dim newFontSize As Single = Convert.ToSingle(Me.LoadPreference("FontSize", "10"))
        Dim newFontStyle As System.Drawing.FontStyle = [Enum].Parse(GetType(System.Drawing.FontStyle), Me.LoadPreference("FontStyle", System.Drawing.FontStyle.Regular.ToString))
        Dim newFont As System.Drawing.Font = New System.Drawing.Font(newFontFamily, newFontSize, newFontStyle)
        Me.QueryTextBox.Font = newFont
        Me.ResultsTextBox.Font = newFont
    End Sub
End Class
