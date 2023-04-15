#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <QtCore/QVector>

//read data helper
std::vector<std::vector<std::string>> readData(){
    std::vector<std::vector<std::string>> content;
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file ("clean_data/rt_movies.csv", std::ios::in);
    if(file.is_open())
    {
        while(getline(file, line))
        {
            row.clear();

            std::stringstream str(line);

            while(getline(str, word, ','))
                row.push_back(word);
            content.push_back(row);
        }
    }
    file.close();
    return content;
}

//sort by tomato ranking and return ten items within ranking range and genre
std::vector<std::vector<std::string>> organizeData(std::vector<std::vector<std::string>> data, std::string genre, double min, double max){
    //sort vector by vector[3] (tomato meter ranking)
    bool use;
    std::vector<std::vector<std::string>> chosenOnes;
    std::vector<std::vector<std::string>> ans;
    for (int i = 1; i < data.size(); ++i) {
                int rank = std::stoi(data.at(i).at(2));
                std::string currGenre = data.at(i).at(1).substr(0, 3);
                if (rank > min && rank < max && currGenre == genre.substr(0, 3)){
                    chosenOnes.push_back(data.at(i));
                }
    }
    std::srand(std::time(nullptr));
    int numbers[10];
    for (int i = 0; i < 10; i++) {
        numbers[i] = std::rand() % chosenOnes.size() + 1;
        ans.push_back(chosenOnes.at(numbers[i]));
    }

    return ans;
}

class MyWidget : public QWidget {
public:
    QLabel *genreLabel = new QLabel("Genre: ", this);
    QComboBox *genreSelector = new QComboBox(this);
    QLabel *rangeLabel = new QLabel("Select Score Range:", this);
    QDoubleSpinBox *startRangeSelector = new QDoubleSpinBox(this);
    QDoubleSpinBox *endRangeSelector = new QDoubleSpinBox(this);
    QPushButton *generateListButton = new QPushButton("Generate List", this);
    QTableWidget *table = new QTableWidget(this);
    std::vector<std::vector<std::string>> shortList;

    void onGenerateClick(){
        shortList = organizeData(readData(),
                     genreSelector->currentText().toStdString(),
                     startRangeSelector->value(),
                     endRangeSelector->value());
        for (int i = 0; i < shortList.size(); ++i) {
            QTableWidgetItem *title = new QTableWidgetItem(QString::fromStdString(shortList.at(i).at(0)));
            QTableWidgetItem *genre = new QTableWidgetItem(QString::fromStdString(shortList.at(i).at(1)));
            QTableWidgetItem *rating =  new QTableWidgetItem(QString::fromStdString(shortList.at(i).at(2)));
            table->setItem(i, 0, title);
            table->setItem(i, 1, genre);
            table->setItem(i, 2, rating);
        }
    }
    MyWidget(QWidget *parent = nullptr) : QWidget(parent) {
        bool displayList = false;
        // Create the genre selector menu
        QFont font("Arial", 12);

        genreLabel->setBuddy(genreSelector);
        genreSelector->addItem("Action");
        genreSelector->addItem("Comedy");
        genreSelector->addItem("Drama");

        // Create the percentage range selectors
        rangeLabel->setBuddy(startRangeSelector);
        startRangeSelector->setSuffix("%");
        startRangeSelector->setRange(0, 100);
        startRangeSelector->setSingleStep(10);
        startRangeSelector->setValue(0);

        endRangeSelector->setSuffix("%");
        endRangeSelector->setRange(0, 100);
        endRangeSelector->setSingleStep(10);
        endRangeSelector->setValue(100);



        // Create the generate list button
        connect(generateListButton, &QPushButton::pressed, this, &MyWidget::onGenerateClick);


        //display table with title, rating, and genre of ten movies
        table->setRowCount(10);
        table->setColumnCount(3);
        table->setHorizontalHeaderLabels(QString("Title;Rating;Genre").split(";"));
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);



        // Create the layout
        QHBoxLayout *rangeLayout = new QHBoxLayout();
        rangeLayout->addWidget(rangeLabel);
        rangeLayout->addWidget(startRangeSelector);
        rangeLayout->addWidget(endRangeSelector);
        rangeLayout->setAlignment(Qt::AlignLeft);
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(generateListButton);
        QVBoxLayout *layout = new QVBoxLayout();
        QHBoxLayout *genreLayout = new QHBoxLayout();
        genreLayout->addWidget(genreLabel);
        genreLayout->addWidget(genreSelector);
        genreLayout->setAlignment(Qt::AlignLeft);
        layout->addLayout(genreLayout);
        layout->addLayout(rangeLayout);
        layout->addLayout(buttonLayout);
        layout->addWidget(table);

        // Set the layout
        setLayout(layout);
    }

};

int main(int argc, char **argv) {



    QApplication app(argc, argv);

    // Create an instance of the UI
    MyWidget widget;
    widget.setMinimumSize(800, 500);
    widget.setWindowTitle("Find Me Movie");

    // Show the UI
    widget.show();

    // Run the application
    return app.exec();
}

//quick sort function: take in vec of vec of str, returns vec of vec of str
//counting sort function: take in vec of vec of str, returns vec of vec of str