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
std::vector<std::vector<std::string>> readData(){ //read data from csv file
    std::vector<std::vector<std::string>> content;
    std::vector<std::string> row;
    std::string line, word;

    std::fstream file ("clean_data/rt_movies.csv", std::ios::in); //open file
    if(file.is_open()) //if file is open iterate through file
    {
        while(getline(file, line))
        {
            row.clear();

            std::stringstream str(line);

            while(getline(str, word, ',')) //split line by comma
                row.push_back(word);
            content.push_back(row);
        }
    }
    file.close();
    return content;
}

//counting sort
std::vector<std::vector<std::string>> countingSort(std::vector<std::vector<std::string>> filteredData,
                                                   std::string genre, double min, double max) {
    // Create an array to store the counts of each rank
    int counts[101] = {0};//initialize array to 0

    // Count the number of times each rank appears
    for (int i = 1; i < filteredData.size(); i++) {
        int rank = std::stoi(filteredData.at(i).at(2));
        if (rank >= 0 && rank <= 100) {
            counts[rank]++;
        }
    }

    // Modify the count array to get the actual position of each rank
    for (int i = 1; i <= 100; i++) {
        counts[i] += counts[i-1];
    }

    // Create a new vector to store the sorted data
    std::vector<std::vector<std::string>> sortedData(filteredData.size(), std::vector<std::string>(3));

    // Sort the data by rank using the count array
    for (int i = filteredData.size() - 1; i >= 1; i--) {
        int rank = std::stoi(filteredData.at(i).at(2));
        sortedData[counts[rank]-1] = filteredData.at(i);
        counts[rank]--;
    }

    return sortedData;
}

//quick sort
int partition(std::vector<std::vector<std::string>>& data, int low, int high) { //partition function for quick sort
    double pivot = std::stod(data.at(high).at(2));//pivot is the last element
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) { //iterate through array
        double currRank = std::stod(data.at(j).at(2));
        if (currRank <= pivot) { //if current rank is less than pivot swap
            i++;
            std::swap(data.at(i), data.at(j));
        }
    }
    std::swap(data.at(i + 1), data.at(high));
    return (i + 1);
}

void quickSort(std::vector<std::vector<std::string>>& data, int low, int high) {
    if (low < high) { //if low is less than high partition array and sort
        int pi = partition(data, low, high);

        quickSort(data, low, pi - 1);
        quickSort(data, pi + 1, high);
    }
}

std::vector<std::vector<std::string>> quickSortHelper(std::vector<std::vector<std::string>> filteredData, std::string genre, double min, double max) {
    // Filter data based on genre and rank range
    quickSort(filteredData, 0, filteredData.size() - 1); //sort data
    return filteredData;
}



class MyWidget : public QWidget { //main widget
public:
    //create widgets
    QLabel *genreLabel = new QLabel("Genre: ", this);
    QComboBox *genreSelector = new QComboBox(this);
    QLabel *rangeLabel = new QLabel("Select Score Range:", this);
    QDoubleSpinBox *startRangeSelector = new QDoubleSpinBox(this);
    QDoubleSpinBox *endRangeSelector = new QDoubleSpinBox(this);
    QPushButton *generateListButton = new QPushButton("Generate List", this);
    QComboBox *algorithmSelector = new QComboBox(this);
    QLabel *timeDisplay = new QLabel(this);

    QTableWidget *table = new QTableWidget(this);
    std::vector<std::vector<std::string>> shortList;

