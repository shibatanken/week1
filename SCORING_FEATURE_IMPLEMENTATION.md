# Scoring Feature Implementation Summary

## Overview
Added complete exam scoring functionality with answer submission, score calculation, and results viewing for both students and teachers.

## Database Changes

### New Tables Created

#### 1. user_exam table
```sql
CREATE TABLE IF NOT EXISTS user_exam (
    id INT AUTO_INCREMENT PRIMARY KEY,
    exam_id INT NOT NULL,
    user_id INT NOT NULL,
    start_time DATETIME NOT NULL,
    end_time DATETIME,
    score DECIMAL(5, 2) DEFAULT 0,
    correct_count INT DEFAULT 0,
    wrong_count INT DEFAULT 0,
    total_questions INT DEFAULT 0,
    submitted_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (exam_id) REFERENCES exam(id) ON DELETE CASCADE,
    FOREIGN KEY (user_id) REFERENCES user(id) ON DELETE CASCADE,
    UNIQUE KEY unique_user_exam (exam_id, user_id)
);
```

#### 2. user_exam_answer table
```sql
CREATE TABLE IF NOT EXISTS user_exam_answer (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_exam_id INT NOT NULL,
    exam_question_id INT NOT NULL,
    selected_answer VARCHAR(10),
    is_correct BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (user_exam_id) REFERENCES user_exam(id) ON DELETE CASCADE,
    FOREIGN KEY (exam_question_id) REFERENCES exam_question(id) ON DELETE CASCADE);
```

### Migration Files
- `backend/src/migrations/create_08_user_exam_table.sql`
- `backend/src/migrations/create_09_user_exam_answer_table.sql`

## API Endpoints

### 1. SUBMIT_EXAM_ANSWERS
**Purpose:** Submit exam answers and calculate score

**Request:**
```
CONTROL SUBMIT_EXAM_ANSWERS
{
  "exam_id": 50,
  "user_id": 101,
  "start_time": "2025-12-15T08:00:00",
  "end_time": "2025-12-15T08:42:15",
  "answers": [
    { "exam_question_id": 205, "answer": "B" },
    { "exam_question_id": 206, "answer": "A" }
  ]
}
```

**Response (Success):**
```
NOTIFICATION SUBMIT_EXAM_SUCCESS 2025-12-31T...
{"message": "Exam submitted successfully", "user_exam_id": 123}
```

**How it works:**
1. Parses the JSON answers array
2. Creates a `user_exam` record with start/end times
3. For each answer:
   - Converts letter answer ("A", "B", etc.) to answer index
   - Checks if the answer is correct by comparing with `exam_answer` table
   - Stores answer in `user_exam_answer` table
4. Calculates score percentage: `(correct_count / total_questions) * 100`
5. Updates `user_exam` with `score`, `correct_count`, `wrong_count`

### 2. GET_EXAM_RESULTS
**Purpose:** Get student's own exam results

**Request:**
```
CONTROL GET_EXAM_RESULTS
{
  "exam_id": 50,
  "user_id": 101
}
```

**Response:**
```
DATA JSON EXAM_RESULTS
{
  "user_exam_id": 123,
  "score": 85.50,
  "correct_count": 17,
  "wrong_count": 3,
  "total_questions": 20,
  "start_time": "2025-12-15 08:00:00",
  "end_time": "2025-12-15 08:42:15",
  "submitted_at": "2025-12-15 08:42:20",
  "exam_name": "Midterm Exam"
}
```

### 3. GET_EXAM_SCORES
**Purpose:** Get all student scores for an exam (teacher view)

**Request:**
```
CONTROL GET_EXAM_SCORES
{
  "exam_id": 50
}
```

**Response:**
```
DATA JSON EXAM_SCORES
{
  "data": [
    {
      "user_id": 101,
      "name": "John Doe",
      "email": "john@example.com",
      "score": 95.00,
      "correct_count": 19,
      "wrong_count": 1,
      "total_questions": 20,
      "submitted_at": "2025-12-15 08:30:00"
    },
    {
      "user_id": 102,
      "name": "Jane Smith",
      "email": "jane@example.com",
      "score": 85.00,
      "correct_count": 17,
      "wrong_count": 3,
      "total_questions": 20,
      "submitted_at": "2025-12-15 08:45:00"
    }
  ]
}
```

