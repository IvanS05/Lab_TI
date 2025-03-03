#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QLineEdit>
#include <QLabel>
#include <QMap>
#include <QChar>
#include <algorithm>

class CryptoApp : public QWidget {
    Q_OBJECT
public:
    CryptoApp(QWidget *parent = nullptr) : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout(this);

        inputText = new QTextEdit(this);
        outputText = new QTextEdit(this);
        keyInput = new QLineEdit(this);
        QPushButton *openFileBtn = new QPushButton("Открыть файл", this);
        QPushButton *saveFileBtn = new QPushButton("Сохранить файл", this);
        QPushButton *encryptPlayfairBtn = new QPushButton("Зашифровать (Плейфейр)", this);
        QPushButton *decryptPlayfairBtn = new QPushButton("Расшифровать (Плейфейр)", this);
        QPushButton *encryptVigenereBtn = new QPushButton("Зашифровать (Виженер)", this);
        QPushButton *decryptVigenereBtn = new QPushButton("Расшифровать (Виженер)", this);
        QPushButton *exitBtn = new QPushButton("Выход", this);

        layout->addWidget(openFileBtn);
        layout->addWidget(new QLabel("Исходный текст:"));
        layout->addWidget(inputText);
        layout->addWidget(new QLabel("Ключ:"));
        layout->addWidget(keyInput);
        layout->addWidget(encryptPlayfairBtn);
        layout->addWidget(decryptPlayfairBtn);
        layout->addWidget(encryptVigenereBtn);
        layout->addWidget(decryptVigenereBtn);
        layout->addWidget(new QLabel("Результат:"));
        layout->addWidget(outputText);
        layout->addWidget(saveFileBtn);
        layout->addWidget(exitBtn);

        connect(openFileBtn, &QPushButton::clicked, this, &CryptoApp::openFile);
        connect(saveFileBtn, &QPushButton::clicked, this, &CryptoApp::saveFile);
        connect(encryptPlayfairBtn, &QPushButton::clicked, this, &CryptoApp::encryptPlayfair);
        connect(decryptPlayfairBtn, &QPushButton::clicked, this, &CryptoApp::decryptPlayfair);
        connect(encryptVigenereBtn, &QPushButton::clicked, this, &CryptoApp::encryptVigenere);
        connect(decryptVigenereBtn, &QPushButton::clicked, this, &CryptoApp::decryptVigenere);
        connect(exitBtn, &QPushButton::clicked, this, &QApplication::quit);
    }

private slots:
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Открыть файл", "", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                inputText->setText(in.readAll());
                file.close();
            }
        }
    }

    void saveFile() {
        QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "Text Files (*.txt)");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << outputText->toPlainText();
                file.close();
            }
        }
    }

    void encryptPlayfair() {
        QString text = inputText->toPlainText().toUpper();
        QString key = keyInput->text().toUpper();
        outputText->setText(playfairCipher(text, key, true));
    }

    void decryptPlayfair() {
        QString text = inputText->toPlainText().toUpper();
        QString key = keyInput->text().toUpper();
        outputText->setText(playfairCipher(text, key, false));
    }

    void encryptVigenere() {
        QString text = inputText->toPlainText().toUpper();
        QString key = keyInput->text().toUpper();
        outputText->setText(vigenereCipher(text, key, true));
    }

    void decryptVigenere() {
        QString text = inputText->toPlainText().toUpper();
        QString key = keyInput->text().toUpper();
        outputText->setText(vigenereCipher(text, key, false));
    }

