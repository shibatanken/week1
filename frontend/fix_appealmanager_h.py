#!/usr/bin/env python3
# Update appealmanager.h signature

file_path = 'F:/LTM/week1/frontend/appealmanager.h'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_sig = '    void onReviewAppeal(int appealId, QString status);'
new_sig = '    void onReviewAppeal(int appealId, QString status, double currentScore, QString examName);'

if old_sig in content:
    content = content.replace(old_sig, new_sig)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Updated appealmanager.h signature")
else:
    print("[SKIP] Signature already updated or not found")
