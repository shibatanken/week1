#!/usr/bin/env python3
import sys
sys.stdout.reconfigure(encoding='utf-8')

file_path = 'F:/LTM/week1/frontend/appealmanager.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Step 1: Update onReviewAppeal signature to accept current_score
old_sig = 'void AppealManager::onReviewAppeal(int appealId, QString status)'
new_sig = 'void AppealManager::onReviewAppeal(int appealId, QString status, double currentScore, QString examName)'

# Step 2: Update the dialog to show current score
old_dialog_start = '''    // Create dialog for review
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(status == "approved" ? "Duyệt khiếu nại" : "Từ chối khiếu nại");
    dialog->setMinimumSize(400, 300);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // Teacher comment field (optional)'''

new_dialog_start = '''    // Create dialog for review
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(status == "approved" ? "Duyệt khiếu nại" : "Từ chối khiếu nại");
    dialog->setMinimumSize(450, 400);

    QVBoxLayout *layout = new QVBoxLayout(dialog);

    // Show exam name
    QLabel *examLabel = new QLabel("Bài thi: " + examName);
    examLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-bottom: 10px;");
    layout->addWidget(examLabel);

    // Show current score
    QLabel *scoreLabel = new QLabel(QString("Điểm hiện tại: %1").arg(currentScore, 0, 'f', 1));
    scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2196F3; margin-bottom: 10px;");
    layout->addWidget(scoreLabel);

    // Teacher comment field (optional)'''

# Step 3: Update button connections to pass currentScore and examName
old_approve = '''            connect(approveBtn, &QPushButton::clicked, [this, appealId]() {
                onReviewAppeal(appealId, "approved");
            });'''

new_approve = '''            double currentScore = appeal["current_score"].toDouble();
            QString examName = appeal["exam_name"].toString();
            connect(approveBtn, &QPushButton::clicked, [this, appealId, currentScore, examName]() {
                onReviewAppeal(appealId, "approved", currentScore, examName);
            });'''

old_reject = '''            connect(rejectBtn, &QPushButton::clicked, [this, appealId]() {
                onReviewAppeal(appealId, "rejected");
            });'''

new_reject = '''            connect(rejectBtn, &QPushButton::clicked, [this, appealId, currentScore, examName]() {
                onReviewAppeal(appealId, "rejected", currentScore, examName);
            });'''

fixed_count = 0

if old_sig in content:
    content = content.replace(old_sig, new_sig)
    print("[OK] Updated onReviewAppeal signature")
    fixed_count += 1
else:
    print("[SKIP] Signature already updated or not found")

if old_dialog_start in content:
    content = content.replace(old_dialog_start, new_dialog_start)
    print("[OK] Added current score display in dialog")
    fixed_count += 1
else:
    print("[SKIP] Dialog already updated or not found")

if old_approve in content:
    content = content.replace(old_approve, new_approve)
    print("[OK] Updated approve button connection")
    fixed_count += 1
else:
    print("[SKIP] Approve button already updated or not found")

if old_reject in content:
    content = content.replace(old_reject, new_reject)
    print("[OK] Updated reject button connection")
    fixed_count += 1
else:
    print("[SKIP] Reject button already updated or not found")

with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print(f"\n[DONE] Applied {fixed_count}/4 fixes to appealmanager.cpp")
