#!/usr/bin/env python3
# Fix popup timing in signin.cpp

file_path = 'F:/LTM/week1/frontend/signin.cpp'

with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Find the loginSuccess emit and move it before appeal check
new_lines = []
i = 0
while i < len(lines):
    line = lines[i]

    # Found the appeal check section
    if '// Check for unread appeals' in line:
        # First emit loginSuccess
        new_lines.append('                // First navigate to main screen\n')
        new_lines.append('                emit loginSuccess();\n')
        new_lines.append('\n')
        new_lines.append('                // THEN show popup after delay so main screen loads first\n')
        new_lines.append(line)  # // Check for unread appeals
        i += 1

        # Copy appeal logic but wrap in QTimer
        new_lines.append(lines[i])  # int unreadCount = ...
        i += 1
        new_lines.append(lines[i])  # if (unreadCount > 0) {
        i += 1

        # Copy role/message logic
        while i < len(lines) and 'QMessageBox msgBox' not in lines[i]:
            new_lines.append(lines[i])
            i += 1

        # Add QTimer wrapper
        new_lines.append('\n')
        new_lines.append('                    // Delay 500ms for main screen to load\n')
        new_lines.append('                    QTimer::singleShot(500, this, [this, message]() {\n')
        new_lines.append('                        QMessageBox msgBox;\n')
        i += 1  # Skip original msgBox(this)

        # Copy rest of msgBox setup
        while i < len(lines) and 'emit loginSuccess' not in lines[i]:
            if 'msgBox.' in lines[i] or 'int ret' in lines[i] or 'if (ret' in lines[i] or 'emit showAppealManager' in lines[i] or '}' in lines[i]:
                # Adjust indentation
                new_lines.append('    ' + lines[i])
            else:
                new_lines.append(lines[i])
            i += 1
            if 'showAppealManager' in lines[i-1]:
                break

        # Close QTimer lambda
        new_lines.append('                    });\n')
        new_lines.append('                }\n')

        # Skip old emit loginSuccess section
        while i < len(lines) and 'emit loginSuccess' not in lines[i]:
            i += 1
        i += 1  # Skip emit loginSuccess line

    else:
        new_lines.append(line)
        i += 1

with open(file_path, 'w', encoding='utf-8') as f:
    f.writelines(new_lines)

print("[OK] Fixed popup timing in signin.cpp")
