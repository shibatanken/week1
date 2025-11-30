CREATE TABLE IF NOT EXISTS user (
    id INT AUTO_INCREMENT PRIMARY KEY,
    email VARCHAR(255) NOT NULL,
    pass VARCHAR(255) NOT NULL,
    name VARCHAR(255) NOT NULL,
    dob DATETIME
);

CREATE TABLE IF NOT EXISTS room (
    id INT AUTO_INCREMENT PRIMARY KEY,
    subject VARCHAR(255) NOT NULL,
    description VARCHAR(255),
    number_of_easy_question INT NOT NULL,
    number_of_medium_question INT NOT NULL,
    number_of_hard_question INT NOT NULL,
    time_limit INT NOT NULL,
    start DATETIME NOT NULL,
    end DATETIME NOT NULL
);

CREATE TABLE IF NOT EXISTS question (
    id INT AUTO_INCREMENT PRIMARY KEY,
    difficulty INT NOT NULL,
    content VARCHAR(255),
    room_id INT NOT NULL,
    FOREIGN KEY (room_id) REFERENCES room(id)
);

CREATE TABLE IF NOT EXISTS answer_of_question (
    id INT AUTO_INCREMENT PRIMARY KEY,
    content VARCHAR(255) NOT NULL,
    is_true BOOLEAN NOT NULL,
    question_id INT NOT NULL,
    FOREIGN KEY (question_id) REFERENCES question(id)
);

CREATE TABLE IF NOT EXISTS practice_session (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    room_id INT NOT NULL,
    start_time DATETIME,
    end_time DATETIME,
    score INT,
    FOREIGN KEY (user_id) REFERENCES user(id),
    FOREIGN KEY (room_id) REFERENCES room(id)
);

CREATE TABLE IF NOT EXISTS exam (
    id INT AUTO_INCREMENT PRIMARY KEY,
    user_id INT NOT NULL,
    room_id INT NOT NULL,
    start_time DATETIME NOT NULL,
    end_time DATETIME,
    score INT NOT NULL DEFAULT 0,
    FOREIGN KEY (user_id) REFERENCES user(id),
    FOREIGN KEY (room_id) REFERENCES room(id)
);

CREATE TABLE IF NOT EXISTS exam_question (
    id INT AUTO_INCREMENT PRIMARY KEY,
    question_id INT NOT NULL,
    exam_id INT NOT NULL,
    user_answer INT,
    FOREIGN KEY (question_id) REFERENCES question(id),
    FOREIGN KEY (exam_id) REFERENCES exam(id)
);

CREATE TABLE IF NOT EXISTS practice_question (
    id INT AUTO_INCREMENT PRIMARY KEY,
    practice_session_id INT NOT NULL,
    question_id INT NOT NULL,
    user_answer INT,
    FOREIGN KEY (practice_session_id) REFERENCES practice_session(id),
    FOREIGN KEY (question_id) REFERENCES question(id)
);

CREATE TABLE IF NOT EXISTS user_in_room (
    id INT AUTO_INCREMENT PRIMARY KEY,
    room_id INT NOT NULL,
    user_id INT NOT NULL,
    FOREIGN KEY (room_id) REFERENCES room(id),
    FOREIGN KEY (user_id) REFERENCES user(id)
);

CREATE TABLE IF NOT EXISTS log (
    id INT AUTO_INCREMENT PRIMARY KEY,
    log_content VARCHAR(255) NOT NULL,
    log_time DATETIME NOT NULL
);

CREATE TABLE IF NOT EXISTS admin (
    id INT AUTO_INCREMENT PRIMARY KEY,
    username VARCHAR(255) NOT NULL,
    pass VARCHAR(255) NOT NULL
);

ALTER TABLE room
ADD COLUMN status ENUM('Not started', 'Ongoing', 'Finished') NOT NULL DEFAULT 'Not started';

