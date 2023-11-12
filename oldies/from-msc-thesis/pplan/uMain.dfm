object frmMain: TfrmMain
  Left = 98
  Top = 100
  BorderStyle = bsSingle
  Caption = 'DE-Demo'
  ClientHeight = 437
  ClientWidth = 801
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object imgViewer: TImage
    Left = 8
    Top = 8
    Width = 400
    Height = 400
    OnMouseDown = imgViewerMouseDown
    OnMouseMove = imgViewerMouseMove
  end
  object imgMap: TImage
    Left = 16
    Top = 16
    Width = 400
    Height = 400
    Visible = False
  end
  object Label7: TLabel
    Left = 8
    Top = 412
    Width = 169
    Height = 13
    Caption = 'Left Click: Start Configuration'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label8: TLabel
    Left = 220
    Top = 412
    Width = 187
    Height = 13
    Caption = 'Right Click: Target Configuration'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object PageControl1: TPageControl
    Left = 420
    Top = 8
    Width = 373
    Height = 401
    ActivePage = TabSheet2
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'Generale'
      object Label9: TLabel
        Left = 132
        Top = 44
        Width = 13
        Height = 13
        Caption = 'ms'
      end
      object Label20: TLabel
        Left = 8
        Top = 76
        Width = 3
        Height = 13
      end
      object lblTimeElapsed: TLabel
        Left = 8
        Top = 96
        Width = 3
        Height = 13
      end
      object btnLoadMap: TButton
        Left = 8
        Top = 8
        Width = 85
        Height = 25
        Caption = 'Carica mappa'
        TabOrder = 0
        OnClick = btnLoadMapClick
      end
      object CheckBox1: TCheckBox
        Left = 8
        Top = 40
        Width = 101
        Height = 21
        Caption = 'Timer attivo'
        TabOrder = 1
        OnClick = CheckBox1Click
      end
      object txtTimerInterval: TEdit
        Left = 88
        Top = 40
        Width = 41
        Height = 21
        TabOrder = 2
      end
      object Button1: TButton
        Left = 152
        Top = 40
        Width = 93
        Height = 25
        Caption = 'Passo per passo'
        TabOrder = 3
        OnClick = Button1Click
      end
      object rgMouseMove: TRadioGroup
        Left = 8
        Top = 136
        Width = 301
        Height = 105
        Caption = 'Passaggio del mouse'
        ItemIndex = 0
        Items.Strings = (
          'Nessuna azione'
          'Mostra Clearance nella posizione del puntatore'
          'Mostra Influence del nodo vicino al puntatore'
          'Il puntatore è un ostacolo')
        TabOrder = 4
      end
      object rgMouseClick: TRadioGroup
        Left = 8
        Top = 252
        Width = 301
        Height = 105
        Caption = 'Click del mouse'
        ItemIndex = 0
        Items.Strings = (
          'Sceglie Start e Target Configuration del path topologico'
          'Sposta l'#39'inizio e la fine dell'#39'Elastic Band'
          'Gruyere Test'
          'Aggiunge posizioni manualmente alla DPTM')
        TabOrder = 5
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'DPTM'
      ImageIndex = 1
      object GroupBox1: TGroupBox
        Left = 8
        Top = 8
        Width = 213
        Height = 85
        Caption = 'Statistiche'
        TabOrder = 0
        object Label1: TLabel
          Left = 9
          Top = 16
          Width = 3
          Height = 13
        end
        object Label2: TLabel
          Left = 9
          Top = 32
          Width = 3
          Height = 13
        end
        object Label3: TLabel
          Left = 9
          Top = 48
          Width = 3
          Height = 13
        end
        object Label13: TLabel
          Left = 9
          Top = 64
          Width = 3
          Height = 13
        end
      end
      object GroupBox4: TGroupBox
        Left = 8
        Top = 252
        Width = 261
        Height = 97
        Caption = 'Posizioni da dare in input alla rete'
        TabOrder = 1
        object Label6: TLabel
          Left = 12
          Top = 44
          Width = 173
          Height = 13
          Caption = 'Numero di pos. casuali ad ogni timer:'
        end
        object txtRandomInputs: TEdit
          Left = 192
          Top = 40
          Width = 41
          Height = 21
          TabOrder = 0
        end
        object chkRandomPosTimer: TCheckBox
          Left = 12
          Top = 20
          Width = 241
          Height = 17
          Caption = 'Aggiungi posizioni casuali ad ogni ciclo di timer'
          Checked = True
          State = cbChecked
          TabOrder = 1
        end
      end
      object btnReset: TButton
        Left = 280
        Top = 260
        Width = 77
        Height = 25
        Caption = 'Resetta rete'
        TabOrder = 2
        OnClick = btnResetClick
      end
      object GroupBox2: TGroupBox
        Left = 8
        Top = 100
        Width = 213
        Height = 145
        Caption = 'Parametri'
        TabOrder = 3
        object Label5: TLabel
          Left = 12
          Top = 44
          Width = 59
          Height = 13
          Caption = 'Link Radius:'
        end
        object Label10: TLabel
          Left = 12
          Top = 68
          Width = 58
          Height = 13
          Caption = 'L.F. Winner:'
        end
        object Label11: TLabel
          Left = 12
          Top = 92
          Width = 61
          Height = 13
          Caption = 'L.F. Second:'
        end
        object Label12: TLabel
          Left = 12
          Top = 116
          Width = 73
          Height = 13
          Caption = 'Max Edge Age:'
        end
        object txtLinkRadius: TEdit
          Left = 96
          Top = 40
          Width = 41
          Height = 21
          TabOrder = 0
        end
        object Button2: TButton
          Left = 144
          Top = 16
          Width = 59
          Height = 25
          Caption = 'Update'
          TabOrder = 1
          OnClick = Button2Click
        end
        object txtLFWinner: TEdit
          Left = 96
          Top = 64
          Width = 41
          Height = 21
          TabOrder = 2
        end
        object txtLFSecond: TEdit
          Left = 96
          Top = 88
          Width = 41
          Height = 21
          TabOrder = 3
        end
        object txtMaxEdgeAge: TEdit
          Left = 96
          Top = 112
          Width = 41
          Height = 21
          TabOrder = 4
        end
      end
      object GroupBox3: TGroupBox
        Left = 232
        Top = 8
        Width = 125
        Height = 181
        Caption = 'Visualizzazione'
        TabOrder = 4
        object chkDisplayNetwork: TCheckBox
          Left = 12
          Top = 20
          Width = 97
          Height = 17
          Caption = 'Mostra rete'
          TabOrder = 0
          OnClick = chkDisplayNetworkClick
        end
        object chkDisplayPath: TCheckBox
          Left = 12
          Top = 52
          Width = 109
          Height = 17
          Caption = 'Mostra path topol.'
          TabOrder = 1
          OnClick = chkDisplayNetworkClick
        end
        object chkDisplayClearance: TCheckBox
          Left = 12
          Top = 76
          Width = 109
          Height = 17
          Caption = 'Mostra Clearance'
          TabOrder = 2
          OnClick = chkDisplayNetworkClick
        end
        object chkDisplayInfluenceN: TCheckBox
          Left = 12
          Top = 100
          Width = 97
          Height = 17
          Caption = 'Valori Influence'
          TabOrder = 3
          OnClick = chkDisplayNetworkClick
        end
        object chkDisplayClearanceN: TCheckBox
          Left = 12
          Top = 116
          Width = 97
          Height = 17
          Caption = 'Valori Clearance'
          TabOrder = 4
          OnClick = chkDisplayNetworkClick
        end
        object Button5: TButton
          Left = 24
          Top = 140
          Width = 75
          Height = 25
          Caption = 'Refresh'
          TabOrder = 5
          OnClick = Button5Click
        end
      end
      object btnFindTopologicalPath: TButton
        Left = 232
        Top = 220
        Width = 125
        Height = 25
        Caption = 'Trova path topologico'
        TabOrder = 5
        OnClick = btnFindTopologicalPathClick
      end
      object CheckBox2: TCheckBox
        Left = 228
        Top = 200
        Width = 133
        Height = 17
        Caption = 'Nuove pos. iniz. e goal'
        Checked = True
        State = cbChecked
        TabOrder = 6
      end
      object Button6: TButton
        Left = 280
        Top = 300
        Width = 75
        Height = 25
        Caption = 'Evita incroci'
        TabOrder = 7
        OnClick = Button6Click
      end
      object Button8: TButton
        Left = 264
        Top = 340
        Width = 97
        Height = 25
        Caption = 'Rimuovi edges rid.'
        TabOrder = 8
        OnClick = Button8Click
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Elastic Stick'
      ImageIndex = 2
      object Label14: TLabel
        Left = 12
        Top = 148
        Width = 183
        Height = 13
        Caption = 'Numero iterazioni ad ogni ciclo di timer:'
      end
      object btnEBInitialize: TButton
        Left = 12
        Top = 12
        Width = 341
        Height = 25
        Caption = 'Inizializza l'#39'Elastic Stick con il Topological Path della DTM'
        TabOrder = 0
        OnClick = btnEBInitializeClick
      end
      object txtElasticStickIterations: TEdit
        Left = 204
        Top = 144
        Width = 33
        Height = 21
        TabOrder = 1
        Text = '10'
      end
      object GroupBox6: TGroupBox
        Left = 12
        Top = 172
        Width = 213
        Height = 85
        Caption = 'Statistiche'
        TabOrder = 2
        object Label15: TLabel
          Left = 9
          Top = 16
          Width = 3
          Height = 13
        end
        object Label16: TLabel
          Left = 9
          Top = 32
          Width = 3
          Height = 13
        end
        object Label17: TLabel
          Left = 9
          Top = 48
          Width = 3
          Height = 13
        end
        object Label18: TLabel
          Left = 9
          Top = 64
          Width = 3
          Height = 13
        end
      end
      object GroupBox7: TGroupBox
        Left = 12
        Top = 44
        Width = 225
        Height = 73
        Caption = 'Visualizzazione'
        TabOrder = 3
        object chkViewBubbles: TCheckBox
          Left = 12
          Top = 44
          Width = 97
          Height = 17
          Caption = 'Mostra bolle'
          Checked = True
          State = cbChecked
          TabOrder = 0
          OnClick = chkDisplayNetworkClick
        end
        object chkViewElasticStick: TCheckBox
          Left = 12
          Top = 20
          Width = 169
          Height = 17
          Caption = 'Mostra Elastic Stick Path'
          Checked = True
          State = cbChecked
          TabOrder = 1
          OnClick = chkDisplayNetworkClick
        end
      end
      object chkEBAdjust: TCheckBox
        Left = 12
        Top = 124
        Width = 201
        Height = 17
        Caption = 'Adatta la Elastic Band ad ogni timer'
        Checked = True
        State = cbChecked
        TabOrder = 4
      end
      object GroupBox8: TGroupBox
        Left = 12
        Top = 264
        Width = 285
        Height = 73
        Caption = 'Parametri'
        TabOrder = 5
        object Label19: TLabel
          Left = 12
          Top = 20
          Width = 142
          Height = 13
          Caption = 'Minima distanza dagli ostacoli:'
        end
        object Label4: TLabel
          Left = 12
          Top = 48
          Width = 127
          Height = 13
          Caption = 'Minimo raggio di curvatura:'
        end
        object txtMinObstacleDistance: TEdit
          Left = 160
          Top = 16
          Width = 41
          Height = 21
          TabOrder = 0
        end
        object Button7: TButton
          Left = 212
          Top = 16
          Width = 59
          Height = 25
          Caption = 'Update'
          TabOrder = 1
          OnClick = Button7Click
        end
        object txtMinSteeringRadius: TEdit
          Left = 160
          Top = 44
          Width = 41
          Height = 21
          TabOrder = 2
        end
      end
      object chkEBReqNewPath: TCheckBox
        Left = 16
        Top = 344
        Width = 325
        Height = 17
        Caption = 'Richiedi nuovo path se l'#39'Elastic Band si spezza'
        TabOrder = 6
      end
      object Button3: TButton
        Left = 252
        Top = 52
        Width = 95
        Height = 25
        Caption = '&Declare unsafe'
        TabOrder = 7
        OnClick = Button3Click
      end
      object Button4: TButton
        Left = 252
        Top = 88
        Width = 93
        Height = 25
        Caption = 'Clear unsafe'
        TabOrder = 8
        OnClick = Button4Click
      end
    end
  end
  object Timer1: TTimer
    Enabled = False
    Interval = 100
    OnTimer = Timer1Timer
    Left = 200
    Top = 208
  end
  object dlgOpen: TOpenDialog
    DefaultExt = 'BMP'
    Filter = 'Bitmap o Fig (*.bmp;*.fig)|*.bmp;*.fig'
    Left = 268
    Top = 60
  end
end
