-- Demo Data for Quiz Application
-- Run: mysql -u quizz -pQuizz2003@ quizz_db < demo_data.sql

-- Clear existing data
SET FOREIGN_KEY_CHECKS = 0;
TRUNCATE TABLE appeals;
TRUNCATE TABLE exam_answers;
TRUNCATE TABLE exam_submissions;
TRUNCATE TABLE exam_questions;
TRUNCATE TABLE practice_answers;
TRUNCATE TABLE practice_sessions;
TRUNCATE TABLE questions;
TRUNCATE TABLE exam;
TRUNCATE TABLE user_in_class;
TRUNCATE TABLE class;
TRUNCATE TABLE log;
TRUNCATE TABLE user;
SET FOREIGN_KEY_CHECKS = 1;

-- Insert Users (Admin, Teachers, Students)
INSERT INTO user (email, pass, name, role, status) VALUES
('admin@test.com', '123456', 'Admin User', 'admin', 'approved'),
('teacher1@test.com', '123456', 'Nguyễn Văn Thầy', 'teacher', 'approved'),
('teacher2@test.com', '123456', 'Trần Thị Cô', 'teacher', 'approved'),
('student1@test.com', '123456', 'Lê Văn A', 'student', 'approved'),
('student2@test.com', '123456', 'Phạm Thị B', 'student', 'approved'),
('student3@test.com', '123456', 'Hoàng Văn C', 'student', 'approved'),
('student4@test.com', '123456', 'Ngô Thị D', 'student', 'approved'),
('student5@test.com', '123456', 'Vũ Văn E', 'student', 'approved'),
('pending1@test.com', '123456', 'Người Chờ Duyệt 1', 'student', 'pending'),
('pending2@test.com', '123456', 'Người Chờ Duyệt 2', 'teacher', 'pending');

-- Insert Classes
INSERT INTO class (class_name, description, teacher_id) VALUES
('Lập trình mạng', 'Học về socket, TCP/IP, multi-threading', 2),
('Cơ sở dữ liệu', 'SQL, MySQL, thiết kế database', 2),
('Lập trình C++', 'OOP, STL, templates', 3);

-- Add Students to Classes
INSERT INTO user_in_class (user_id, class_id) VALUES
(4, 1), (5, 1), (6, 1), (7, 1), (8, 1),  -- 5 students in class 1
(4, 2), (5, 2), (6, 2),                   -- 3 students in class 2
(7, 3), (8, 3);                           -- 2 students in class 3

-- Insert Questions to Question Bank (class 1 - Lập trình mạng)
INSERT INTO questions (class_id, content, option_a, option_b, option_c, option_d, correct_option) VALUES
(1, 'Giao thức TCP hoạt động ở tầng nào trong mô hình OSI?', 'Tầng vật lý', 'Tầng liên kết dữ liệu', 'Tầng mạng', 'Tầng giao vận', 'D'),
(1, 'Cổng mặc định của HTTP là gì?', '21', '22', '80', '443', 'C'),
(1, 'Socket là gì?', 'Một loại ổ cắm điện', 'Điểm cuối của kết nối mạng', 'Một giao thức mạng', 'Một loại cable', 'B'),
(1, 'Hàm nào dùng để lắng nghe kết nối trong TCP?', 'connect()', 'bind()', 'listen()', 'accept()', 'C'),
(1, 'UDP khác TCP ở điểm nào?', 'UDP nhanh hơn và không đảm bảo', 'UDP chậm hơn và đảm bảo', 'UDP dùng port, TCP không', 'Không có sự khác biệt', 'A'),
(1, 'Hàm accept() trả về gì?', 'File descriptor của socket mới', 'Địa chỉ IP', 'Số port', 'Không trả về gì', 'A'),
(1, 'Để tạo multi-threaded server, ta dùng thư viện nào?', 'stdlib.h', 'stdio.h', 'pthread.h', 'string.h', 'C'),
(1, 'Mutex dùng để làm gì?', 'Tăng tốc độ xử lý', 'Đồng bộ hóa luồng', 'Tạo kết nối mạng', 'Mã hóa dữ liệu', 'B'),
(1, 'Deadlock xảy ra khi nào?', 'Khi server quá tải', 'Khi 2 luồng chờ lẫn nhau', 'Khi mất kết nối', 'Khi hết bộ nhớ', 'B'),
(1, 'Hàm nào gửi dữ liệu qua TCP socket?', 'read()', 'write()', 'send()', 'B và C đều đúng', 'D');

