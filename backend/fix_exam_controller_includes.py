#!/usr/bin/env python3
# Add missing MySQL include

file_path = 'F:/LTM/week1/backend/src/controllers/exam/exam_controller.c'

with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

old_includes = '''#include "exam_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <time.h>'''

new_includes = '''#include "exam_controller.h"
#include "../../data_structures/index.h"
#include "../../services/service.h"
#include "../../utils/json_utils.h"
#include "../../db/connect-db.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>
#include <cjson/cJSON.h>
#include <time.h>'''

if old_includes in content:
    content = content.replace(old_includes, new_includes)
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(content)
    print("[OK] Added MySQL includes to exam_controller.c")
else:
    print("[SKIP] Already has includes or pattern not found")
