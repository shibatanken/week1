#include <QApplication>
#include "examresults.h"
#include "examscores.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Test ExamResults (Student View)
    // Replace with your actual exam_id and user_id
    ExamResults *results = new ExamResults();
    results->loadResults(1, 1); // exam_id=1, user_id=1
    results->show();

    // Or test ExamScores (Teacher View)
    // ExamScores *scores = new ExamScores();
    // scores->loadScores(1); // exam_id=1
    // scores->show();

    return app.exec();
}
