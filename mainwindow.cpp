#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <random>
#include <cstdlib>
#include <ctime>
#include <QJsonArray>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    networkManager(new QNetworkAccessManager(this))


{
    ui->setupUi(this);
    connect(ui->SendButton, &QPushButton::clicked, this, &MainWindow::on_SendButton_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}



QString MainWindow::getRandomJoke()
{
    QStringList jokes;
    jokes << "Why don't scientists trust atoms? Because they make up everything!"
          << "Why did the bicycle fall over? Because it was two-tired!"
          << "What do you call fake spaghetti? An impasta!"
          << "Why don't eggs tell jokes? Because they might crack up!"
          << "What do you call a train carrying bubble-gum? A chew-chew train!";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, jokes.size() - 1);
    int index = dist(gen);
    return jokes[index];
}

QString MainWindow::generateChatbotReply(const QString& message)
{
    QString reply;
    QString lowercaseMessage = message.toLower();

    if (lowercaseMessage.contains("hello") || lowercaseMessage.contains("hi"))
    {
        reply = "Hello!";
    }
    else if (lowercaseMessage.contains("how are you"))
    {
        reply = "I'm doing well, thank you! How about you?";
    }
    else if (lowercaseMessage.contains("fine") || lowercaseMessage.contains("good"))
    {
        reply = "That's great to hear!";
    }
    else if (lowercaseMessage.contains("bad"))
    {
        reply = "I'm sorry to hear that.";
    }
    else if (lowercaseMessage.contains("my name"))
    {
        reply= "I am sorry but I don't know your name";
    }
    else if (lowercaseMessage.contains("joke"))
    {
        QString joke = getRandomJoke();
        m_lastJoke = joke;
        reply = joke;
    }
    else if (lowercaseMessage.contains("another") && lowercaseMessage.contains("joke"))
    {
        if (!m_lastJoke.isEmpty())
        {
            QString anotherJoke = getRandomJoke();
            m_lastJoke = anotherJoke;
            reply = anotherJoke;
        }
        else
        {
            reply = "Sorry, I don't have another joke at the moment.";
        }
    }
    else if (lowercaseMessage.contains("sad"))
    {
        reply = "Don't be sad. Everything's gonna be alright. Cheer up!";
    }
    else if (lowercaseMessage.contains("happy"))
    {
        reply = "Glad to hear!";
    }
    else if (lowercaseMessage.contains("thank"))
    {
        reply = "Your welcome!";
    }
    else if (lowercaseMessage.contains("ok"))
    {
        reply="Yes, ok!";
    }

    else if (lowercaseMessage.contains("play game") || lowercaseMessage.contains("play a game"))
    {
        reply = "Sure! Let's play Rock, Paper, Scissors. Choose one: rock, paper, or scissors.";
    }
    else if (lowercaseMessage.contains("rock") || lowercaseMessage.contains("paper") || lowercaseMessage.contains("scissors"))
    {
        srand(time(0));
        int computerMove = rand() % 3;
        QString userMove;
        if (lowercaseMessage.contains("rock"))
        {
            userMove = "rock";
        }
        else if (lowercaseMessage.contains("paper"))
        {
            userMove = "paper";
        }
        else if (lowercaseMessage.contains("scissors"))
        {
            userMove = "scissors";
        }
        QString result;
        if ((userMove == "rock" && computerMove == 2) || (userMove == "paper" && computerMove == 0) || (userMove == "scissors" && computerMove == 1))
        {
            result = "You win!";
        }
        else if (userMove == "rock" && computerMove == 1)
        {
            result = "I win!";
        }
        else if (userMove == "paper" && computerMove == 2)
        {
            result = "I win!";
        }
        else if (userMove == "scissors" && computerMove == 0)
        {
            result = "I win!";
        }
        else
        {
            result = "It's a tie!";
        }
        reply = "You chose " + userMove + ". I chose ";
        if (computerMove == 0)
        {
            reply += "rock.";
        }
        else if (computerMove == 1)
        {
            reply += "paper.";
        }
        else if (computerMove == 2)
        {
            reply += "scissors.";
        }
        reply += " " + result;
    }
    else if (lowercaseMessage.contains("your name"))
    {
        reply = "I don't actually have a name. But you can call me 'My Chatbot'.";
    }
    else if (lowercaseMessage.contains("quiz")|| lowercaseMessage.contains("question")|| lowercaseMessage.contains("next"))
    {
        QString apiUrl = "https://opentdb.com/api.php?amount=1&category=18&difficulty=easy&type=multiple";
        fetchTriviaQuestion(apiUrl);
        // Return an empty reply for now, the trivia question will be displayed asynchronously
        return "";
    }
    else
    {
        reply = "I'm sorry, I didn't quite understand. Can you please rephrase your message?";
    }

    return reply;
}

