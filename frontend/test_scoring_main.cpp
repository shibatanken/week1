#include <QApplication>
#include "examresults.h"
#include "examscores.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Test Student Results View
    ExamResults *results = new ExamResults();
    results->loadResults(1, 1); // exam_id=1, user_id=1
    results->show();

    // Uncomment to test Teacher Scores View instead:
    // ExamScores *scores = new ExamScores();
    // scores->loadScores(1); // exam_id=1
    // scores->show();

    return app.exec();
}
