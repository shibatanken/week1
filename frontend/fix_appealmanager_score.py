#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

# Update appealmanager.cpp to show current score and new score in review dialog

file_path = 'F:/LTM/week1/frontend/appealmanager.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find onReviewAppeal function and modify it
new_lines = []
i = 0
while i < len(lines):
    if 'void AppealManager::onReviewAppeal(int appealId, QString status)' in lines[i]:
        # Found the function - need to pass appeal object instead of just appealId
        # But first, let's update the call site
        new_lines.append(lines[i])
        i += 1
    elif 'connect(approveBtn, &QPushButton::clicked, [this, appealId]() {' in lines[i]:
        # Update the approve button lambda to pass appeal object
        new_lines.append('            int appealId = appeal["id"].toInt();\n')
        new_lines.append('            double currentScore = appeal["current_score"].toDouble();\n')
        new_lines.append('            QString examName = appeal["exam_name"].toString();\n')
        new_lines.append('            connect(approveBtn, &QPushButton::clicked, [this, appealId, currentScore, examName]() {\n')
        new_lines.append('                onReviewAppeal(appealId, "approved", currentScore, examName);\n')
        i += 2  # Skip old line and the next one
    elif 'connect(rejectBtn, &QPushButton::clicked, [this, appealId]() {' in lines[i]:
        # Update the reject button lambda
        new_lines.append('            connect(rejectBtn, &QPushButton::clicked, [this, appealId, currentScore, examName]() {\n')
        new_lines.append('                onReviewAppeal(appealId, "rejected", currentScore, examName);\n')
        i += 2  # Skip old line and the next one
    else:
        new_lines.append(lines[i])
        i += 1

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("[OK] Phase 1: Updated appealmanager.cpp button connections")
