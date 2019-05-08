// NeL - MMORPG Framework <http://dev.ryzom.com/projects/nel/>
// Copyright (C) 2010  Winch Gate Property Limited
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "nel/misc/log.h"
#include "nel/misc/string_common.h"
#include "nel/misc/command.h"
#include "nel/misc/hierarchical_timer.h"
#include "nel/misc/debug.h"
#include "nel/misc/window_displayer.h"
#include "nel/misc/gtk_displayer.h"
#include "nel/misc/win_displayer.h"
#include "nel/misc/stdin_monitor_thread.h"

using namespace NLMISC;
using namespace std;

CWindowDisplayer*   WindowDisplayer = NULL;

void  testcalltime()
{
    H_AUTO(testcalltime);

    nlSleep(1);
}

static bool isExit = false;

#if defined(NL_OS_WINDOWS) && defined(_WINDOWS)
int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, const char **argv)
#endif
{
    NLMISC::CApplicationContext	serviceContext;
    CHTimer::startBench(false, true);
    CHTimer::endBench();

    CWindowDisplayer*   W111 = NULL;

    // create a logger; it's an information logger.
    CLog CommandLog (CLog::LOG_INFO);

#ifdef NL_OS_WINDOWS
    WindowDisplayer = new CWinDisplayer ("DEFAULT_WD");
#endif // NL_OS_WINDOWS

    if ( WindowDisplayer!=NULL )
    {
        WindowDisplayer->create ( "*INIT* Test App.", false, -1, -1, -1, -1, -1, 0, "", false, &CommandLog);
        WindowDisplayer->setTitleBar ("App version 1.0.0");

        //   DebugLog->addDisplayer (WindowDisplayer);
        //  InfoLog->addDisplayer (WindowDisplayer);
        //   WarningLog->addDisplayer (WindowDisplayer);
        //   ErrorLog->addDisplayer (WindowDisplayer);
        //   AssertLog->addDisplayer (WindowDisplayer);
        CommandLog.addDisplayer(WindowDisplayer, true);
    }
    else
    {
        IStdinMonitorSingleton::getInstance()->init();
    }

    while ( !isExit )
    {
        CHTimer::startBench(false, true, false);

        if ( WindowDisplayer!=NULL )
        {
            WindowDisplayer->update ();
        }
        
        // deal with any input waiting from stdin
        {
            H_AUTO(NLNETStdinMonitorUpdate);
            IStdinMonitorSingleton::getInstance()->update();
        }

        {
            H_AUTO(TTTTestTimer);
            nlSleep(7);

            testcalltime();
            testcalltime();
        }
        
        testcalltime();
        nlSleep(100);
        CHTimer::endBench();
    }

    // remove the stdin monitor thread
    IStdinMonitorSingleton::getInstance()->release(); // does nothing if not initialized
    return EXIT_SUCCESS;
}



NLMISC_CATEGORISED_COMMAND(nel, quit, "exit the service", "")
{
    if(args.size() != 0) return false;

    isExit = true;
    log.displayNL("User ask me with a command to quit");

    return true;
}

