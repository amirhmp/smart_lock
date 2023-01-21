
#ifndef CODE_MENUS_H
#define CODE_MENUS_H

#include "menu_builder.h"
#include "../../smart_lock.h"


struct MenuItem phoneMenu[] = {
        {
                "<Empty>",
                &backMenu
        },
        {
                "<Empty>",
                &backMenu
        },
        {
                "<Empty>",
                &backMenu
        },
        {
                "<Empty>", /** WARNING:: age bishtar az 15 char beshe hang mikone, chun az x=1 shoru mikonim be chap kardan va lcd ham 16 char mibashad*/
                &backMenu
        },
        {
                "Back",
                &backMenu
        }
};

struct MenuItem mainMenu[] = {
        {
                "Set Phone",
                &onSetPhoneClicked
        },
        {
                "Change Password",
                &enterMenu,
                phoneMenu,
                ARRAY_SIZE(phoneMenu)
        },
        {
                "Exit",
                &onExitMenuClicked
        }
};

#endif //CODE_MENUS_H
