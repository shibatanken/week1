# Frontend Scoring Feature - Integration Guide

## Files Created

### Student Results View
- `frontend/examresults.h` - Header file
- `frontend/examresults.cpp` - Implementation
- `frontend/examresults.ui` - UI layout

### Teacher Scores Dashboard
- `frontend/examscores.h` - Header file
- `frontend/examscores.cpp` - Implementation
- `frontend/examscores.ui` - UI layout

### Updated
- `frontend/frontend.pro` - Added new files to build

## How to Use These Pages

### 1. ExamResults (Student View)

**Purpose:** Display student's exam results after submission

**Usage:**
```cpp
// In your exam submission page or exam list
#include "examresults.h"

// After student submits exam or clicks "View Results"
ExamResults *resultsPage = new ExamResults(this);
resultsPage->loadResults(examId, userId);
resultsPage->show();

// Connect back signal
connect(resultsPage, &ExamResults::backToExamList, this, &YourPage::handleBack);
```

**What it shows:**
- Exam name
- Score percentage (colored: green ≥80%, orange ≥50%, red <50%)
- Correct answers count
- Wrong answers count
- Start and end times

### 2. ExamScores (Teacher View)

**Purpose:** Show all student scores for an exam

**Usage:**
```cpp
// In your exam management page (teacher only)
#include "examscores.h"

// When teacher clicks "View Scores" for an exam
ExamScores *scoresPage = new ExamScores(this);
scoresPage->loadScores(examId);
scoresPage->show();

// Connect back signal
connect(scoresPage, &ExamScores::backToExamList, this, &YourPage::handleBack);
```

**What it shows:**
- Table with columns: Name, Email, Score, Correct, Wrong, Submitted At
- Student count
- Refresh button
- Scores are color-coded (same as student view)
- Sortable columns

## Integration Steps

### Step 1: Add to MainWindow or Navigation

You need to integrate these pages into your existing navigation. Here's how:

#### Option A: Add to Exam List Page

```cpp
// In your exam list page (e.g., classdetail.cpp or similar)

// For Students - After they finish an exam
void YourExamListPage::onExamCompleted(int examId) {
    ExamResults *results = new ExamResults(this);
    results->loadResults(examId, UserData::instance().getUserId());

    // Add to stacked widget or show as dialog
    stackedWidget->addWidget(results);
    stackedWidget->setCurrentWidget(results);

    connect(results, &ExamResults::backToExamList, this, [this]() {
        stackedWidget->removeWidget(stackedWidget->currentWidget());
    });
}

// For Teachers - View all scores button
void YourExamListPage::on_viewScoresButton_clicked() {
    int examId = getSelectedExamId(); // Your method to get selected exam

    ExamScores *scores = new ExamScores(this);
    scores->loadScores(examId);

    stackedWidget->addWidget(scores);
    stackedWidget->setCurrentWidget(scores);

    connect(scores, &ExamScores::backToExamList, this, [this]() {
        stackedWidget->removeWidget(stackedWidget->currentWidget());
    });
}
```

#### Option B: Add buttons to existing exam pages

In your exam detail or exam list UI, add:

**For Students:**
```cpp
// Button: "View My Results"
// Visible only after student has submitted the exam
void on_viewResultsButton_clicked() {
    ExamResults *results = new ExamResults(this);
    results->loadResults(currentExamId, UserData::instance().getUserId());
    results->show();
}
```

**For Teachers:**
```cpp
// Button: "View Student Scores"
// Visible only for teachers
void on_viewScoresButton_clicked() {
    ExamScores *scores = new ExamScores(this);
    scores->loadScores(currentExamId);
    scores->show();
}
```

### Step 2: Check User Role

Make sure to show the right page based on user role:

```cpp
#include "userdata.h"

if (UserData::instance().isTeacher()) {
    // Show ExamScores (teacher view)
    ExamScores *scores = new ExamScores(this);
    scores->loadScores(examId);
    scores->show();
} else {
    // Show ExamResults (student view)
    ExamResults *results = new ExamResults(this);
    results->loadResults(examId, UserData::instance().getUserId());
    results->show();
}
```

## Build Instructions

```bash
cd frontend

# Generate Makefile
qmake frontend.pro

# Build
make

# Run
./frontend        # Linux/WSL
# or
./frontend.app/Contents/MacOS/frontend  # macOS
```

## API Calls Made

### ExamResults
- **Endpoint:** `GET_EXAM_RESULTS`
- **Request:** `{"exam_id": X, "user_id": Y}`
- **Response:** JSON with score, correct_count, wrong_count, etc.

### ExamScores
- **Endpoint:** `GET_EXAM_SCORES`
- **Request:** `{"exam_id": X}`
- **Response:** JSON array of student scores

## Screenshots (What to Expect)

### Student Results Page
```
┌──────────────────────────────────┐
│     Your Exam Results            │
│       Math Quiz                  │
├──────────────────────────────────┤
│ Score:          85.50%           │ (green/orange/red)
│ Correct:        17 / 20          │ (green)
│ Wrong:          3 / 20           │ (red)
│ Start Time:     10:00:00         │
│ End Time:       10:30:00         │
│                                  │
│ [Back to Exam List]              │
└──────────────────────────────────┘
```

### Teacher Scores Dashboard
```
┌────────────────────────────────────────────────────────────┐
│           Student Exam Scores                              │
├────────────────────────────────────────────────────────────┤
│ Total Students: 25                      [Refresh]          │
├────────┬──────────────┬───────┬─────────┬───────┬──────────┤
│ Name   │ Email        │ Score │ Correct │ Wrong │ Submitted│
├────────┼──────────────┼───────┼─────────┼───────┼──────────┤
│ John   │ john@test    │ 95%   │ 19/20   │ 1/20  │ 8:30 AM  │
│ Jane   │ jane@test    │ 85%   │ 17/20   │ 3/20  │ 8:45 AM  │
│ ...    │ ...          │ ...   │ ...     │ ...   │ ...      │
└────────┴──────────────┴───────┴─────────┴───────┴──────────┘
│                [Back to Exam List]                         │
└────────────────────────────────────────────────────────────┘
```

## Next Steps

1. Decide where to add these pages in your navigation flow
2. Add buttons/menu items to access them
3. Test with real exam data
4. Optionally add more features:
   - Export scores to CSV
   - Print results
   - Email results
   - Detailed answer review (show which questions were wrong)

## Notes

- Both pages automatically connect to `SERVER_IP:SERVER_PORT` from [config.h](frontend/config.h)
- Socket connection is handled internally
- Error messages shown via QMessageBox
- Tables are sortable by clicking column headers
- Colors update dynamically based on score thresholds
