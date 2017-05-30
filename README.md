# PixelFontEditor

Основное назначение - генерация h-файла с таблицей шрифта для использования в программировании микроконтроллеров, работающих с LCD-дисплеями.

Основное меню:
- New - Очистка области рисования
- Open - Загрузка шрифта из специального бинарного файла, либо из растрового изображения подходящего размера
- Build - Генерация шрифта в область рисования из установленных в системе шрифтов
- Save - Сохранение шрифта в бинарный файл, либо в растровое изображение
- Export - Генерация заголовочного файла с таблицей шрифта

Редактирование пикселей возможно только внутри текущего символа, обозначенного красной рамкой.
Смена текущего символа - CTRL+LEFT_MOUSE_CLICK или RIGHT_MOUSE_CLICK по нужному символу внутри области рисования или в таблице справа.

Сочетания клавиш:
- CTRL+C - Копирование текущего символа в буфер
- CTRL+V - Замена текущего символа содержимым буфера
- CTRL+W - Смещение текущего символа вверх на один пиксель
- CTRL+S - Смещение текущего символа вниз на один пиксель
- CTRL+A - Смещение текущего символа влево на один пиксель
- CTRL+D - Смещение текущего символа вправо на один пиксель
- CTRL+I - Инвертирование цветов всего шрифта

default.pfnt - Файл с уже сгенерированным шрифтом, который загружается по умолчанию при запуске программы. Не обязателен, можно сгенерировать в процессе работы.

settings.cfg - Файл настроек. Если отсутствует, то будут использоваться значения по-умолчанию. Исходные коды для работы с настройками взяты из репозитория [SimpleSettings](https://github.com/olesya-wo/SimpleSettings)
