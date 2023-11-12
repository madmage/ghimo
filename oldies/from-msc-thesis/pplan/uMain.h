//---------------------------------------------------------------------------

#ifndef uMainH
#define uMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>

#include "ppmap.h"
#include "dptm.h"
#include "elasticstick.h"
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TfrmMain : public TForm
{
__published:	// IDE-managed Components
    TImage *imgViewer;
    TTimer *Timer1;
    TImage *imgMap;
    TOpenDialog *dlgOpen;
    TLabel *Label7;
    TLabel *Label8;
    TPageControl *PageControl1;
    TTabSheet *TabSheet1;
    TTabSheet *TabSheet2;
    TTabSheet *TabSheet3;
    TButton *btnLoadMap;
    TGroupBox *GroupBox1;
    TLabel *Label1;
    TLabel *Label2;
    TLabel *Label3;
    TGroupBox *GroupBox4;
    TLabel *Label6;
    TEdit *txtRandomInputs;
    TButton *btnReset;
    TGroupBox *GroupBox2;
    TLabel *Label5;
    TEdit *txtLinkRadius;
    TButton *Button2;
    TGroupBox *GroupBox3;
    TCheckBox *chkDisplayNetwork;
    TCheckBox *chkDisplayPath;
    TCheckBox *chkDisplayClearance;
    TCheckBox *chkDisplayInfluenceN;
    TCheckBox *chkDisplayClearanceN;
    TButton *Button5;
    TLabel *Label10;
    TEdit *txtLFWinner;
    TLabel *Label11;
    TEdit *txtLFSecond;
    TLabel *Label12;
    TEdit *txtMaxEdgeAge;
    TButton *btnFindTopologicalPath;
    TLabel *Label13;
    TButton *btnEBInitialize;
    TLabel *Label14;
    TEdit *txtElasticStickIterations;
    TCheckBox *CheckBox1;
    TEdit *txtTimerInterval;
    TLabel *Label9;
    TButton *Button1;
    TCheckBox *chkRandomPosTimer;
    TGroupBox *GroupBox6;
    TLabel *Label15;
    TLabel *Label16;
    TLabel *Label17;
    TLabel *Label18;
    TGroupBox *GroupBox7;
    TCheckBox *chkViewBubbles;
    TCheckBox *chkViewElasticStick;
    TCheckBox *chkEBAdjust;
    TGroupBox *GroupBox8;
    TLabel *Label19;
    TEdit *txtMinObstacleDistance;
    TButton *Button7;
    TCheckBox *chkEBReqNewPath;
    TLabel *Label20;
    TLabel *lblTimeElapsed;
    TRadioGroup *rgMouseMove;
    TRadioGroup *rgMouseClick;
    TButton *Button3;
    TButton *Button4;
    TCheckBox *CheckBox2;
    TButton *Button6;
    TLabel *Label4;
    TEdit *txtMinSteeringRadius;
    TButton *Button8;
    void __fastcall Timer1Timer(TObject *Sender);
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall imgViewerMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnLoadMapClick(TObject *Sender);
    void __fastcall btnResetClick(TObject *Sender);
    void __fastcall Button1Click(TObject *Sender);
    void __fastcall CheckBox1Click(TObject *Sender);
    void __fastcall Button2Click(TObject *Sender);
    void __fastcall chkDisplayNetworkClick(TObject *Sender);
    void __fastcall Button5Click(TObject *Sender);
    void __fastcall imgViewerMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
    void __fastcall btnFindTopologicalPathClick(TObject *Sender);
    void __fastcall btnEBInitializeClick(TObject *Sender);
    void __fastcall Button7Click(TObject *Sender);
    void __fastcall Button3Click(TObject *Sender);
    void __fastcall Button4Click(TObject *Sender);
    void __fastcall Button6Click(TObject *Sender);
    void __fastcall Button8Click(TObject *Sender);
private:	// User declarations
    ppmap PPMap;
    dptm Dptm;
    elasticstick ElasticStick;
    ppmap::position StartPosition, TargetPosition;
    void __fastcall LoadMapBmp(AnsiString MapFileName);
    void __fastcall LoadMapFig(AnsiString MapFileName);
    void __fastcall RefreshViewer();
    int RandomInputs;
    std::list<ppmap::position> TopologicalPath;
public:		// User declarations
    __fastcall TfrmMain(TComponent* Owner);

    int InputX;
    int InputY;
    ppmap::position BridgeTest1, BridgeTest2;
    DWORD StartTicks;
    dptm::nodes_iterator Winner1, Winner2;
    dptm::nodes_iterator Redundant1, Redundant2;
    double PathCost;
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmMain *frmMain;
//---------------------------------------------------------------------------
#endif