void MainWindow::on_SendButton_clicked()
{
    sendMessage();
}

void MainWindow::sendMessage()
{
    QString message = ui->messageTextEdit->toPlainText();
    message = message.trimmed();
    if (message.isEmpty())
    {
        ui->messageTextEdit->clear();
        return;
    }
    ui->chatDisplayTextEdit->append("User: " + message);
    ui->messageTextEdit->clear();

    if (m_currentQuestion.isValid && !m_currentQuestion.options.isEmpty())
    {
        // User provided an answer to the quiz
        bool isValidAnswer = false;
        int selectedOption = message.toInt(&isValidAnswer);
        if (isValidAnswer && selectedOption >= 1 && selectedOption <= m_currentQuestion.options.size())
        {
            QString userAnswer = m_currentQuestion.options[selectedOption - 1];
            if (userAnswer == m_currentQuestion.correctAnswer)
            {
                ui->chatDisplayTextEdit->append("Chatbot: Correct answer!");
            }
            else
            {
                ui->chatDisplayTextEdit->append("Chatbot: Incorrect answer. The correct answer is: " + m_currentQuestion.correctAnswer);
            }
        }
        else
        {
            ui->chatDisplayTextEdit->append("Chatbot: Invalid answer. Please provide a valid option number.");
        }

        m_currentQuestion.clear(); // Clear the current question for the next quiz
    }
    else
    {
        QString reply = generateChatbotReply(message);
        if (!reply.isEmpty())
        {
            ui->chatDisplayTextEdit->append("Chatbot: " + reply);
        }
    }
}

// ...

void MainWindow::fetchTriviaQuestion(const QString& apiUrl)
{
    QNetworkRequest request;
    request.setUrl(QUrl(apiUrl));

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray response = reply->readAll();
            QJsonDocument jsonDoc(QJsonDocument::fromJson(response));
            QJsonObject jsonObject = jsonDoc.object();
            QJsonArray results = jsonObject["results"].toArray();
            if (!results.isEmpty()) {
                QJsonObject triviaQuestion = results[0].toObject();
                m_currentQuestion.question = triviaQuestion["question"].toString();
                m_currentQuestion.correctAnswer = triviaQuestion["correct_answer"].toString();
                QJsonArray incorrectAnswers = triviaQuestion["incorrect_answers"].toArray();
                for (const QJsonValue& value : incorrectAnswers)
                {
                    m_currentQuestion.options.append(value.toString());
                }

                // Add the correct answer to the options list
                m_currentQuestion.options.append(m_currentQuestion.correctAnswer);

                // Shuffle the options to randomize their order
                std::random_device rd;
                std::mt19937 gen(rd());
                std::shuffle(m_currentQuestion.options.begin(), m_currentQuestion.options.end(), gen);

                // Remove HTML entities from the question and options
                m_currentQuestion.question = decodeHtmlEntities(m_currentQuestion.question);
                for (QString& option : m_currentQuestion.options)
                {
                    option = decodeHtmlEntities(option);
                }

                ui->chatDisplayTextEdit->append("Chatbot: " + m_currentQuestion.question);

                for (int i = 0; i < m_currentQuestion.options.size(); ++i)
                {
                    QString optionNumber = QString::number(i + 1);
                    QString optionText = m_currentQuestion.options[i];
                    ui->chatDisplayTextEdit->append(optionNumber + ". " + optionText);
                }

                m_currentQuestion.isValid = true;
            }
            else {
                ui->chatDisplayTextEdit->append("Chatbot: Sorry, I couldn't fetch a trivia question.");
            }
        } else {
            QString error = reply->errorString();
            qDebug() << "Network error:" << error;
            ui->chatDisplayTextEdit->append("Chatbot: Sorry, I couldn't fetch a trivia question. Error: " + error);
        }
        reply->deleteLater();
    });
}

// ...


QString MainWindow::decodeHtmlEntities(const QString& text)
{
    QString decodedText = text;
    decodedText.replace("&quot;", "\"");
    decodedText.replace("&amp;", "&");
    decodedText.replace("&lt;", "<");
    decodedText.replace("&gt;", ">");
    decodedText.replace("&#039;", "'");
    return decodedText;
}
