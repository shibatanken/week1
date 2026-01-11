#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/frontend/examlist.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find line with "// Score and status" around line 104
new_lines = []
i = 0
while i < len(lines):
    if i == 103 and '// Score and status' in lines[i]:
        # Replace this entire section (lines 104-131)
        new_lines.append(lines[i])  # // Score and status
        new_lines.append(lines[i+1])  # QHBoxLayout *infoLayout = new QHBoxLayout();
        new_lines.append(lines[i+2])  # blank
        new_lines.append(lines[i+3])  # QString status = exam["status"].toString();
        new_lines.append(lines[i+4])  # double score = exam["score"].toDouble();
        new_lines.append(lines[i+5])  # int correct = exam["correct_answers"].toInt();
        new_lines.append(lines[i+6])  # int total = exam["total_questions"].toInt();
        new_lines.append('        int submissionId = exam["submission_id"].toInt();\n')
        new_lines.append('        int examId = exam["exam_id"].toInt();\n')
        new_lines.append('\n')
        new_lines.append('        if (status == "submitted") {\n')
        new_lines.append('            // SUBMITTED: Show score\n')
        new_lines.append('            QLabel *scoreLabel = new QLabel(QString("Điểm: %1 (%2/%3 câu đúng)")\n')
        new_lines.append('                .arg(score, 0, \'f\', 1).arg(correct).arg(total));\n')
        new_lines.append('            scoreLabel->setStyleSheet("font-weight: bold; color: #4CAF50;");\n')
        new_lines.append('            infoLayout->addWidget(scoreLabel);\n')
        new_lines.append('\n')
        new_lines.append('            QLabel *statusLabel = new QLabel("✓ Đã nộp");\n')
        new_lines.append('            statusLabel->setStyleSheet("color: green;");\n')
        new_lines.append('            infoLayout->addWidget(statusLabel);\n')
        new_lines.append('\n')
        new_lines.append('            infoLayout->addStretch();\n')
        new_lines.append('            cardLayout->addLayout(infoLayout);\n')
        new_lines.append('\n')
        new_lines.append('            // View result button (for submitted exams)\n')
        new_lines.append('            QPushButton *viewBtn = new QPushButton("Xem chi tiết");\n')
        new_lines.append('            viewBtn->setStyleSheet("background-color: #2196F3; color: white; padding: 8px 16px; border-radius: 4px;");\n')
        new_lines.append('            connect(viewBtn, &QPushButton::clicked, [this, submissionId]() {\n')
        new_lines.append('                emit viewExamResult(submissionId);\n')
        new_lines.append('            });\n')
        new_lines.append('            cardLayout->addWidget(viewBtn);\n')
        new_lines.append('        } else {\n')
        new_lines.append('            // IN_PROGRESS: Show status message\n')
        new_lines.append('            QLabel *statusLabel = new QLabel("⏳ Đang làm dở - Cần hoàn thành!");\n')
        new_lines.append('            statusLabel->setStyleSheet("color: orange; font-weight: bold;");\n')
        new_lines.append('            infoLayout->addWidget(statusLabel);\n')
        new_lines.append('\n')
        new_lines.append('            infoLayout->addStretch();\n')
        new_lines.append('            cardLayout->addLayout(infoLayout);\n')
        new_lines.append('\n')
        new_lines.append('            // Continue exam button (for in-progress exams)\n')
        new_lines.append('            QString examName = exam["exam_name"].toString();\n')
        new_lines.append('            int timeLimit = exam["time_limit"].toInt();\n')
        new_lines.append('            QPushButton *continueBtn = new QPushButton("Tiếp tục làm bài");\n')
        new_lines.append('            continueBtn->setStyleSheet("background-color: #FF9800; color: white; padding: 8px 16px; border-radius: 4px;");\n')
        new_lines.append('            connect(continueBtn, &QPushButton::clicked, [this, examId, examName, timeLimit]() {\n')
        new_lines.append('                emit startExam(examId, examName, timeLimit);\n')
        new_lines.append('            });\n')
        new_lines.append('            cardLayout->addWidget(continueBtn);\n')
        new_lines.append('        }\n')
        # Skip old lines 104-131 (28 lines total)
        i += 28
    else:
        new_lines.append(lines[i])
        i += 1

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("[OK] Fixed examlist.cpp to handle submitted vs in-progress exams")
