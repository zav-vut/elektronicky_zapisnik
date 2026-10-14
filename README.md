# Udělat nějáký rámec a potom to rozšiřovat

## Základní parametry:
1. Zobrazení úkolů pro aktuální den, navigace mezi úkoly.
2. Úkoly označitelné jako hotové, možnost nastavení priority.
3. Možnost kopírování či přesouvání úkolů mezi daty.
Bonusový úkol: Implementujte vykreslení jednotlivých událostí v kalendáři (denní/týdenní
režim).

- CRUDM(Move)
- Menu 
- Persistence

- Vykresleni dynamickeho menu,
- Ukladani do souboru a mechanicka prace jako backend.

# Features
## Šimon
- Vytvoreni poznamky kde bude datum kdy to splnit, Hlavicka a samotny text, je hotovy?
- Vypsani poznamky
- Upraveni poznamky
- Smazani poznamky
- Zapisovat poznamky do csv souboru

## Lukáš
- Dynamicke menu 1
> V loopu vypsat vsechny nasledujci ukoly
> Vytvoreni poznamky
> Zmena Datumu
> Vypsani do Kalendare
- Po kliknuti na poznamku se objevi dalsi menu
> Oznacit jako splneno
> Zobrazit poznamku
> Editovat poznamku
> Smazat poznamku



---

- Access throught password, encryption of persistent file Luky
- Search engine, by tag, date regex name. Šimon
- Dynamic Menu throught Arrows Šimon
- Color the menu Šimon

---

## 🛠️ Data Structure Overview

Defined in `notebook.h`:

```c
typedef struct {
    int id;
    char title[100];
    char category[30];
    char date[20];
    char *content;
} Note;

typedef struct {
    Note *notes;
    int count;
    int capacity;
} Notebook;

// Dynamic Menu Item Structure
typedef struct {
    char label[50];
    void (*action)(Notebook *);
} MenuItem;

typedef struct {
    MenuItem *items;
    int count;
    int selected_index;
} DynamicMenu;