**Note:** Results are ordered by score (descending), then by submission time (ascending).

## Files Modified/Created

### New Message Types
- `backend/src/data_structures/control_message.h`
  - Added: `SUBMIT_EXAM_ANSWERS`, `GET_EXAM_RESULTS`, `GET_EXAM_SCORES`

### Service Layer
- `backend/src/services/exam/exam_service.h` - Added function declarations
- `backend/src/services/exam/exam_service.c` - Added 3 new functions:
  - `submit_exam_answers()` - ~160 lines
  - `get_exam_results()` - ~45 lines
  - `get_exam_scores()` - ~45 lines

### Controller Layer
- `backend/src/controllers/exam/exam_controller.h` - Added handler declarations
- `backend/src/controllers/exam/exam_controller.c` - Added 3 new handlers:
  - `handle_submit_exam_answers()` - ~55 lines
  - `handle_get_exam_results()` - ~35 lines
  - `handle_get_exam_scores()` - ~35 lines

### Routing
- `backend/src/routes/routes.c` - Added routing for 3 new endpoints

## How to Deploy

### 1. Run Migrations
```bash
cd backend
make migrate
# This will run all migration files including the 2 new tables
```

### 2. Build
```bash
cd backend
make clean && make
```

### 3. Run Server
```bash
./server
```

## Score Calculation Logic

The scoring system uses letter-based answers (A, B, C, D, etc.):

1. **Answer Storage:** Answers in `exam_answer` table are ordered by `id`
2. **Letter to Index:** "A" = 0, "B" = 1, "C" = 2, etc.
3. **Validation:** For each submitted answer:
   - Query all answers for that question ordered by `id`
   - Find the answer at the letter's index
   - Check if `is_correct = TRUE`
4. **Score Calculation:**
   - `score = (correct_count / total_questions) * 100`
   - Stored as DECIMAL(5,2) for precision (e.g., 85.50%)

## Frontend Integration (Next Steps)

### Student View - Results Page
Display after exam submission:
```
Your Results for [Exam Name]
━━━━━━━━━━━━━━━━━━━━━━━━━━
Score: 85.50%
Correct: 17 / 20
Wrong: 3 / 20

Time: 42 minutes 15 seconds
Submitted: Dec 15, 2025 8:42 AM
```

### Teacher View - Scores Dashboard
```
Exam Scores - [Exam Name]
━━━━━━━━━━━━━━━━━━━━━━━━━━
| Name       | Email          | Score  | Correct | Submitted       |
|------------|----------------|--------|---------|-----------------|
| John Doe   | john@test.com  | 95.00% | 19/20   | Dec 15, 8:30 AM |
| Jane Smith | jane@test.com  | 85.00% | 17/20   | Dec 15, 8:45 AM |
```

## Testing

### Manual Test with netcat/telnet:
```bash
# 1. Submit exam answers
printf "CONTROL SUBMIT_EXAM_ANSWERS\n{\"exam_id\": 1, \"user_id\": 1, \"start_time\": \"2025-12-15T08:00:00\", \"end_time\": \"2025-12-15T08:42:15\", \"answers\": [{\"exam_question_id\": 1, \"answer\": \"A\"}, {\"exam_question_id\": 2, \"answer\": \"B\"}]}" | nc localhost 8081

# 2. Get student results
printf "CONTROL GET_EXAM_RESULTS\n{\"exam_id\": 1, \"user_id\": 1}" | nc localhost 8081

# 3. Get all scores (teacher)
printf "CONTROL GET_EXAM_SCORES\n{\"exam_id\": 1}" | nc localhost 8081
```

## Code Statistics
- **New Tables:** 2
- **New Endpoints:** 3
- **New Service Functions:** 3 (~250 lines)
- **New Controller Handlers:** 3 (~125 lines)
- **Total New Code:** ~400 lines

## Notes
- Score is stored with 2 decimal precision (e.g., 85.50%)
- One user can only submit once per exam (UNIQUE constraint on exam_id, user_id)
- Resubmission will update the existing record
- Answers are validated against correct answers in real-time
- Teacher view shows students sorted by score (highest first)
