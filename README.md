# Udělat nějáký rámec a potom to rozšiřovat

## Základní parametry:
1. Zobrazení úkolů pro aktuální den, navigace mezi úkoly.
2. Úkoly označitelné jako hotové, možnost nastavení priority.
3. Možnost kopírování či přesouvání úkolů mezi daty.
Bonusový úkol: Implementujte vykreslení jednotlivých událostí v kalendáři (denní/týdenní
režim)

# Features
## Šimon
- Vytvoreni poznamky kde bude datum kdy to splnit, Hlavicka a samotny text, je hotovy?
- Vypsani poznamky
- Upraveni poznamky
- Smazani poznamky
- Zapisovat poznamky do csv souboru

## Lukáš
- Dynamicke menu
  - V loopu vypsat vsechny nasledujci ukoly
  - Vytvoreni poznamky
   - Zmena Datumu
   - Vypsani do Kalendare
- Po kliknuti na poznamku se objevi dalsi menu
  - Oznacit jako splneno
  - Zobrazit poznamku
  - Editovat poznamku
  - Smazat poznamku
- Zoprazení poznámek v kalendáři
- Načítáni dat z csv


---

- Access throught password, encryption of persistent file Luky
- Search engine, by tag, date regex name. Šimon
- Dynamic Menu throught Arrows Šimon
- Color the menu Šimon

---

## 🛠️ Data Structure Overview
```c
typedef struct {
    int id;
    char title[MAX_TITLE];
    char text[MAX_TEXT];
    int completed; // 0 - uncompleted, 1 = completed
    char complate_date[11]; // YYYY-MM-DD
} Note;

```