private:
    QTextEdit *inputText;
    QTextEdit *outputText;
    QLineEdit *keyInput;

    QString vigenereCipher(const QString &text, QString key, bool encrypt) {
        QString result;
        int keyIndex = 0;

        // Определяем русский алфавит с буквой Ё
        QString alphabet = "АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
        int alphabetSize = alphabet.length(); // 33 буквы

        // Фильтрация ключа: оставляем только русские буквы
        QString filteredKey;
        for (QChar ch : key.toUpper()) {
            if (alphabet.contains(ch)) {
                filteredKey.append(ch);
            }
        }

        // Проверяем, есть ли русские буквы в ключе
        if (filteredKey.isEmpty()) {
            return "Ошибка: В ключе нет русских букв!";
        }

        key = filteredKey;

        // Проверка, содержит ли исходный текст только английские буквы
        QString englishAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        bool containsOnlyEnglish = true;
        for (QChar ch : text) {
            if (!englishAlphabet.contains(ch.toUpper())) {
                containsOnlyEnglish = false;
                break;
            }
        }

        if (containsOnlyEnglish) {
            return "Ошибка: В тексте нет русских букв!";
        }

        // Проход по тексту
        for (QChar ch : text) {
            int charIndex = alphabet.indexOf(ch); // Индекс буквы в алфавите

            if (charIndex != -1) { // Если буква русская
                int keyCharIndex = alphabet.indexOf(key[keyIndex % key.length()]); // Индекс буквы ключа
                int shift = keyCharIndex; // Сдвиг зависит от буквы ключа

                if (!encrypt) shift = -shift; // Для расшифровки меняем знак сдвига

                // Вычисляем новый индекс
                int newIndex = (charIndex + shift + alphabetSize) % alphabetSize;
                result.append(alphabet[newIndex]);

                keyIndex++; // Двигаемся по ключу

                // Прогрессивное изменение ключа после каждого полного прохода
                if (keyIndex % key.length() == 0) {
                    for (int j = 0; j < key.length(); j++) {
                        int kIndex = alphabet.indexOf(key[j]);
                        kIndex = (kIndex + 1) % alphabetSize; // Двигаем букву на 1 позицию вперёд
                        key[j] = alphabet[kIndex]; // Обновляем ключ
                    }
                }
            } else {
                // Оставляем символ без изменений
                result.append(ch);
            }
        }

        return result;
    }



    QString playfairCipher(const QString &text, const QString &key, bool encrypt) {
        QString alphabet = "ABCDEFGHIKLMNOPQRSTUVWXYZ";
        QString filteredKey;
        QSet<QChar> used;
        bool hasEnglishLetters = false;

        // Фильтрация ключа: оставляем только английские буквы (без J, заменяя на I)
        for (QChar ch : key.toUpper()) {
            if (ch == 'J') ch = 'I';
            if (alphabet.contains(ch) && !used.contains(ch)) {
                filteredKey.append(ch);
                used.insert(ch);
                hasEnglishLetters = true;
            }
        }

        // Проверка наличия английских букв в ключе
        if (!hasEnglishLetters) {
            return "Error: No English letters in the key!";
        }

        // Дополняем алфавитом, если есть пробелы
        for (QChar ch : alphabet) {
            if (!used.contains(ch)) {
                filteredKey.append(ch);
            }
        }

        QMap<QChar, QPoint> pos;
        QChar matrix[5][5];

        for (int i = 0, k = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j, ++k) {
                matrix[i][j] = filteredKey[k];
                pos[filteredKey[k]] = QPoint(j, i);
            }
        }

        QString result;
        QString processedText;
        hasEnglishLetters = false;

        // Фильтрация текста: оставляем только английские буквы (заменяем 'J' на 'I')
        for (QChar ch : text.toUpper()) {
            if (ch == 'J') ch = 'I';
            if (alphabet.contains(ch)) {
                if (processedText.length() % 2 == 1 && processedText.back() == ch) {
                    processedText.append('X');
                }
                processedText.append(ch);
                hasEnglishLetters = true;
            }
        }

        // Проверка наличия английских букв в тексте
        if (!hasEnglishLetters) {
            return "Error: No English letters in the input text!";
        }

        if (processedText.length() % 2 == 1) {
            processedText.append('X');
        }

        // Шифрование/дешифрование биграмм
        for (int i = 0; i < processedText.length(); i += 2) {
            QChar a = processedText[i], b = processedText[i + 1];
            QPoint p1 = pos[a], p2 = pos[b];

            if (p1.y() == p2.y()) {
                // Одна строка: смещаем вправо при шифровании, влево при расшифровке
                int shift = encrypt ? 1 : -1;
                p1.setX((p1.x() + shift + 5) % 5);
                p2.setX((p2.x() + shift + 5) % 5);
            } else if (p1.x() == p2.x()) {
                // Один столбец: смещаем вниз при шифровании, вверх при расшифровке
                int shift = encrypt ? 1 : -1;
                p1.setY((p1.y() + shift + 5) % 5);
                p2.setY((p2.y() + shift + 5) % 5);
            } else {
                // Прямоугольник: меняем столбцы
                std::swap(p1.rx(), p2.rx());
            }

            result.append(matrix[p1.y()][p1.x()]).append(matrix[p2.y()][p2.x()]);
        }

        // Удаляем лишние X при расшифровке
        if (!encrypt) {
            QString cleanedResult;
            for (int i = 0; i < result.length(); ++i) {
                if (i > 0 && result[i] == 'X' && (i == result.length() - 1 || result[i - 1] == result[i + 1])) {
                    continue;
                }
                cleanedResult.append(result[i]);
            }
            return cleanedResult;
        }

        return result;
    }

};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    CryptoApp window;
    window.setWindowTitle("Шифрование и дешифрование");
    window.resize(600, 500);
    window.show();
    return app.exec();
}

#include "main.moc"
