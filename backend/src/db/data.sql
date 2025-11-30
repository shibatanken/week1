INSERT INTO user (email, pass, name, dob) 
VALUES 
    ('phu', '1', 'Phu', '2003-01-1 08:30:00'),
    ('jane.smith@gmail.com', 'securepass456', 'Jane Smith', '1995-05-20 12:00:00'),
    ('alex.brown@gmail.com', 'myp@ssw0rd', 'Alex Brown', '1988-10-30 09:15:00'),
    ('lisa.white@gmail.com', 'qwerty789', 'Lisa White', '1992-03-10 18:45:00');
    
INSERT INTO room (subject, description, number_of_easy_question, number_of_medium_question, number_of_hard_question, time_limit, start, end, status) VALUES
('Mathematics', 'Math quiz for beginners', 5, 3, 2, 30, '2024-12-01 08:00:00', '2024-12-01 09:00:00', 'Not started'),
('Physics', 'Physics quiz for advanced students', 3, 4, 3, 45, '2024-12-02 10:00:00', '2024-12-02 11:00:00', 'Ongoing'),
('History', 'World History quiz', 6, 2, 1, 25, '2024-12-03 15:00:00', '2024-12-03 15:30:00', 'Finished');

INSERT INTO question (difficulty, content, room_id) VALUES
(1, 'What is 2 + 2?', 1),
(2, 'Explain Newton\'s second law.', 2),
(3, 'When did World War II end?', 3);

INSERT INTO answer_of_question (content, is_true, question_id) VALUES
('4', TRUE, 1),
('5', FALSE, 1),
('Force equals mass times acceleration.', TRUE, 2),
('World War II ended in 1946.', FALSE, 3),
('World War II ended in 1945.', TRUE, 3);

INSERT INTO practice_session (user_id, room_id, start_time, end_time, score) VALUES
(1, 1, '2024-12-01 08:00:00', '2024-12-01 08:30:00', 80),
(2, 2, '2024-12-02 10:15:00', '2024-12-02 10:45:00', 90);

INSERT INTO exam (user_id, room_id, start_time, end_time, score) VALUES
(3, 3, '2024-12-03 15:00:00', '2024-12-03 15:30:00', 85);

INSERT INTO exam_question (question_id, exam_id, user_answer) VALUES
(1, 1, 1),
(2, 1, NULL),
(3, 1, 5);

INSERT INTO practice_question (practice_session_id, question_id, user_answer) VALUES
(1, 1, 1),
(1, 2, NULL),
(2, 3, 5);

INSERT INTO user_in_room (room_id, user_id) VALUES
(1, 1),
(2, 2),
(3, 3);
