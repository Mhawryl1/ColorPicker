
/*To add resource file to project compile .rc file with rc.exe  ->"rc.exe is in dev command prompt". Next link created
*.res file to project by adding  /link <path_to_file> in task.json in args list 
Remakes: resource.h file must have new line on the end*/
#include<windows.h>
#include "../include/resource.h"
IDI_PICKER ICON "colour_picker.ico"
IDI_CLOSE ICON "close.ico"
IDI_CANCEL ICON "cancel.ico"
IDI_PASTE ICON "paste.ico"
IDI_PASTEWHITE ICON "pasteWhite.ico"
IDI_CLOSEBMPR BITMAP "closeSubMenu.bmp"
IDI_SETTING BITMAP "settingSubMenu.bmp"
IDR_POPUPMENU MENU
    BEGIN 
    POPUP "SUBMENU" 
      BEGIN     
        MENUITEM "&Setting",             IDM_SETTING
        MENUITEM SEPARATOR
        MENUITEM "&Close",               IDM_CLOSE
      END
END
