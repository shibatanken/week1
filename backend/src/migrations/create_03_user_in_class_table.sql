-- Migration: Create user_in_class table (Many-to-Many)
-- Description: Quản lý sinh viên trong lớp học

CREATE TABLE IF NOT EXISTS user_in_class (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    class_id INT NOT NULL,
    joined_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES user(id) ON DELETE CASCADE,
    FOREIGN KEY (class_id) REFERENCES class(id) ON DELETE CASCADE,
    UNIQUE KEY unique_user_class (user_id, class_id) -- Mỗi user chỉ join 1 lần
);


