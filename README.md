# Udělat nějáký rámec a potom to rozšiřovat
- CRUD Luky
- Menu Šimon
- Persistence Luky

---

- Access throught password, encryption of persistent file Luky
- Search engine, by tag, date regex name. Šimon
- Dynamic Menu throught Arrows Šimon
- Color the menu Šimon




# AI shit

| Component | Partner | Mandatory Responsibilities | Key Files |
| :--- | :--- | :--- | :--- |
| **User Interface** | **Šimon** | • Dynamic vector menu navigation<br>• Interactive ANSI arrow-key selection<br>• Note display, pagination & formatting<br>• User input validation & search UI | `main.c`, `ui.c`, `ui.h` |
| **Data & Storage** | **Lukáš** | • Memory allocation & CRUD logic<br>• File persistence & encryption logic<br>• Password key derivation (PBKDF/hashing)<br>• Category/date filtering & search algorithms | `storage.c`, `storage.h`, `crypto.c` |

---

## 📋 Mandatory Features

### 1. CRUD Operations
* **Create:** Add notes with title, content, category tags, and creation timestamp.
* **Read:** View full note content or display paginated lists.
* **Update:** Modify existing note titles, body text, or categories.
* **Remove:** Delete single notes or clear category selections (with confirmation prompts).

### 2. Security & Storage
* **File Encryption:** Store saved notes on disk using encrypted binary/structured formatting.
* **Password Access & Key Derivation:** Secure access with password protection using hashed/derived keys rather than raw string comparisons.
* **Memory Wiping:** Clear decrypted text buffers from RAM upon session lock or exit.

### 3. Filtering & Search Engine
* **Category & Date Filtering:** Filter records by category tags and specific date ranges simultaneously.
* **Keyword Search:** Case-insensitive substring matching (`strstr`) across titles and content text.

### 4. Interactive Console UI
* **Dynamic Vector Menu:** Fully modular menu state system built on dynamic structures.
* **Arrow-Key Navigation:** Terminal navigation powered by ANSI escape sequences (`Up`/`Down` arrows to highlight, `Enter` to select).

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