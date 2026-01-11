#!/usr/bin/env python3
# Fix BUG #3: Add question button adds 2 questions
# Root cause: Qt auto-connects on_<objectName>_<signal> + manual connect = double trigger

import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/frontend/createexam.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Remove manual connect for addQuestionButton (Qt auto-connects it)
old_line = '    connect(ui->addQuestionButton, &QPushButton::clicked, this, &CreateExam::on_addQuestionButton_clicked);'
new_line = '    // Qt auto-connects on_addQuestionButton_clicked - no manual connect needed'

if old_line in content:
    content = content.replace(old_line, new_line)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Fixed BUG #3: Removed duplicate connect for addQuestionButton")
else:
    print("[SKIP] Pattern not found or already fixed")
