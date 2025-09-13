# Microtest - Настройка разработки веб-игры

Игра на языке C, скомпилированная с помощью Emscripten и raylib.

## 🎮 Играть онлайн
Посетите [d00m4ace.com](https://d00m4ace.com/) для других игр

## 📋 Необходимые компоненты для Windows

Для компиляции игры в веб-версию необходимо установить следующие инструменты:

### 1. Emscripten SDK
Emscripten - это набор инструментов для компиляции кода C/C++ в WebAssembly.

**Шаги установки:**
1. Скачайте и установите [Git для Windows](https://git-scm.com/download/win)
2. Клонируйте Emscripten SDK:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ```
3. Установите и активируйте последнюю версию Emscripten:
   ```bash
   emsdk install latest
   emsdk activate latest
   ```
4. Настройте переменные окружения (выполняйте каждый раз в новом терминале или добавьте в PATH):
   ```bash
   emsdk_env.bat
   ```

### 2. MinGW-w64 (Обязательно)
Предоставляет компилятор GCC и Unix-подобные инструменты для Windows.

**Шаги установки:**
1. Скачайте MinGW-w64 из одного из источников:
   - [MinGW-w64 официальный сайт](https://www.mingw-w64.org/downloads/)
   - [WinLibs](https://winlibs.com/) (standalone сборка, рекомендуется)
   - [TDM-GCC](https://jmeubank.github.io/tdm-gcc/)
   
2. Распакуйте в папку (например, `C:\mingw64`)

3. Добавьте в системную переменную PATH:
   - Правый клик на "Этот компьютер" → Свойства → Дополнительные параметры системы
   - Нажмите "Переменные среды"
   - В разделе "Системные переменные" найдите "Path" и нажмите "Изменить"
   - Добавьте путь к папке bin MinGW-w64 (например, `C:\mingw64\bin`)
   
4. Проверьте установку:
   ```bash
   gcc --version
   mingw32-make --version
   ```

### 3. raylib для Web (raylibweb)
Библиотека raylib, скомпилированная для WebAssembly/Emscripten.

**Вариант 1: Скачать готовую библиотеку**
1. Скачайте raylib для Web с:
   - [raylib GitHub Releases](https://github.com/raysan5/raylib/releases) (ищите файл "raylib-x.x.x_webassembly.zip")
   - Распакуйте архив для получения `libraylib.a`

**Вариант 2: Скомпилировать raylib самостоятельно**
1. Клонируйте репозиторий raylib:
   ```bash
   git clone https://github.com/raysan5/raylib.git
   cd raylib/src
   ```

2. Убедитесь, что Emscripten активирован:
   ```bash
   путь\к\emsdk\emsdk_env.bat
   ```

3. Скомпилируйте raylib для Web:
   ```bash
   mingw32-make PLATFORM=PLATFORM_WEB -B
   ```
   
4. Скомпилированный файл `libraylib.a` будет находиться в папке `raylib/src`

5. Скопируйте необходимые файлы в ваш проект:
   ```
   microtest/
   └── raylibweb/
       ├── libraylib.a      # Скомпилированная библиотека из raylib/src/
       ├── raylib.h         # Из raylib/src/
       ├── raymath.h        # Из raylib/src/
       ├── rlgl.h           # Из raylib/src/
       └── external/        # Скопируйте папку raylib/src/external/
           ├── glfw/
           └── ...
   ```

**Примечание:** Используйте одну и ту же версию Emscripten для компиляции raylib и вашей игры во избежание проблем совместимости.

### 4. Утилита Make
MinGW-w64 включает `mingw32-make`. Для удобства можно создать алиас:
- Создайте копию `mingw32-make.exe` и переименуйте её в `make.exe` в той же папке
- Или используйте `mingw32-make` напрямую в командах

## 🔨 Инструкции по сборке

### Структура проекта
```
microtest/
├── main.c              # Основной исходный код игры
├── minshell.html       # Пользовательский HTML шаблон
├── resources/          # Игровые ресурсы (текстуры, звуки и т.д.)
└── raylibweb/          # Библиотека raylib для веб (см. установку выше)
    ├── libraylib.a     # Скомпилированная библиотека raylib для Web
    ├── raylib.h        # Основной заголовочный файл raylib
    ├── raymath.h       # Математические функции raylib
    ├── rlgl.h          # Слой абстракции OpenGL raylib
    └── external/       # Внешние зависимости
        └── glfw/       # Заголовки GLFW
```

### Компиляция

1. Откройте командную строку или PowerShell
2. Перейдите в папку `microtest`:
   ```bash
   cd путь\к\microtest
   ```
3. Активируйте окружение Emscripten:
   ```bash
   путь\к\emsdk\emsdk_env.bat
   ```
4. Выполните команду компиляции:
   ```bash
   emcc main.c -o main.html ../raylibweb/libraylib.a -I. -I../raylibweb -I../raylibweb/external -L. -L../raylibweb -Wall -std=c11 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -O3 -sINITIAL_MEMORY=64MB -sTOTAL_MEMORY=256MB -sALLOW_MEMORY_GROWTH=1 -sASSERTIONS=1 -sFORCE_FILESYSTEM=1 -sEXPORTED_FUNCTIONS=['_main','_malloc','_free','_LoadFileCallback','_OnClipboardPaste'] -sEXPORTED_RUNTIME_METHODS=['HEAPF32','ccall','cwrap','UTF8ToString','HEAPU8'] -sMINIFY_HTML=0 -sUSE_GLFW=3 -sASYNCIFY -sASYNCIFY_STACK_SIZE=64MB -lidbfs.js --shell-file minshell.html --preload-file resources -DPLATFORM_WEB
   ```

### Выходные файлы
После успешной компиляции вы получите:
- `main.html` - Основной HTML файл
- `main.js` - JavaScript код-связка
- `main.wasm` - Бинарный файл WebAssembly
- `main.data` - Предзагруженные ресурсы

## 🚀 Запуск игры

### Локальное тестирование
Из-за политик безопасности браузера необходим локальный веб-сервер:

**Вариант 1: Python (если установлен)**
```bash
python -m http.server 8000
```
Затем откройте `http://localhost:8000/main.html`

**Вариант 2: Node.js http-server**
```bash
npm install -g http-server
http-server -p 8000
```

**Вариант 3: Visual Studio Code**
Установите расширение "Live Server" и кликните правой кнопкой на `main.html` → "Open with Live Server"

## 📝 Объяснение флагов компиляции

- `-sINITIAL_MEMORY=64MB` - Начальное выделение памяти
- `-sTOTAL_MEMORY=256MB` - Максимальный лимит памяти
- `-sALLOW_MEMORY_GROWTH=1` - Разрешить динамический рост памяти
- `-sFORCE_FILESYSTEM=1` - Включить поддержку файловой системы
- `-sASYNCIFY` - Включить асинхронные операции
- `--preload-file resources` - Упаковать папку ресурсов
- `--shell-file minshell.html` - Использовать пользовательский HTML шаблон

## 🐛 Решение проблем

### Частые проблемы:

1. **"emcc не распознан как команда"**
   - Убедитесь, что вы выполнили `emsdk_env.bat`
   - Проверьте правильность установки Emscripten

2. **"Не найден libraylib.a"**
   - Убедитесь, что raylib скомпилирован для Web с помощью Emscripten (не desktop версия!)
   - Проверьте, что структура папки raylibweb соответствует описанной выше
   - Проверьте наличие всех заголовочных файлов

3. **Ресурсы не загружаются**
   - Проверьте существование папки resources и наличие в ней файлов
   - Проверьте консоль браузера на наличие ошибок

4. **Ошибки памяти**
   - Увеличьте лимиты памяти в флагах компиляции
   - Проверьте код на утечки памяти

## 📚 Дополнительные ресурсы

- [Документация Emscripten](https://emscripten.org/docs/)
- [Официальный сайт raylib](https://www.raylib.com/)
- [WebAssembly MDN документация](https://developer.mozilla.org/ru/docs/WebAssembly)

## 📄 Лицензия

Этот проект может быть использован только для некоммерческих целей. Коммерческое использование запрещено без письменного разрешения автора.

## 👤 Автор

Создано d00m4ace - Посетите [d00m4ace.com](https://d00m4ace.com/) для других игр!