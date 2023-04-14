#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QVector>

class MyWidget : public QWidget {
public:
    MyWidget(QWidget *parent = nullptr) : QWidget(parent) {
        // Create the genre selector menu
        QComboBox *genreSelector = new QComboBox(this);
        genreSelector->addItem("Action");
        genreSelector->addItem("Comedy");
        genreSelector->addItem("Drama");

        // Create the percentage range selectors
        QDoubleSpinBox *startRangeSelector = new QDoubleSpinBox(this);
        startRangeSelector->setSuffix("%");
        startRangeSelector->setRange(0, 100);
        startRangeSelector->setSingleStep(0.1);
        QDoubleSpinBox *endRangeSelector = new QDoubleSpinBox(this);
        endRangeSelector->setSuffix("%");
        endRangeSelector->setRange(0, 100);
        endRangeSelector->setSingleStep(0.1);

        // Create the generate list button
        QPushButton *generateListButton = new QPushButton("Generate List", this);

        // Create the layout
        QHBoxLayout *rangeLayout = new QHBoxLayout();
        rangeLayout->addWidget(startRangeSelector);
        rangeLayout->addWidget(endRangeSelector);
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(generateListButton);
        QVBoxLayout *layout = new QVBoxLayout();
        layout->addWidget(genreSelector);
        layout->addLayout(rangeLayout);
        layout->addLayout(buttonLayout);

        // Set the layout
        setLayout(layout);
    }
};

int main(int argc, char **argv) {
    // import the data and select columns
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


    QApplication app(argc, argv);

    // Create an instance of the UI
    MyWidget widget;

    // Show the UI
    widget.show();

    // Run the application
    return app.exec();
}