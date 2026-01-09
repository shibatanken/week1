-- Thêm cột status và role admin cho user
ALTER TABLE user 
ADD COLUMN status ENUM('pending', 'approved', 'rejected') DEFAULT 'approved',
MODIFY COLUMN role ENUM('student', 'teacher', 'admin') NOT NULL DEFAULT 'student';

-- Thêm cột difficulty cho questions
ALTER TABLE questions 
ADD COLUMN difficulty ENUM('easy', 'medium', 'hard') DEFAULT 'medium',
ADD COLUMN category VARCHAR(100) DEFAULT NULL;

