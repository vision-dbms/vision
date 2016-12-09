Public Class FindReplaceForm
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'UserControl overrides dispose to clean up the component list.
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
    Friend WithEvents FindLabel As System.Windows.Forms.Label
    Friend WithEvents FindTextBox As System.Windows.Forms.TextBox
    Friend WithEvents FindButton As System.Windows.Forms.Button
    Friend WithEvents DirectionGroupBox As System.Windows.Forms.GroupBox
    Friend WithEvents UpRadioButton As System.Windows.Forms.RadioButton
    Friend WithEvents MatchCaseCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents FindCancelButton As System.Windows.Forms.Button
    Friend WithEvents DownRadioButton As System.Windows.Forms.RadioButton
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(FindReplaceForm))
        Me.FindLabel = New System.Windows.Forms.Label
        Me.FindTextBox = New System.Windows.Forms.TextBox
        Me.FindButton = New System.Windows.Forms.Button
        Me.FindCancelButton = New System.Windows.Forms.Button
        Me.DirectionGroupBox = New System.Windows.Forms.GroupBox
        Me.DownRadioButton = New System.Windows.Forms.RadioButton
        Me.UpRadioButton = New System.Windows.Forms.RadioButton
        Me.MatchCaseCheckBox = New System.Windows.Forms.CheckBox
        Me.DirectionGroupBox.SuspendLayout()
        Me.SuspendLayout()
        '
        'FindLabel
        '
        Me.FindLabel.Location = New System.Drawing.Point(8, 8)
        Me.FindLabel.Name = "FindLabel"
        Me.FindLabel.Size = New System.Drawing.Size(72, 24)
        Me.FindLabel.TabIndex = 0
        Me.FindLabel.Text = "Find what:"
        '
        'FindTextBox
        '
        Me.FindTextBox.Anchor = CType(((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Left) _
                    Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.FindTextBox.Location = New System.Drawing.Point(88, 8)
        Me.FindTextBox.Name = "FindTextBox"
        Me.FindTextBox.Size = New System.Drawing.Size(168, 20)
        Me.FindTextBox.TabIndex = 1
        Me.FindTextBox.Text = ""
        '
        'FindButton
        '
        Me.FindButton.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.FindButton.Location = New System.Drawing.Point(264, 8)
        Me.FindButton.Name = "FindButton"
        Me.FindButton.TabIndex = 2
        Me.FindButton.Text = "Find"
        '
        'FindCancelButton
        '
        Me.FindCancelButton.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.FindCancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.FindCancelButton.Location = New System.Drawing.Point(264, 40)
        Me.FindCancelButton.Name = "FindCancelButton"
        Me.FindCancelButton.TabIndex = 3
        Me.FindCancelButton.Text = "Cancel"
        '
        'DirectionGroupBox
        '
        Me.DirectionGroupBox.Anchor = CType((System.Windows.Forms.AnchorStyles.Top Or System.Windows.Forms.AnchorStyles.Right), System.Windows.Forms.AnchorStyles)
        Me.DirectionGroupBox.Controls.Add(Me.DownRadioButton)
        Me.DirectionGroupBox.Controls.Add(Me.UpRadioButton)
        Me.DirectionGroupBox.Location = New System.Drawing.Point(136, 40)
        Me.DirectionGroupBox.Name = "DirectionGroupBox"
        Me.DirectionGroupBox.Size = New System.Drawing.Size(120, 48)
        Me.DirectionGroupBox.TabIndex = 4
        Me.DirectionGroupBox.TabStop = False
        Me.DirectionGroupBox.Text = "Direction"
        '
        'DownRadioButton
        '
        Me.DownRadioButton.Checked = True
        Me.DownRadioButton.Location = New System.Drawing.Point(56, 16)
        Me.DownRadioButton.Name = "DownRadioButton"
        Me.DownRadioButton.Size = New System.Drawing.Size(56, 24)
        Me.DownRadioButton.TabIndex = 1
        Me.DownRadioButton.TabStop = True
        Me.DownRadioButton.Text = "Down"
        '
        'UpRadioButton
        '
        Me.UpRadioButton.Location = New System.Drawing.Point(8, 16)
        Me.UpRadioButton.Name = "UpRadioButton"
        Me.UpRadioButton.Size = New System.Drawing.Size(48, 24)
        Me.UpRadioButton.TabIndex = 0
        Me.UpRadioButton.Text = "Up"
        '
        'MatchCaseCheckBox
        '
        Me.MatchCaseCheckBox.Location = New System.Drawing.Point(8, 64)
        Me.MatchCaseCheckBox.Name = "MatchCaseCheckBox"
        Me.MatchCaseCheckBox.TabIndex = 5
        Me.MatchCaseCheckBox.Text = "Match case"
        '
        'FindReplaceForm
        '
        Me.AcceptButton = Me.FindButton
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.CancelButton = Me.FindCancelButton
        Me.ClientSize = New System.Drawing.Size(344, 92)
        Me.Controls.Add(Me.MatchCaseCheckBox)
        Me.Controls.Add(Me.DirectionGroupBox)
        Me.Controls.Add(Me.FindCancelButton)
        Me.Controls.Add(Me.FindButton)
        Me.Controls.Add(Me.FindTextBox)
        Me.Controls.Add(Me.FindLabel)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "FindReplaceForm"
        Me.ShowInTaskbar = False
        Me.Text = "Find"
        Me.TopMost = True
        Me.DirectionGroupBox.ResumeLayout(False)
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private contextTextBox As TextBox

    Public Sub SetContext(ByRef textBox As TextBox)
        Me.contextTextBox = textBox
    End Sub

    Private Sub CancelButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FindCancelButton.Click
        Me.Hide()
    End Sub

    Private Sub FindButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FindButton.Click
        Dim foundIdx As Integer
        Dim needle As String
        Dim haystack As String
        If (Me.MatchCaseCheckBox.Checked) Then
            needle = Me.FindTextBox.Text
            haystack = Me.contextTextBox.Text
        Else
            needle = Me.FindTextBox.Text.ToLower()
            haystack = Me.contextTextBox.Text.ToLower()
        End If
        If (Me.DownRadioButton.Checked) Then
            foundIdx = haystack.IndexOf(needle, Me.contextTextBox.SelectionStart + Me.contextTextBox.SelectionLength)
        Else
            foundIdx = haystack.LastIndexOf(needle, Me.contextTextBox.SelectionStart)
        End If

        If (foundIdx > -1) Then
            Me.contextTextBox.SelectionStart = foundIdx
            Me.contextTextBox.SelectionLength = Me.FindTextBox.Text.Length
            Me.contextTextBox.ScrollToCaret()
        Else
            MsgBox("Cannot find '" & Me.FindTextBox.Text & "'.")
        End If
    End Sub
End Class
