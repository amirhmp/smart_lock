
#ifndef CODE_MENU_BUILDER_H
#define CODE_MENU_BUILDER_H

extern unsigned short menuIndex;
struct MenuItem {
    flash unsigned char *title;
    void (*onClick)();
    struct MenuItem *subMenuItems;  //array of menuItems
    unsigned short subMenuItemsSize;
};

extern struct MenuItem *currentMenu;
void init_menu_builder(struct MenuItem mainMenu[], int childrenSize);
void print_main_menu();
void enterMenu();
void backMenu();
void print_menu(const struct MenuItem menu[], unsigned short);
void print_current_menu();
void menu_item_on_click(); /* dar soorati ke dar menu haye tu dar tu hastim vaghti ru btn click mishe in method ro seda mikonim ta menu dakheli tar load she */
void menu_up();
void menu_down();

#endif //CODE_MENUS_H
