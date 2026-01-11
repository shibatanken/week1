#!/usr/bin/env python3
# Fix examlist.cpp to handle submitted vs in-progress exams

file_path = 'F:/LTM/week1/frontend/examlist.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Find and replace the exam display section
old_code = '''        // Score and status
        QHBoxLayout *infoLayout = new QHBoxLayout();

        QString status = exam["status"].toString();
        double score = exam["score"].toDouble();
        int correct = exam["correct_answers"].toInt();
        int total = exam["total_questions"].toInt();

        QLabel *scoreLabel = new QLabel(QString("Điểm: %1 (%2/%3 câu đúng)")
            .arg(score, 0, 'f', 1).arg(correct).arg(total));
        scoreLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
        infoLayout->addWidget(scoreLabel);

        QLabel *statusLabel = new QLabel(status == "submitted" ? "✓ Đã nộp" : "⏳ Đang làm");
        statusLabel->setStyleSheet(status == "submitted" ? "color: green;" : "color: orange;");
        infoLayout->addWidget(statusLabel);

        infoLayout->addStretch();
        cardLayout->addLayout(infoLayout);

        // View result button
        int submissionId = exam["submission_id"].toInt();
        QPushButton *viewBtn = new QPushButton("Xem chi tiết");
        viewBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 4px;");
        connect(viewBtn, &QPushButton::clicked, [this, submissionId]() {
            emit viewExamResult(submissionId);
        });
        cardLayout->addWidget(viewBtn);'''

new_code = '''        // Score and status
        QHBoxLayout *infoLayout = new QHBoxLayout();

        QString status = exam["status"].toString();
        double score = exam["score"].toDouble();
        int correct = exam["correct_answers"].toInt();
        int total = exam["total_questions"].toInt();
        int submissionId = exam["submission_id"].toInt();
        int examId = exam["exam_id"].toInt();

        if (status == "submitted") {
            // SUBMITTED: Show score
            QLabel *scoreLabel = new QLabel(QString("Điểm: %1 (%2/%3 câu đúng)")
                .arg(score, 0, 'f', 1).arg(correct).arg(total));
            scoreLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");
            infoLayout->addWidget(scoreLabel);

            QLabel *statusLabel = new QLabel("✓ Đã nộp");
            statusLabel->setStyleSheet("color: green;");
            infoLayout->addWidget(statusLabel);

            infoLayout->addStretch();
            cardLayout->addLayout(infoLayout);

            // View result button (for submitted exams)
            QPushButton *viewBtn = new QPushButton("Xem chi tiết");
            viewBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 4px;");
            connect(viewBtn, &QPushButton::clicked, [this, submissionId]() {
                emit viewExamResult(submissionId);
            });
            cardLayout->addWidget(viewBtn);
        } else {
            // IN_PROGRESS: Show status message
            QLabel *statusLabel = new QLabel("⏳ Đang làm dở - Cần hoàn thành!");
            statusLabel->setStyleSheet("color: orange; font-weight: bold;");
            infoLayout->addWidget(statusLabel);

            infoLayout->addStretch();
            cardLayout->addLayout(infoLayout);

            // Continue exam button (for in-progress exams)
            QString examName = exam["exam_name"].toString();
            int timeLimit = exam["time_limit"].toInt();
            QPushButton *continueBtn = new QPushButton("Tiếp tục làm bài");
            continueBtn->setStyleSheet("background-color: #FF9800; color: white; padding: 8px 16px; border-radius: 4px;");
            connect(continueBtn, &QPushButton::clicked, [this, examId, examName, timeLimit]() {
                emit startExam(examId, examName, timeLimit);
            });
            cardLayout->addWidget(continueBtn);
        }'''

if old_code in content:
    content = content.replace(old_code, new_code)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Fixed examlist.cpp")
else:
    print("[FAIL] Pattern not found in examlist.cpp")
