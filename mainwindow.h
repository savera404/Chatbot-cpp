#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendMessage();
    void on_SendButton_clicked();
    void fetchTriviaQuestion(const QString& apiUrl);


private:
    Ui::MainWindow *ui;
    QString generateChatbotReply(const QString& message);
    QString getRandomJoke();
    QString m_lastJoke;
    QNetworkAccessManager* networkManager;



    struct Question {
        QString question;
        QStringList options;
        QString correctAnswer;
        bool isValid;

        Question()
            : isValid(false) {}

        void clear()
        {
            question.clear();
            options.clear();
            correctAnswer.clear();
            isValid = false;
        }
    };

    Question m_currentQuestion;
    QString decodeHtmlEntities(const QString& text);
};



#endif // MAINWINDOW_H

