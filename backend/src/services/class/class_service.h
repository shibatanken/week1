#ifndef CLASS_SERVICE_H
#define CLASS_SERVICE_H

// Class management services
int create_class(const char *class_name, const char *description, int teacher_id);
char *get_all_classes();
char *get_class_detail(int class_id);
int add_student_to_class(int user_id, int class_id);
int remove_student_from_class(int user_id, int class_id);
char *get_students_in_class(int class_id);
char *get_students_not_in_class(int class_id);
int delete_class(int class_id, int teacher_id); 
char *get_classes_by_teacher(int teacher_id);
char *get_classes_by_student(int student_id);

#endif // CLASS_SERVICE_H




