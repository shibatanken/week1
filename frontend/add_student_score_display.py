#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/frontend/appealmanager.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add score display for students after teacher_comment section
old_comment = '''        // Teacher comment if any
        if (!appeal["teacher_comment"].toString().isEmpty()) {
            QLabel *commentLabel = new QLabel("Bình luận: " + appeal["teacher_comment"].toString());
            commentLabel->setWordWrap(true);
            commentLabel->setStyleSheet("color: #FF9800; font-style: italic; padding: 5px; background-color: #FFF3E0; border-radius: 4px;");
            cardLayout->addWidget(commentLabel);
        }'''

new_comment = '''        // Teacher comment if any
        if (!appeal["teacher_comment"].toString().isEmpty()) {
            QLabel *commentLabel = new QLabel("💬 Bình luận: " + appeal["teacher_comment"].toString());
            commentLabel->setWordWrap(true);
            commentLabel->setStyleSheet("color: #FF9800; font-style: italic; padding: 5px; background-color: #FFF3E0; border-radius: 4px;");
            cardLayout->addWidget(commentLabel);
        }

        // Score adjustment display for students (if resolved)
        if (!isTeacherMode && status != "pending") {
            double currentScore = appeal["current_score"].toDouble();
            double scoreAdj = appeal["score_adjustment"].toDouble();
            double newScore = currentScore + scoreAdj;

            if (scoreAdj != 0.0) {
                QString scoreText = QString("📊 Điểm số: %1 → %2 (%3%4)")
                    .arg(currentScore, 0, 'f', 1)
                    .arg(newScore, 0, 'f', 1)
                    .arg(scoreAdj > 0 ? "+" : "")
                    .arg(scoreAdj, 0, 'f', 1);
                QLabel *scoreChangeLabel = new QLabel(scoreText);
                scoreChangeLabel->setStyleSheet(scoreAdj > 0 ?
                    "font-weight: bold; color: #4CAF50; font-size: 16px; padding: 8px; background-color: #E8F5E9; border-radius: 4px;" :
                    "font-weight: bold; color: #F44336; font-size: 16px; padding: 8px; background-color: #FFEBEE; border-radius: 4px;");
                cardLayout->addWidget(scoreChangeLabel);
            } else {
                QLabel *noChangeLabel = new QLabel(QString("📊 Điểm số: %1 (không thay đổi)").arg(currentScore, 0, 'f', 1));
                noChangeLabel->setStyleSheet("color: #666; font-style: italic;");
                cardLayout->addWidget(noChangeLabel);
            }
        }'''

if old_comment in content:
    content = content.replace(old_comment, new_comment)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Added score adjustment display for students")
else:
    print("[SKIP] Score display already added or pattern not found")
