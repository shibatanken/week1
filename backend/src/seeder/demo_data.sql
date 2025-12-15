-- Demo Data for Class Management System
-- Run this after migrations to populate test data

-- Insert Teacher accounts
INSERT INTO user (email, pass, name, dob, role) VALUES
('teacher1@test.com', '123456', 'Nguyen Van Giang', '1985-05-15', 'teacher'),
('teacher2@test.com', '123456', 'Tran Thi Hoa', '1987-08-20', 'teacher');

-- Insert Student accounts
INSERT INTO user (email, pass, name, dob, role) VALUES
('student1@test.com', '123456', 'Le Van An', '2003-01-10', 'student'),
('student2@test.com', '123456', 'Pham Thi Binh', '2003-02-15', 'student'),
('student3@test.com', '123456', 'Hoang Van Cuong', '2003-03-20', 'student'),
('student4@test.com', '123456', 'Nguyen Thi Dung', '2003-04-25', 'student'),
('student5@test.com', '123456', 'Tran Van Em', '2003-05-30', 'student');

-- Insert Classes (teacher_id = 1 là teacher1@test.com)
INSERT INTO class (class_name, description, teacher_id) VALUES
('Lap trinh mang K65', 'Lop hoc lap trinh mang khoa 65 - Hoc ky 1', 1),
('Co so du lieu K65', 'Lop hoc co so du lieu khoa 65 - Hoc ky 1', 1),
('Cau truc du lieu K66', 'Lop hoc cau truc du lieu khoa 66', 2);

-- Add students to classes
-- Class 1: Lap trinh mang K65 (3 students)
INSERT INTO user_in_class (user_id, class_id) VALUES
(3, 1),  -- Le Van An
(4, 1),  -- Pham Thi Binh
(5, 1);  -- Hoang Van Cuong

-- Class 2: Co so du lieu K65 (2 students)
INSERT INTO user_in_class (user_id, class_id) VALUES
(3, 2),  -- Le Van An
(6, 2);  -- Nguyen Thi Dung

-- Class 3: Cau truc du lieu K66 (1 student)
INSERT INTO user_in_class (user_id, class_id) VALUES
(7, 3);  -- Tran Van Em

