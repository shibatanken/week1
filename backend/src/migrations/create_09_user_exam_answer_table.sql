CREATE TABLE IF NOT EXISTS user_exam_answer (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_exam_id INT NOT NULL,
    exam_question_id INT NOT NULL,
    selected_answer VARCHAR(10),
    is_correct BOOLEAN DEFAULT FALSE,
    FOREIGN KEY (user_exam_id) REFERENCES user_exam(id) ON DELETE CASCADE,
    FOREIGN KEY (exam_question_id) REFERENCES exam_question(id) ON DELETE CASCADE
);