-- Insert Questions to Question Bank (class 2 - Cơ sở dữ liệu)
INSERT INTO questions (class_id, content, option_a, option_b, option_c, option_d, correct_option) VALUES
(2, 'SQL là viết tắt của gì?', 'Structured Query Language', 'Simple Query Language', 'Standard Query Logic', 'System Query Language', 'A'),
(2, 'Lệnh nào dùng để thêm dữ liệu?', 'SELECT', 'INSERT', 'UPDATE', 'DELETE', 'B'),
(2, 'PRIMARY KEY có thể NULL không?', 'Có', 'Không', 'Tùy thuộc vào hệ quản trị', 'Chỉ một giá trị NULL', 'B'),
(2, 'JOIN nào lấy tất cả dữ liệu từ bảng bên trái?', 'INNER JOIN', 'LEFT JOIN', 'RIGHT JOIN', 'CROSS JOIN', 'B'),
(2, 'Index giúp gì cho database?', 'Tăng dung lượng lưu trữ', 'Tăng tốc độ truy vấn', 'Bảo mật dữ liệu', 'Backup tự động', 'B');

-- Create Exams
INSERT INTO exam (exam_name, description, class_id, time_limit, status, start_time, end_time) VALUES
('Kiểm tra giữa kỳ Lập trình mạng', 'Bài kiểm tra 30 phút', 1, 30, 'published', NULL, NULL),
('Kiểm tra cuối kỳ Lập trình mạng', 'Bài kiểm tra 60 phút', 1, 60, 'draft', NULL, NULL),
('Quiz CSDL', 'Quiz nhanh 15 phút', 2, 15, 'finished', '2026-01-01 08:00:00', '2026-01-01 09:00:00');

-- Add Questions to Exam 1 (from bank)
INSERT INTO exam_questions (exam_id, content, option_a, option_b, option_c, option_d, correct_option)
SELECT 1, content, option_a, option_b, option_c, option_d, correct_option FROM questions WHERE class_id = 1 LIMIT 5;

-- Add Questions to Exam 3 (from bank)
INSERT INTO exam_questions (exam_id, content, option_a, option_b, option_c, option_d, correct_option)
SELECT 3, content, option_a, option_b, option_c, option_d, correct_option FROM questions WHERE class_id = 2 LIMIT 3;

-- Create some exam submissions for Exam 3 (finished)
INSERT INTO exam_submissions (exam_id, user_id, score, total_questions, correct_answers, status, started_at, submitted_at) VALUES
(3, 4, 66.67, 3, 2, 'submitted', '2026-01-01 08:05:00', '2026-01-01 08:15:00'),
(3, 5, 100.00, 3, 3, 'submitted', '2026-01-01 08:03:00', '2026-01-01 08:12:00'),
(3, 6, 33.33, 3, 1, 'submitted', '2026-01-01 08:10:00', '2026-01-01 08:25:00');

-- Create exam answers for submissions
INSERT INTO exam_answers (submission_id, question_id, user_answer, is_correct) VALUES
(1, 6, 'A', 1), (1, 7, 'B', 1), (1, 8, 'C', 0),
(2, 6, 'A', 1), (2, 7, 'B', 1), (2, 8, 'B', 1),
(3, 6, 'A', 1), (3, 7, 'C', 0), (3, 8, 'D', 0);

-- Create an appeal
INSERT INTO appeals (submission_id, question_id, user_id, reason, status, created_at) VALUES
(1, 8, 4, 'Tôi nghĩ đáp án C cũng có thể đúng vì theo sách giáo trình trang 45...', 'pending', NOW()),
(3, 8, 6, 'Câu hỏi không rõ ràng, đề nghị xem xét lại.', 'pending', NOW());

-- Log some activities
INSERT INTO log (log_content, log_time) VALUES
('Demo data inserted', NOW()),
('Server started', NOW());

SELECT 'Demo data inserted successfully!' AS status;
SELECT CONCAT('Users: ', COUNT(*)) AS info FROM user;
SELECT CONCAT('Classes: ', COUNT(*)) AS info FROM class;
SELECT CONCAT('Questions: ', COUNT(*)) AS info FROM questions;
SELECT CONCAT('Exams: ', COUNT(*)) AS info FROM exam;