    void onGenerateClick(){ //when generate list button is clicked
        QString selectedAlgorithm = algorithmSelector->currentText();

        // read and sort data
        std::vector<std::vector<std::string>> data = readData(); //read data from csv file
        std::vector<std::vector<std::string>> filteredData;
        std::string genre = genreSelector->currentText().toStdString(); //get genre from combo box
        double min = startRangeSelector->value(); //get min score from spin box
        double max = endRangeSelector->value(); //get max score from spin box

        for (int i = 1; i < data.size(); i++) { //filter data based on genre and score range
            int rank = std::stoi(data.at(i).at(2));
            std::string rowGenre = data.at(i).at(1);
            if (rowGenre.find(genre) != std::string::npos && rank >= min && rank <= max) {
                filteredData.push_back(data.at(i));
            }
        }

        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now(); //start timer
        if (selectedAlgorithm == "Counting Sort") { //sort data based on selected algorithm
            shortList = countingSort(filteredData,
                                           genreSelector->currentText().toStdString(),
                                           startRangeSelector->value(),
                                           endRangeSelector->value());
        }
        else {
            shortList = quickSortHelper(filteredData,
                                        genreSelector->currentText().toStdString(),
                                        startRangeSelector->value(),
                                        endRangeSelector->value());
        }
        std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now(); //end timer
        std::chrono::duration<double> timeTaken = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        timeDisplay->setText(QString::number(timeTaken.count()) + " seconds"); //display time taken

        std::srand(std::time(nullptr)); //randomize list of movies to display 10 random movies
        int numbers[10];
        std::vector<std::vector<std::string>> ans;
        for (int i = 0; i < 10; i++) {
            numbers[i] = std::rand() % shortList.size() + 1;
            ans.push_back(shortList.at(numbers[i]));
        }

        for (int i = 0; i < ans.size(); ++i) { //display list of movies
            QTableWidgetItem *title = new QTableWidgetItem(QString::fromStdString(ans.at(i).at(0)));
            QTableWidgetItem *genre = new QTableWidgetItem(QString::fromStdString(ans.at(i).at(1)));
            QTableWidgetItem *rating =  new QTableWidgetItem(QString::fromStdString(ans.at(i).at(2)));
            table->setItem(i, 0, title);
            table->setItem(i, 1, genre);
            table->setItem(i, 2, rating);
        }
    }
    MyWidget(QWidget *parent = nullptr) : QWidget(parent) { //construct and display widgets
        bool displayList = false;
        // Create the genre selector menu
        QFont font("Arial", 12);

        genreLabel->setBuddy(genreSelector); //set buddy for genre label
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

        QLabel *algorithmLabel = new QLabel("Select Algorithm:", this); //create algorithm label

        algorithmSelector->addItem("Counting Sort"); //add algorithms to combo box
        algorithmSelector->addItem("Quick Sort");

        // Create the 'generate list' button
        connect(generateListButton, &QPushButton::pressed, this, &MyWidget::onGenerateClick);

        QLabel *timeLabel = new QLabel("Time Taken: ", this);


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


        QHBoxLayout *genreLayout = new QHBoxLayout();
        genreLayout->addWidget(genreLabel);
        genreLayout->addWidget(genreSelector);
        genreLayout->setAlignment(Qt::AlignLeft);

        QHBoxLayout *algorithmLayout = new QHBoxLayout();
        algorithmLayout->addWidget(algorithmLabel);
        algorithmLayout->addWidget(algorithmSelector);
        algorithmLayout->setAlignment(Qt::AlignLeft);

        QHBoxLayout *timeLayout = new QHBoxLayout();
        timeLayout->addWidget(timeLabel);
        timeLayout->addWidget(timeDisplay);
        timeLayout->setAlignment(Qt::AlignLeft);

        QVBoxLayout *layout = new QVBoxLayout();
        layout->addLayout(genreLayout);
        layout->addLayout(rangeLayout);
        layout->addLayout(algorithmLayout);
        layout->addLayout(buttonLayout);
        layout->addLayout(timeLayout);
        layout->addWidget(table);

        // Set the layout
        setLayout(layout);
    }

};

int main(int argc, char **argv) {

    QApplication app(argc, argv); // Create a Qt application

    // Create an instance of the UI
    MyWidget widget;
    widget.setMinimumSize(800, 500);
    widget.setWindowTitle("Find Me Movie");

    // Show the UI
    widget.show();

    // Run the application
    return app.exec();
}