//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("pplan.res");
USEFORM("uMain.cpp", frmMain);
USEUNIT("simple_fig_loader.cpp");
USEUNIT("ppmap.cpp");
USEUNIT("utilz.cpp");
USEUNIT("dptm.cpp");
USEUNIT("elasticstick.cpp");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TfrmMain), &frmMain);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    return 0;
}
//---------------------------------------------------------------------------
