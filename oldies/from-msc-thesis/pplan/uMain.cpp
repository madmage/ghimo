//---------------------------------------------------------------------------
#include <vcl.h>
#include <list>
#include <utility>
#include <vector>
using namespace std;
#pragma hdrstop

#include "ppmap.h"
#include "dptm.h"
#include "simple_fig_loader.h"
#include "elasticstick.h"
#include "uMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmMain *frmMain;
//---------------------------------------------------------------------------
__fastcall TfrmMain::TfrmMain(TComponent* Owner)
    : TForm(Owner), StartTicks(0)
{
    Dptm.setPPMap(PPMap);
    ElasticStick.setPPMap(PPMap);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::RefreshViewer()
{
    BitBlt(imgViewer->Canvas->Handle, 0, 0, imgViewer->Width, imgViewer->Height,
        imgMap->Canvas->Handle, 0, 0, SRCCOPY);

    imgViewer->Canvas->Ellipse(0, 0, 1, 1);   // mah... FIXME?

    double rx = (double) imgViewer->Width / (PPMap.getMaxX() - PPMap.getMinX());
    double ry = (double) imgViewer->Width / (PPMap.getMaxY() - PPMap.getMinY());

    Label1->Caption = "Nodi: " + (AnsiString) Dptm.Nodes.size() + "; Archi: " + (AnsiString) Dptm.Edges.size();

    TCanvas* c = imgViewer->Canvas;

    if (chkDisplayNetwork->Checked) {
        if (chkDisplayClearance->Checked) {
            dptm::nodes_iterator it = Dptm.Nodes.begin();
            for ( ; it != Dptm.Nodes.end(); ++it) {
                c->Pen->Color = clWhite;
                c->Brush->Color = TColor(0xAAFFAA);
                c->Ellipse((it->X - it->Clearance - PPMap.getMinX()) * rx, (it->Y - it->Clearance - PPMap.getMinY()) * ry,
                           (it->X + it->Clearance - PPMap.getMinX()) * rx, (it->Y + it->Clearance - PPMap.getMinY()) * ry);
            }
        }

        dptm::edges_iterator it2 = Dptm.Edges.begin();
        for ( ; it2 != Dptm.Edges.end(); ++it2) {
            if (it2->Unsafe) c->Pen->Color = clRed;
            else c->Pen->Color = clBlue;
            c->MoveTo((it2->First->X - PPMap.getMinX()) * rx, (it2->First->Y - PPMap.getMinY()) * ry);
            c->LineTo((it2->Second->X - PPMap.getMinX()) * rx, (it2->Second->Y - PPMap.getMinY()) * ry);
        }

        dptm::nodes_iterator it = Dptm.Nodes.begin();
        for ( ; it != Dptm.Nodes.end(); ++it) {
            c->Pen->Color = clBlack;
            c->Brush->Color = clYellow;
            if (RandomInputs == 1 && (it == Redundant1 || it == Redundant2)) c->Brush->Color = clGreen;
            else if (RandomInputs == 1 && it == Winner1) c->Brush->Color = clBlack;
            else if (RandomInputs == 1 && it == Winner2) c->Brush->Color = clGray;
            c->Ellipse((it->X - PPMap.getMinX()) * rx - 4, (it->Y - PPMap.getMinY()) * ry - 4,
                (it->X - PPMap.getMinX()) * rx + 4, (it->Y - PPMap.getMinY()) * ry + 4);

            AnsiString s;
            c->Brush->Color = TColor(0xAAFFFF);
            if (chkDisplayInfluenceN->Checked) {
                s.sprintf("%.2f", it->Influence);
                c->TextOut((it->X - PPMap.getMinX()) * rx + 5, (it->Y - PPMap.getMinY()) * ry + 5, s);
            }
            c->Brush->Color = TColor(0xAAFFAA);
            if (chkDisplayClearanceN->Checked) {
                s.sprintf("%.2f", it->Clearance);
                c->TextOut((it->X - PPMap.getMinX()) * rx + 5, (it->Y - PPMap.getMinY()) * ry + 5, s);
            }
        }
    }

    if (RandomInputs == 1) {
        c->Pen->Color = clRed;
        c->Pen->Width = 3;
        c->MoveTo((this->InputX - PPMap.getMinX()) * rx - 5, (this->InputY - PPMap.getMinY()) * ry - 5);
        c->LineTo((this->InputX - PPMap.getMinX()) * rx + 5, (this->InputY - PPMap.getMinY()) * ry + 5);
        c->MoveTo((this->InputX - PPMap.getMinX()) * rx + 5, (this->InputY - PPMap.getMinY()) * ry - 5);
        c->LineTo((this->InputX - PPMap.getMinX()) * rx - 5, (this->InputY - PPMap.getMinY()) * ry + 5);
        c->Pen->Width = 1;

        c->Pen->Color = clRed;
        c->MoveTo((BridgeTest1.X - PPMap.getMinX()) * rx, (BridgeTest1.Y - PPMap.getMinY()) * ry);
        c->LineTo((BridgeTest2.X - PPMap.getMinX()) * rx, (BridgeTest2.Y - PPMap.getMinY()) * ry);
    }

    DWORD before, after;
    if (chkDisplayPath->Checked) {
        c->Pen->Color = clGreen;
        c->Pen->Width = 2;

        for (list<ppmap::position>::iterator it = TopologicalPath.begin(); it != TopologicalPath.end(); ++it) {
            if (it == TopologicalPath.begin())
                c->MoveTo((it->X - PPMap.getMinX()) * rx, (it->Y - PPMap.getMinY()) * ry);
            else
                c->LineTo((it->X - PPMap.getMinX()) * rx, (it->Y - PPMap.getMinY()) * ry);
        }

        c->Pen->Width = 1;

        c->Pen->Color = clGreen;
        c->Pen->Width = 2;
        c->Brush->Color = clWhite;
        c->Ellipse((StartPosition.X - PPMap.getMinX()) * rx - 4, (StartPosition.Y - PPMap.getMinY()) * ry - 4,
                   (StartPosition.X - PPMap.getMinX()) * rx + 4, (StartPosition.Y - PPMap.getMinY()) * ry + 4);

        c->Brush->Color = clGreen;
        c->Ellipse((TargetPosition.X - PPMap.getMinX()) * rx - 4, (TargetPosition.Y - PPMap.getMinY()) * ry - 4,
                   (TargetPosition.X - PPMap.getMinX()) * rx + 4, (TargetPosition.Y - PPMap.getMinY()) * ry + 4);

        c->Pen->Width = 1;
    }

    if (chkViewElasticStick->Checked) {
        if (chkViewBubbles->Checked) {
            imgViewer->Canvas->Pen->Width = 1;
            imgViewer->Canvas->Brush->Style = bsClear;
            for (list<elasticstick::node>::iterator it = ElasticStick.Stick.begin();
            it != ElasticStick.Stick.end(); ++it) {
                if (it->BubbleRadius < ElasticStick.ParamMinObstDist) {
                    imgViewer->Canvas->Pen->Color = clRed;
                }
                else {
                    imgViewer->Canvas->Pen->Color = clLime;
                }
                imgViewer->Canvas->Ellipse((it->X - it->BubbleRadius - PPMap.getMinX()) * rx,
                                           (it->Y - it->BubbleRadius - PPMap.getMinY()) * ry,
                                           (it->X + it->BubbleRadius - PPMap.getMinX()) * rx,
                                           (it->Y + it->BubbleRadius - PPMap.getMinY()) * ry);
            }
            imgViewer->Canvas->Brush->Style = bsSolid;
        }

        imgViewer->Canvas->Pen->Color = clFuchsia;
        imgViewer->Canvas->Pen->Width = 2;
        for (list<elasticstick::node>::iterator lnit = ElasticStick.Stick.begin();
        lnit != ElasticStick.Stick.end(); ++lnit) {
            if (lnit == ElasticStick.Stick.begin())
                imgViewer->Canvas->MoveTo((lnit->X - PPMap.getMinX()) * rx, (lnit->Y - PPMap.getMinY()) * ry);
            else
                imgViewer->Canvas->LineTo((lnit->X - PPMap.getMinX()) * rx, (lnit->Y - PPMap.getMinY()) * ry);
            imgViewer->Canvas->Ellipse((lnit->X - PPMap.getMinX()) * rx - 2, (lnit->Y - PPMap.getMinY()) * ry - 2,
                                       (lnit->X - PPMap.getMinX()) * rx + 2, (lnit->Y - PPMap.getMinY()) * ry + 2);
        }

        imgViewer->Canvas->Pen->Width = 1;

        c->Pen->Color = clFuchsia;
        c->Pen->Width = 2;
        c->Brush->Color = clWhite;
        c->Ellipse((ElasticStick.Stick.front().X - PPMap.getMinX()) * rx - 4,
                   (ElasticStick.Stick.front().Y - PPMap.getMinY()) * ry - 4,
                   (ElasticStick.Stick.front().X - PPMap.getMinX()) * rx + 4,
                   (ElasticStick.Stick.front().Y - PPMap.getMinY()) * ry + 4);

        c->Brush->Color = clFuchsia;
        c->Ellipse((ElasticStick.Stick.back().X - PPMap.getMinX()) * rx - 4,
                   (ElasticStick.Stick.back().Y - PPMap.getMinY()) * ry - 4,
                   (ElasticStick.Stick.back().X - PPMap.getMinX()) * rx + 4,
                   (ElasticStick.Stick.back().Y - PPMap.getMinY()) * ry + 4);

        c->Pen->Width = 1;
    }

    if (rgMouseClick->ItemIndex == 2) {
        vector<ppmap::position> poss = PPMap.gruyereTest(StartPosition, TargetPosition);
        c->Pen->Color = clRed;
        c->MoveTo((StartPosition.X - PPMap.getMinX()) * rx, (StartPosition.Y - PPMap.getMinY()) * ry);
        c->LineTo((TargetPosition.X - PPMap.getMinX()) * rx, (TargetPosition.Y - PPMap.getMinY()) * ry);

        c->Brush->Color = clRed;
        for (unsigned int i = 0; i < poss.size(); i++) {
            c->Ellipse((poss[i].X - PPMap.getMinX()) * rx - 4,
                       (poss[i].Y - PPMap.getMinY()) * ry - 4,
                       (poss[i].X - PPMap.getMinX()) * rx + 4,
                       (poss[i].Y - PPMap.getMinY()) * ry + 4);
        }
    }
}

void __fastcall TfrmMain::Timer1Timer(TObject *Sender)
{
    DWORD globalbefore = GetTickCount();
    if (chkRandomPosTimer->Checked) {
        DWORD before, after;
        before = GetTickCount();
        Dptm.randomPositions(RandomInputs);
        after = GetTickCount();

        Label2->Caption = (AnsiString) "Tempo per " + RandomInputs + " pos. random (ms): " + (AnsiString) ((double) (after - before));
    }

    if (chkEBAdjust->Checked) {
        DWORD before, after;
        before = GetTickCount();
        bool ret = ElasticStick.adjust(txtElasticStickIterations->Text.ToIntDef(0));
        after = GetTickCount();

        if (chkEBReqNewPath->Checked) {
            if (!ret && ElasticStick.CriticalBubblePtr != 0) {
                // the stick hasn't broken, but there's a critical bubble
                double MAX_DIST = PPMap.getMaxX() + PPMap.getMaxY() - PPMap.getMinX() - PPMap.getMinY();
                dptm::nodes_iterator winner_iterator = Dptm.Nodes.end();
                dptm::nodes_iterator second_winner_iterator = Dptm.Nodes.end();
                dptm::nodes_iterator it = Dptm.Nodes.begin();
                double best_distance = MAX_DIST;
                double second_best_distance = MAX_DIST;
                for ( ; it != Dptm.Nodes.end(); ) {
                    if (PPMap.posSight(*ElasticStick.CriticalBubblePtr, *it)) {
                        // we check only nodes visible from this configuration
                        double distance = PPMap.posDist(*ElasticStick.CriticalBubblePtr, *it);
                        if (distance < best_distance) {
                            second_best_distance = best_distance;
                            second_winner_iterator = winner_iterator;
                            best_distance = distance;
                            winner_iterator = it;
                        }
                        else if (distance < second_best_distance) {
                            second_best_distance = distance;
                            second_winner_iterator = it;
                        }
                    }
                    ++it;
                }
                if (winner_iterator != Dptm.Nodes.end() && second_winner_iterator != Dptm.Nodes.end()) {
                    dptm::edge* edge_ptr = Dptm.findEdgeP(*winner_iterator, *second_winner_iterator);
                    if (edge_ptr) edge_ptr->Unsafe = true;
                }
                StartPosition = ElasticStick.Stick.front();
                btnFindTopologicalPathClick(imgViewer);
                btnEBInitializeClick(imgViewer);
            }
            else {
                for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) {
                    eit->Unsafe = false;
                }
            }
        }

        Label15->Caption = (AnsiString) "Tempo per " + txtElasticStickIterations->Text.ToIntDef(0) + " adattamenti (ms): " + (AnsiString) ((double) (after - before));
        Label16->Caption = (AnsiString) "Numero di bolle: " + ElasticStick.Stick.size();
    }

    RefreshViewer();
    DWORD globalafter = GetTickCount();
    Label20->Caption = "Tempo totale di ciclo: " + (AnsiString) (globalafter - globalbefore) + " ms";
    lblTimeElapsed->Caption = "Tempo trascorso: " + (AnsiString) ((double) (GetTickCount() - StartTicks) / 1000) + " s";
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::FormCreate(TObject *Sender)
{
    PPMap.setLimits(0, 0, imgViewer->Width - 1, imgViewer->Height - 1);
    TargetPosition.X = imgViewer->Width / 2;
    TargetPosition.Y = imgViewer->Height / 2;
    txtTimerInterval->Text = (AnsiString) Timer1->Interval;
    txtLFWinner->Text = (AnsiString) Dptm.ParamLearnFactorWinner;
    txtLFSecond->Text = (AnsiString) Dptm.ParamLearnFactorSecondWinner;
    txtMaxEdgeAge->Text = (AnsiString) Dptm.ParamMaxEdgeAge;
    txtMinObstacleDistance->Text = (AnsiString) ElasticStick.ParamMinObstDist;
    txtMinSteeringRadius->Text = (AnsiString) ElasticStick.ParamMinSteeringRadius;
    RandomInputs = 10;
    txtRandomInputs->Text = (AnsiString) RandomInputs;
    RefreshViewer();
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LoadMapBmp(AnsiString MapFileName)
{
    imgMap->Picture->LoadFromFile(MapFileName);
    PPMap.setLimits(0, 0, imgMap->Width - 1, imgMap->Height - 1);
    PPMap.allocObstBmps(1);
    for (int y = 0; y < PPMap.getObstBmpsHeight(); y++) {
        for (int x = 0; x < PPMap.getObstBmpsWidth(); x++) {
            if (imgMap->Canvas->Pixels[x][y] == clBlack) PPMap.StaticObstBmp[x][y] = true;
            else PPMap.StaticObstBmp[x][y] = false;
            PPMap.TemporaryObstBmp[x][y] = false;
        }
    }
    PPMap.UseObstBmps = true;
    PPMap.UseObstSegments = false;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::LoadMapFig(AnsiString MapFileName)
{
    simple_fig_loader figloader;
    figloader.loadFig(MapFileName.c_str());
    int minx = 0, miny = 0, maxx = 0, maxy = 0;
    for (unsigned int i = 0; i < figloader.Segments.size(); i++) {
        if (figloader.Segments[i].X1 < minx) minx = figloader.Segments[i].X1;
        if (figloader.Segments[i].X2 < minx) minx = figloader.Segments[i].X2;
        if (figloader.Segments[i].Y1 < miny) miny = figloader.Segments[i].Y1;
        if (figloader.Segments[i].Y2 < miny) miny = figloader.Segments[i].Y2;
        if (figloader.Segments[i].X1 > maxx) maxx = figloader.Segments[i].X1;
        if (figloader.Segments[i].X2 > maxx) maxx = figloader.Segments[i].X2;
        if (figloader.Segments[i].Y1 > maxy) maxy = figloader.Segments[i].Y1;
        if (figloader.Segments[i].Y2 > maxy) maxy = figloader.Segments[i].Y2;
    }
    double rx = (double) imgMap->Width / (maxx - minx);
    double ry = (double) imgMap->Height / (maxy - miny);
    imgMap->Canvas->Brush->Color = clWhite;
    imgMap->Canvas->FillRect(Rect(0, 0, imgMap->Width, imgMap->Height));
    imgMap->Canvas->Pen->Color = clBlack;
    for (unsigned int i = 0; i < figloader.Segments.size(); i++) {
        imgMap->Canvas->Pen->Width = figloader.Segments[i].Thickness;
        imgMap->Canvas->MoveTo((figloader.Segments[i].X1 - minx) * rx, (figloader.Segments[i].Y1 - miny) * ry);
        imgMap->Canvas->LineTo((figloader.Segments[i].X2 - minx) * rx, (figloader.Segments[i].Y2 - miny) * ry);
    }
    PPMap.StaticObstSegments = figloader.Segments;
    PPMap.setLimits(minx, miny, maxx, maxy);
    PPMap.allocObstBmps(100);
    PPMap.UseObstBmps = true;
    PPMap.UseObstSegments = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::imgViewerMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    X = ((double) X * (PPMap.getMaxX() - PPMap.getMinX()) / imgViewer->Width) + PPMap.getMinX();
    Y = ((double) Y * (PPMap.getMaxY() - PPMap.getMinY()) / imgViewer->Height) + PPMap.getMinY();
    if (rgMouseClick->ItemIndex == 0) {
        if (Button == mbLeft) StartPosition = ppmap::position(X, Y);
        else if (Button == mbRight) TargetPosition = ppmap::position(X, Y);
        btnFindTopologicalPathClick(imgViewer);
        btnEBInitializeClick(imgViewer);
    }
    if (rgMouseClick->ItemIndex == 3) {
        Dptm.putPosition(ppmap::position(X, Y));
        RandomInputs = 1;
    }
    else if (rgMouseClick->ItemIndex == 1) {
        list<elasticstick::node>::iterator it;
        if (Button == mbLeft) {
            it = ElasticStick.Stick.begin();
        }
        else if (Button == mbRight) {
            it = --ElasticStick.Stick.end();
        }
        it->X = X;
        it->Y = Y;
    }
    else if (rgMouseClick->ItemIndex == 2) {
        if (Button == mbLeft) StartPosition = ppmap::position(X, Y);
        else if (Button == mbRight) TargetPosition = ppmap::position(X, Y);
    }
    RefreshViewer();
    if (Timer1->Enabled) RandomInputs = txtRandomInputs->Text.ToIntDef(1);
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnLoadMapClick(TObject *Sender)
{
    bool was_enabled = Timer1->Enabled;
    Timer1->Enabled = false;
    dlgOpen->InitialDir = "..\\maps";
    if (dlgOpen->Execute() && dlgOpen->FileName != "") {
        if (ExtractFileExt(dlgOpen->FileName).LowerCase() == ".fig") LoadMapFig(dlgOpen->FileName);
        else if (ExtractFileExt(dlgOpen->FileName).LowerCase() == ".bmp") LoadMapBmp(dlgOpen->FileName);
    }
    btnFindTopologicalPathClick(btnLoadMap);
    btnResetClick(btnLoadMap);
    RefreshViewer();
    Timer1->Enabled = was_enabled;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnResetClick(TObject *Sender)
{
    Dptm.Edges.clear();
    Dptm.Nodes.clear();
    RefreshViewer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button1Click(TObject *Sender)
{
    RandomInputs = txtRandomInputs->Text.ToIntDef(1);
    Timer1Timer(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::CheckBox1Click(TObject *Sender)
{
    if (CheckBox1->Checked) {
        StartTicks = GetTickCount();
        Timer1->Interval = txtTimerInterval->Text.ToIntDef(100);
        RandomInputs = txtRandomInputs->Text.ToIntDef(1);
    }
    Timer1->Enabled = CheckBox1->Checked;
    Button1->Enabled = !CheckBox1->Checked;
    txtTimerInterval->Enabled = !CheckBox1->Checked;
    txtRandomInputs->Enabled = !CheckBox1->Checked;
    txtElasticStickIterations->Enabled = !CheckBox1->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button2Click(TObject *Sender)
{
    bool was_active = Timer1->Enabled;
    Timer1->Enabled = false;
    Dptm.ParamLearnFactorWinner = txtLFWinner->Text.ToDouble();
    Dptm.ParamLearnFactorSecondWinner = txtLFSecond->Text.ToDouble();
    Dptm.ParamMaxEdgeAge = txtMaxEdgeAge->Text.ToIntDef(0);
    Timer1->Enabled = was_active;
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::chkDisplayNetworkClick(TObject *Sender)
{
    RefreshViewer();    
}
//---------------------------------------------------------------------------





//---------------------------------------------------------------------------
void __fastcall TfrmMain::Button5Click(TObject *Sender)
{
    RefreshViewer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::imgViewerMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    double rx = (double) imgViewer->Width / (PPMap.getMaxX() - PPMap.getMinX());
    double ry = (double) imgViewer->Height / (PPMap.getMaxY() - PPMap.getMinY());
    X = ((double) X * (PPMap.getMaxX() - PPMap.getMinX()) / imgViewer->Width) + PPMap.getMinX();
    Y = ((double) Y * (PPMap.getMaxY() - PPMap.getMinY()) / imgViewer->Height) + PPMap.getMinY();
    if (rgMouseMove->ItemIndex != 0) RefreshViewer();
    if (rgMouseMove->ItemIndex == 1) {
        imgViewer->Canvas->Brush->Color = TColor(0xAAFFAA);
        imgViewer->Canvas->Pen->Color = clWhite;
        ppmap::position min_obst_pt;
        int r = PPMap.nearestObstDistance(ppmap::position(X, Y), &min_obst_pt);
        imgViewer->Canvas->Ellipse((X - r - PPMap.getMinX()) * rx, (Y - r - PPMap.getMinY()) * ry,
                                   (X + r - PPMap.getMinX()) * rx, (Y + r - PPMap.getMinY()) * ry);
        imgViewer->Canvas->MoveTo((X - PPMap.getMinX()) * rx, (Y - PPMap.getMinY()) * ry);
        imgViewer->Canvas->Pen->Color = clBlack;
        imgViewer->Canvas->LineTo((min_obst_pt.X - PPMap.getMinX()) * rx, (min_obst_pt.Y - PPMap.getMinY()) * ry);
    }
    else if (rgMouseMove->ItemIndex == 2) {
        dptm::nodes_iterator it = Dptm.Nodes.begin();
        for ( ; it != Dptm.Nodes.end(); ++it) {
            double x = it->X * rx - X, y = it->Y * ry - Y;
            if (sqrt(x * x + y * y) < 10) {
                imgViewer->Canvas->Pen->Color = clWhite;
                imgViewer->Canvas->Brush->Color = TColor(0xAAFFFF);
                imgViewer->Canvas->Ellipse((it->X - it->Influence - PPMap.getMinX()) * rx, (it->Y - it->Influence - PPMap.getMinY()) * ry,
                           (it->X + it->Influence - PPMap.getMinX()) * rx, (it->Y + it->Influence - PPMap.getMinY()) * ry);
                imgViewer->Canvas->Pen->Color = clBlack;
                imgViewer->Canvas->Brush->Color = clYellow;
                imgViewer->Canvas->Ellipse((it->X - PPMap.getMinX()) * rx - 4, (it->Y - PPMap.getMinY()) * ry - 4,
                    (it->X - PPMap.getMinX()) * rx + 4, (it->Y - PPMap.getMinY()) * ry + 4);
            }
        }
    }
    else if (rgMouseMove->ItemIndex == 3) {
        if (PPMap.UseObstBmps && PPMap.TemporaryObstBmp) {
            ppmap::position p(X, Y);
            for (int y = 0; y < PPMap.getObstBmpsHeight(); y++) {
                for (int x = 0; x < PPMap.getObstBmpsWidth(); x++) {
                    PPMap.TemporaryObstBmp[x][y] = false;
                    if (x == PPMap.posToBmpX(p) && y == PPMap.posToBmpY(p)) PPMap.TemporaryObstBmp[x][y] = true;
                }
            }
            ppmap::position p2 = PPMap.bmpToPos(PPMap.posToBmpX(p), PPMap.posToBmpY(p));
            Caption = (AnsiString) p2.X + " " + p2.Y + " " + ((p2.X - PPMap.getMinX()) * rx);
            imgViewer->Canvas->Brush->Color = clBlack;
            imgViewer->Canvas->FillRect(Rect(
                (p2.X - PPMap.getMinX()) * rx, (p2.Y - PPMap.getMinY()) * ry,
                (p2.X + PPMap.getObstBmpsQuantization() - PPMap.getMinX()) * rx, (p2.Y + PPMap.getObstBmpsQuantization() - PPMap.getMinY()) * ry
            ));
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmMain::btnFindTopologicalPathClick(TObject *Sender)
{
    DWORD before, after;
    before = GetTickCount();
    TopologicalPath = Dptm.findTopologicalPath(StartPosition, TargetPosition, CheckBox2->Checked);
    after = GetTickCount();

    AnsiString a;
    a.sprintf("%.2f", PathCost);
    Label3->Caption = "Tempo per path topologico (ms): " + (AnsiString) ((double) after - before);
    Label13->Caption = "Lunghezza path topologico: " + a;

    RefreshViewer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::btnEBInitializeClick(TObject *Sender)
{
    ElasticStick.initialize(TopologicalPath);
    RefreshViewer();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button7Click(TObject *Sender)
{
    bool was_active = Timer1->Enabled;
    Timer1->Enabled = false;
    ElasticStick.ParamMinObstDist = txtMinObstacleDistance->Text.ToIntDef(0);
    ElasticStick.ParamMinSteeringRadius = txtMinSteeringRadius->Text.ToIntDef(0);
    Timer1->Enabled = was_active;
}
//---------------------------------------------------------------------------


void __fastcall TfrmMain::Button3Click(TObject *Sender)
{
	double MAX_DIST = PPMap.getMaxX() + PPMap.getMaxY() - PPMap.getMinX() - PPMap.getMinY();
    if (ElasticStick.CriticalBubblePtr == 0) return;
	double min_distance = MAX_DIST;
	dptm::edge* edge_ptr = 0;
    ppmap::position p;
    TCanvas* c = imgViewer->Canvas;
	for (dptm::edges_iterator eit = Dptm.Edges.begin(); eit != Dptm.Edges.end(); ++eit) {
		if (!eit->Unsafe && (PPMap.posSight(*ElasticStick.CriticalBubblePtr, *eit->First) || PPMap.posSight(*ElasticStick.CriticalBubblePtr, *eit->Second))) {
            c->Ellipse(ElasticStick.CriticalBubblePtr->X - 3, ElasticStick.CriticalBubblePtr->Y - 3, ElasticStick.CriticalBubblePtr->X + 3, ElasticStick.CriticalBubblePtr->Y + 3);
            c->Pen->Color = clBlack;
            c->Pen->Width = 2;
            c->MoveTo(eit->First->X, eit->First->Y);
            c->LineTo(eit->Second->X, eit->Second->Y);
			double distance = PPMap.posDistFromSegment(*ElasticStick.CriticalBubblePtr, *eit->First, *eit->Second, &p);
            c->Pen->Color = clGreen;
            c->Pen->Width = 1;
            c->MoveTo(ElasticStick.CriticalBubblePtr->X, ElasticStick.CriticalBubblePtr->Y);
            c->LineTo(p.X, p.Y);
			if (distance < min_distance) {
				edge_ptr = &*eit;
				min_distance = distance;
			}
		}
	}
	if (edge_ptr) {
        edge_ptr->Unsafe = true;
        c->Pen->Color = clRed;
        c->MoveTo(edge_ptr->First->X, edge_ptr->First->Y);
        c->LineTo(edge_ptr->Second->X, edge_ptr->Second->Y);
    }
    c->Pen->Width = 1;
    c->Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button4Click(TObject *Sender)
{
    for (dptm::edges_iterator it = Dptm.Edges.begin(); it != Dptm.Edges.end(); ++it) {
        it->Unsafe = false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button6Click(TObject *Sender)
{
    Dptm.removeCrosses();
}
//---------------------------------------------------------------------------

void __fastcall TfrmMain::Button8Click(TObject *Sender)
{
    Dptm.removeRedundantEdges();    
}
//---------------------------------------------------------------------------

