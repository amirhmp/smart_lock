#include "alcd.h"
#include "stdio.h"
#include "menu_builder.h"
#include "static_stack.h"

char _lcd_text[16];
const unsigned short PAGE_SIZE = 2; //max lcd rows


struct MenuItem first_menu;


void init_menu_builder(struct MenuItem mainMenu[], int childrenSize) {
    first_menu.onClick = &enterMenu;
    first_menu.subMenuItems = mainMenu;
    first_menu.subMenuItemsSize = childrenSize;
    first_menu.title = "";
}



void print_menu(const struct MenuItem menu[], unsigned short startIndex) { //dar har startIndex 2 menu ra namyesh midahim, pas dar har bar namyesh 2 khane az array ra mikhanim
    unsigned short i;
    lcd_clear();
    for (i = startIndex; i < startIndex + PAGE_SIZE; ++i) {
        sprintf(_lcd_text, menu[i].title);
        lcd_gotoxy(1, i - startIndex); //x = 1 chun mikhahim ye char az aval vase > ja bezarim
        lcd_puts(_lcd_text);
    }
}


//LCD and Menu needed variables
struct MenuItem *currentMenu;
unsigned short lcd_arrow_y;
unsigned short menuIndex;
unsigned short startIndex;
#define endIndex ((startIndex + PAGE_SIZE - 1) >= currentMenu->subMenuItemsSize ? currentMenu->subMenuItemsSize - 1 : (startIndex + PAGE_SIZE - 1))

void reset_indices() {
    startIndex = 0;
    lcd_arrow_y = 0;
    menuIndex = 0;
}

void select_item(unsigned short newIndex, unsigned short prevIndex) {
    if (prevIndex != newIndex) {
        lcd_gotoxy(0, prevIndex);
        lcd_putchar(' ');
    }
    lcd_gotoxy(0, newIndex);
    lcd_putchar('>');
}

void print_main_menu() {
    currentMenu = (struct MenuItem *) &first_menu;
    print_menu(first_menu.subMenuItems, 0);
    select_item(0, 0);
}

void enterMenu() {
    //save backward path
    stack_push(currentMenu);
    currentMenu = (struct MenuItem *) &currentMenu->subMenuItems[menuIndex];
    //printing new menu
    print_menu(currentMenu->subMenuItems, 0);
    select_item(0, 0);
    //reset lcd variables
    reset_indices();
}

void backMenu() {
    if (!stack_is_empty()) {
        currentMenu = stack_pop();
        print_menu(currentMenu->subMenuItems, 0);
        select_item(0, 0);
    }
    //reset lcd variables
    reset_indices();
}

void menu_down() {
    if (menuIndex < currentMenu->subMenuItemsSize - 1) {
        unsigned short prev_arrow_y = lcd_arrow_y;
        menuIndex++;
        if (menuIndex > endIndex) { //bayad page jadid ro neshun bedim
            startIndex++;
            print_menu(currentMenu->subMenuItems, startIndex);
        }
        lcd_arrow_y = menuIndex - startIndex;
        select_item(lcd_arrow_y, prev_arrow_y);
    }
}

void menu_up() {
    if (menuIndex > 0) {
        unsigned short prev_arrow_y = lcd_arrow_y;
        menuIndex--;
        if (menuIndex < startIndex) { //bayad page jadid ro neshun bedim
            startIndex--;
            print_menu(currentMenu->subMenuItems, startIndex);
        }

        lcd_arrow_y = menuIndex - startIndex;
        select_item(lcd_arrow_y, prev_arrow_y);
    }
}


void print_current_menu() {
    print_menu(currentMenu->subMenuItems, 0);
    select_item(0, 0);
    reset_indices();
}

void menu_item_on_click() {
    currentMenu->subMenuItems[menuIndex].onClick();
}