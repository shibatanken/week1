CREATE TABLE IF NOT EXISTS exam (
    id INT AUTO_INCREMENT PRIMARY KEY,
    exam_name VARCHAR(255) NOT NULL,
    description TEXT,
    class_id INT NOT NULL,
    time_limit INT NOT NULL DEFAULT 60,
    status ENUM('draft', 'published', 'ongoing', 'finished') NOT NULL DEFAULT 'draft',
    start_time DATETIME,
    end_time DATETIME,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (class_id) REFERENCES class(id) ON DELETE CASCADE
);